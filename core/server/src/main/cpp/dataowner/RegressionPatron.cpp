/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */

/* 3rd Party Headers */
#include <mpc/Randomness.h>
#include <mpc/RandomnessDealer.h>

/* Safrn Headers */
#include <dataowner/RegressionPatron.h>
#include <dealer/RandomTableLookup.h>

/* logging configuration */
#include <ff/logging.h>

namespace safrn {
namespace dataowner {

RegressionRandomnessPatron::RegressionRandomnessPatron(
    RegressionInfo const * const info,
    const safrn::Identity * dealerIdentity,
    const size_t F_rows,
    const size_t t_rows,
    dealer::RandomTableLookupInfo const * const F_info,
    dealer::RandomTableLookupInfo const * const t_info,
    const size_t dispenserSize) :
    regressionDispenser(
        new ff::mpc::RandomnessDispenser<
            RegressionRandomness,
            ff::mpc::DoNotGenerateInfo>(ff::mpc::DoNotGenerateInfo())),
    info(info),
    dealerIdentity(dealerIdentity),
    F_rows(F_rows),
    t_rows(t_rows),
    F_info(F_info),
    t_info(t_info),
    dispenserSize(
        dispenserSize), // dispenserSize = num Regressions we're going to need
    numModConvUpNeeded(
        (this->info->num_IVs * this->info->num_IVs +
         this->info->num_IVs + 3)),
    numDivideNeeded(
        this->info->num_IVs + 2 + this->info->num_IVs + 1 +
        this->info
            ->num_IVs), // 2 for R^2 and MSE, num_IVs for SE_coeffs, 1 for F-statistic, num_IVs for t-statistics
    numConditionalEvaluateNeeded(1),
    numBeaverTripleForFactoryNeeded(
        (this->info->zipAdjacentInfo.batchSize - 1) *
        (this->info->verticalNonDV_numIVs *
         (this->info->verticalDV_numIVs + 1))),
    numBeaverTripleForMatrixMultiplyNeeded(
        this->info->num_IVs * this->info->num_IVs *
        (this->info->num_IVs + 1)),
    numRandomSquareMatrixNeeded(1),
    numBeaverTripleForFinalMultiplyNeeded(
        this->info->num_IVs +
        2 *
            this->info
                ->num_IVs + // for negative one multiplication twice
        this->info->num_IVs * this->info->num_IVs +
        2 * this->info->num_IVs + 2 +
        this->info->num_IVs * this->info->num_IVs +
        3 * this->info->num_IVs + this->info->num_IVs + 2 +
        this->info->num_IVs + this->info->num_IVs + 1),
    numCompareEndModulusNeeded(
        this->info->num_IVs + 1 + this->info->num_IVs),
    numCompareNeeded(this->F_rows + this->t_rows),
    numTypeCastFromBitNeeded(
        this->info->num_IVs + this->F_rows + this->t_rows + 1 +
        this->info->num_IVs),
    numTableLookupFNeeded(1),
    numTableLookuptNeeded(this->info->num_IVs)
/** lines above for `FirstMultiplyForErrorTerms`,
      `SecondMultiplyForErrorTerms`, `ThirdMultiplyForErrorTerms`,
      `FourthMultiplyForErrorTerms`, in that order */
{
  log_debug("Constructor");
}

void RegressionRandomnessPatron::init() {
  log_debug("Calling init on RegressionPatron");

  this->numCrossParties = 0;

  size_t i = 0;
  this->getPeers().forEachDataowner([&, this](const Identity & other) {
    if (other.vertical != this->getSelf().vertical) {
      this->numCrossParties++;
      this->indexOfCrossParties.insert(
          std::pair<Identity, size_t>(other, i));
      i++;
    }
  });
  log_debug("HERE");
  std::unique_ptr<Fronctocol> patron(
      new ff::mpc::ModConvUpRandomnessPatron<
          SAFRN_TYPES,
          SmallNum,
          LargeNum,
          LargeNum>(
          &this->info->modConvUpInfo,
          dealerIdentity,
          this->numModConvUpNeeded * this->dispenserSize));
  log_debug("HERE");
  this->invoke(std::move(patron), this->getPeers());

  this->state = awaitingModConvUp;
}

void RegressionRandomnessPatron::handleReceive(IncomingMessage & imsg) {
  log_error("RegressionPatron Fronctocol received unexpected "
            "handle receive");
  (void)imsg;
}

void RegressionRandomnessPatron::handleComplete(Fronctocol & f) {
  log_debug("RegressionPatron received handle complete");
  switch (this->state) {
    case awaitingModConvUp: {

      this->modConvUpDispenser =
          std::move(static_cast<ff::mpc::ModConvUpRandomnessPatron<
                        SAFRN_TYPES,
                        SmallNum,
                        LargeNum,
                        LargeNum> &>(f)
                        .modConvUpDispenser);

      std::unique_ptr<Fronctocol> patron(
          new ff::mpc::
              DivideRandomnessPatron<SAFRN_TYPES, LargeNum, SmallNum>(
                  &this->info->divideInfo,
                  dealerIdentity,
                  this->numDivideNeeded * this->dispenserSize));
      this->invoke(std::move(patron), this->getPeers());
      this->state = awaitingDivide;
    } break;
    case awaitingDivide: {
      this->divideDispenser =
          std::move(static_cast<ff::mpc::DivideRandomnessPatron<
                        SAFRN_TYPES,
                        LargeNum,
                        SmallNum> &>(f)
                        .divideDispenser);

      this->getPeers().forEachDataowner(
          [&, this](const Identity & other) {
            if (other.vertical != this->getSelf().vertical) {
              zipAdjacentDispensers.emplace_back(nullptr);

              std::unique_ptr<Fronctocol> patron(
                  new ff::mpc::ZipAdjacentRandomnessPatron<
                      SAFRN_TYPES,
                      LargeNum,
                      SmallNum>(
                      &this->info->zipAdjacentInfo,
                      dealerIdentity,
                      this->numConditionalEvaluateNeeded *
                          this->dispenserSize));

              PeerSet ps = PeerSet();
              ps.add(this->getSelf());
              ps.add(other);
              ps.add(*dealerIdentity);

              this->invoke(std::move(patron), ps);
            }
          });
      this->numPartiesAwaiting = this->numCrossParties;
      this->state = awaitingConditionalEvaluate;
    } break;
    case awaitingConditionalEvaluate: {
      PeerSet ps = f.getPeers(); // should be only one other party
      ps.forEachDataowner([&, this](const Identity & other) {
        if (other.vertical != this->getSelf().vertical) {
          this->zipAdjacentDispensers.at(
              indexOfCrossParties.at(other)) =
              std::move(
                  static_cast<ff::mpc::ZipAdjacentRandomnessPatron<
                      SAFRN_TYPES,
                      LargeNum,
                      SmallNum> &>(f)
                      .zipAdjacentDispenser);
        }
      });

      this->numPartiesAwaiting--;
      if (this->numPartiesAwaiting == 0) {

        arithmeticMultiplyForFactoryDispensers.resize(
            this->numCrossParties);
        this->getPeers().forEachDataowner(
            [&, this](const Identity & other) {
              if (other.vertical != this->getSelf().vertical) {

                std::unique_ptr<Fronctocol> patron(
                    new ff::mpc::RandomnessPatron<
                        SAFRN_TYPES,
                        ff::mpc::BeaverTriple<LargeNum>,
                        ff::mpc::BeaverInfo<LargeNum>>(
                        *dealerIdentity,
                        this->numBeaverTripleForFactoryNeeded *
                            this->dispenserSize,
                        ff::mpc::BeaverInfo<LargeNum>(
                            this->info->startModulus)));

                PeerSet ps = PeerSet();
                ps.add(this->getSelf());
                ps.add(other);
                ps.add(*dealerIdentity);

                this->invoke(std::move(patron), ps);
              }
            });
        this->numPartiesAwaiting = this->numCrossParties;
        this->state = awaitingBeaverTripleForFactory;
      }
    } break;
    case awaitingBeaverTripleForFactory: {
      PeerSet ps = f.getPeers(); // should be only one other party
      ps.forEachDataowner([&, this](const Identity & other) {
        if (other.vertical != this->getSelf().vertical) {

          this->arithmeticMultiplyForFactoryDispensers.at(
              indexOfCrossParties.at(other)) =
              std::move(
                  static_cast<
                      PromiseFronctocol<ff::mpc::RandomnessDispenser<
                          ff::mpc::BeaverTriple<LargeNum>,
                          ff::mpc::BeaverInfo<LargeNum>>> &>(f)
                      .result);
        }
      });

      this->numPartiesAwaiting--;
      if (this->numPartiesAwaiting == 0) {

        std::unique_ptr<Fronctocol> patron(
            new ff::mpc::RandomnessPatron<
                SAFRN_TYPES,
                ff::mpc::BeaverTriple<LargeNum>,
                ff::mpc::BeaverInfo<LargeNum>>(
                *dealerIdentity,
                this->numBeaverTripleForMatrixMultiplyNeeded *
                    this->dispenserSize,
                ff::mpc::BeaverInfo<LargeNum>(this->info->endModulus)));
        this->invoke(std::move(patron), this->getPeers());
        this->state = awaitingBeaverTripleForMatrixMultiply;
      }
    } break;
    case awaitingBeaverTripleForMatrixMultiply: {
      this->arithmeticMultiplyForMatrixDispenser = std::move(
          static_cast<PromiseFronctocol<ff::mpc::RandomnessDispenser<
              ff::mpc::BeaverTriple<LargeNum>,
              ff::mpc::BeaverInfo<LargeNum>>> &>(f)
              .result);

      std::unique_ptr<Fronctocol> patron(
          new ff::mpc::RandomnessPatron<
              SAFRN_TYPES,
              dealer::RandomSquareMatrix<LargeNum>,
              dealer::RandomSquareMatrixInfo<LargeNum, LargeNum>>(
              *dealerIdentity,
              this->numRandomSquareMatrixNeeded * this->dispenserSize,
              dealer::RandomSquareMatrixInfo<LargeNum, LargeNum>(
                  this->info->num_IVs, this->info->endModulus)));
      this->invoke(std::move(patron), this->getPeers());
      this->state = awaitingRandomSquareMatrix;
    } break;
    case awaitingRandomSquareMatrix: {
      log_debug("awaitingRandomSquareMatrix");
      this->randomMatrixAndDetInverseDispenser = std::move(
          static_cast<PromiseFronctocol<ff::mpc::RandomnessDispenser<
              dealer::RandomSquareMatrix<LargeNum>,
              dealer::RandomSquareMatrixInfo<LargeNum, LargeNum>>> &>(f)
              .result);

      std::unique_ptr<Fronctocol> patron(
          new ff::mpc::RandomnessPatron<
              SAFRN_TYPES,
              ff::mpc::BeaverTriple<LargeNum>,
              ff::mpc::BeaverInfo<LargeNum>>(
              *dealerIdentity,
              this->numBeaverTripleForFinalMultiplyNeeded *
                  this->dispenserSize,
              ff::mpc::BeaverInfo<LargeNum>(this->info->endModulus)));
      this->invoke(std::move(patron), this->getPeers());
      this->state = awaitingBeaverTripleForFinalMultiply;
    } break;
    case awaitingBeaverTripleForFinalMultiply: {
      log_debug("awaitingBeaverTripleForFinalMultiply");
      this->arithmeticMultiplyForFinalDispenser = std::move(
          static_cast<PromiseFronctocol<ff::mpc::RandomnessDispenser<
              ff::mpc::BeaverTriple<LargeNum>,
              ff::mpc::BeaverInfo<LargeNum>>> &>(f)
              .result);

      std::unique_ptr<Fronctocol> patron(
          new ff::mpc::
              CompareRandomnessPatron<SAFRN_TYPES, LargeNum, SmallNum>(
                  &this->info->compareInfoEndModulus,
                  dealerIdentity,
                  this->numCompareEndModulusNeeded *
                      this->dispenserSize));
      this->invoke(std::move(patron), this->getPeers());
      this->state = awaitingCompareEndModulus;
    } break;
    case awaitingCompareEndModulus: {
      log_debug("awaitingCompareEndModulus");
      this->compareEndModulusDispenser =
          std::move(static_cast<ff::mpc::CompareRandomnessPatron<
                        SAFRN_TYPES,
                        LargeNum,
                        SmallNum> &>(f)
                        .compareDispenser);

      std::unique_ptr<Fronctocol> patron(
          new ff::mpc::
              CompareRandomnessPatron<SAFRN_TYPES, LargeNum, SmallNum>(
                  &this->info->compareInfo,
                  dealerIdentity,
                  this->numCompareNeeded * this->dispenserSize));
      this->invoke(std::move(patron), this->getPeers());
      this->state = awaitingCompare;
    } break;
    case awaitingCompare: {
      log_debug("awaitingCompare");
      this->compareDispenser =
          std::move(static_cast<ff::mpc::CompareRandomnessPatron<
                        SAFRN_TYPES,
                        LargeNum,
                        SmallNum> &>(f)
                        .compareDispenser);

      std::unique_ptr<Fronctocol> patron(
          new ff::mpc::RandomnessPatron<
              SAFRN_TYPES,
              ff::mpc::TypeCastTriple<LargeNum>,
              ff::mpc::TypeCastFromBitInfo<LargeNum>>(
              *dealerIdentity,
              this->numTypeCastFromBitNeeded * this->dispenserSize,
              ff::mpc::TypeCastFromBitInfo<LargeNum>(
                  this->info->endModulus)));
      this->invoke(std::move(patron), this->getPeers());
      this->state = awaitingTypeCastFromBit;
    } break;
    case awaitingTypeCastFromBit: {
      log_debug("awaitingTypeCastFromBit");
      this->typeCastFromBitDispenser = std::move(
          static_cast<PromiseFronctocol<ff::mpc::RandomnessDispenser<
              ff::mpc::TypeCastTriple<LargeNum>,
              ff::mpc::TypeCastFromBitInfo<LargeNum>>> &>(f)
              .result);
      log_debug(
          "F_info.size(), t_info.size() %u, %u",
          this->F_info->table_size_,
          this->t_info->table_size_);
      ::std::unique_ptr<Fronctocol> patron(new LookupRandomnessPatron(
          this->F_info,
          &this->info->compareInfoEndModulus,
          dealerIdentity,
          this->numTableLookupFNeeded * this->dispenserSize));
      this->invoke(::std::move(patron), this->getPeers());
      this->state = awaitingF_lookup;
    } break;
    case awaitingF_lookup: {
      log_debug("awaitingF_lookup");
      this->F_lookupDispenser = std::move(
          static_cast<LookupRandomnessPatron &>(f).lookupDispenser);

      ::std::unique_ptr<Fronctocol> patron(new LookupRandomnessPatron(
          this->t_info,
          &this->info->compareInfoEndModulus,
          dealerIdentity,
          this->numTableLookuptNeeded * this->dispenserSize));
      this->invoke(::std::move(patron), this->getPeers());
      this->state = awaitingt_lookup;
    } break;
    case awaitingt_lookup: {
      log_debug("awaitingt_lookup");
      this->t_lookupDispenser = std::move(
          static_cast<LookupRandomnessPatron &>(f).lookupDispenser);

      this->generateOutputDispenser();
    } break;
    default:
      log_error("State machine in unexpected state");
  }
}

void RegressionRandomnessPatron::handlePromise(Fronctocol &) {
  log_error("RegressionPatron Fronctocol received unexpected"
            "handle promise");
}

std::string RegressionRandomnessPatron::name() {
  return std::string("Regression Randomness Patron");
}

void RegressionRandomnessPatron::generateOutputDispenser() {
  for (size_t i = 0; i < this->dispenserSize; i++) {

    log_debug("Generating OutputDispenser");

    std::vector<std::unique_ptr<ff::mpc::RandomnessDispenser<
        ff::mpc::ZipAdjacentRandomness<LargeNum, SmallNum>,
        ff::mpc::DoNotGenerateInfo>>>
        littleZipAdjacentDispensers;
    std::vector<std::unique_ptr<ff::mpc::RandomnessDispenser<
        ff::mpc::BeaverTriple<LargeNum>,
        ff::mpc::BeaverInfo<LargeNum>>>>
        littleArithmeticMultiplyForFactoryDispensers;

    littleZipAdjacentDispensers.reserve(this->numCrossParties);
    littleArithmeticMultiplyForFactoryDispensers.reserve(
        this->numCrossParties);
    for (size_t j = 0; j < this->numCrossParties; j++) {
      littleZipAdjacentDispensers.emplace_back(
          std::move(this->zipAdjacentDispensers.at(j)->littleDispenser(
              this->numConditionalEvaluateNeeded)));
      littleArithmeticMultiplyForFactoryDispensers.emplace_back(
          std::move(this->arithmeticMultiplyForFactoryDispensers.at(j)
                        ->littleDispenser(
                            this->numBeaverTripleForFactoryNeeded)));
    }

    this->regressionDispenser->insert(RegressionRandomness(
        std::move(this->modConvUpDispenser->littleDispenser(
            this->numModConvUpNeeded)),
        std::move(this->divideDispenser->littleDispenser(
            this->numDivideNeeded)),
        std::move(littleZipAdjacentDispensers),
        std::move(littleArithmeticMultiplyForFactoryDispensers),
        std::move(
            this->arithmeticMultiplyForMatrixDispenser->littleDispenser(
                this->numBeaverTripleForMatrixMultiplyNeeded)),
        std::move(
            this->randomMatrixAndDetInverseDispenser->littleDispenser(
                this->numRandomSquareMatrixNeeded)),
        std::move(
            this->arithmeticMultiplyForFinalDispenser->littleDispenser(
                this->numBeaverTripleForFinalMultiplyNeeded)),
        std::move(this->compareEndModulusDispenser->littleDispenser(
            this->numCompareEndModulusNeeded)),
        std::move(this->compareDispenser->littleDispenser(
            this->numCompareNeeded)),
        std::move(this->typeCastFromBitDispenser->littleDispenser(
            this->numTypeCastFromBitNeeded)),
        std::move(this->F_lookupDispenser->littleDispenser(
            this->numTableLookupFNeeded)),
        std::move(this->t_lookupDispenser->littleDispenser(
            this->numTableLookuptNeeded))));
  }
  log_debug("calling this->complete");

  this->complete();
}

} // namespace dataowner
} // namespace safrn
