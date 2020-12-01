/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <dataowner/Regression.h>
#include <dealer/RandomTableLookup.h>

#include <mpc/ObservationList.h>

/* logging configuration */
#include <ff/logging.h>

namespace safrn {
namespace dataowner {

Regression::Regression(
    ff::mpc::ObservationList<LargeNum> && olist,
    std::string F_tableFile,
    std::string t_tableFile,
    GlobalInfo const * const globals,
    std::unique_ptr<RegressionInfo const> i) :
    ownList(std::move(olist)),
    F_tableFile(std::move(F_tableFile)),
    t_tableFile(std::move(t_tableFile)),
    globals(globals),
    info(std::move(i)),
    startModulusPayloadVector(
        this->info->num_IVs * this->info->num_IVs +
        this->info->num_IVs + 3),
    matrixShare(this->info->num_IVs * this->info->num_IVs),
    vectorShare(this->info->num_IVs),
    matMultiplyOutput(this->info->num_IVs, this->info->num_IVs + 1),
    F_info(),
    t_info() {
  /** these two calls will populate F/t row_ids, col_ids, table_data */
  if (!dealer::read_table_csv_file(
          this->F_tableFile,
          this->F_row_ids,
          this->F_table_data,
          this->num_F_cols,
          this->F_cols_bits_of_precision,
          this->F_cols_step_size,
          this->info->max_F_t_table_num_rows,
          this->info->bytesInLookupTableCells)) {
    this->abortFlag = true;
  }
  if (!dealer::read_table_csv_file(
          this->t_tableFile,
          this->t_row_ids,
          this->t_table_data,
          this->num_t_cols,
          this->t_cols_bits_of_precision,
          this->t_cols_step_size,
          this->info->max_F_t_table_num_rows,
          this->info->bytesInLookupTableCells)) {
    this->abortFlag = true;
  }

  if (this->abortFlag) {
    return;
  }

  if_debug {
    log_debug(
        "start mod %s, end mod %s",
        ff::mpc::dec(this->info->startModulus).c_str(),
        ff::mpc::dec(this->info->endModulus).c_str());
    for (size_t i = 0; i < this->ownList.elements.size(); i++) {
      for (size_t j = 0; j < this->ownList.numArithmeticPayloadCols;
           j++) {
        log_debug(
            "payload[%zu][%zu] = %s",
            i,
            j,
            ff::mpc::dec(
                this->ownList.elements[i].arithmeticPayloadCols[j])
                .c_str());
      }
    }
  }

  this->computePayloadVectorAndPadList();

  this->setupCrossVerticalShares();
}

void Regression::computePayloadVectorAndPadList() {
  log_debug("Calling computePayloadVectorAndPadList");
  // Issue #220
  log_debug(
      "This->ownList.elements.size() %zu, maxListSize %zu",
      this->ownList.elements.size(),
      this->globals->maxListSize);
  log_debug("this->info->payloadLength %zu", this->info->payloadLength);
  if (this->info->selfVertical == this->info->verticalDV) {
    for (auto & o : this->ownList.elements) {
      o.arithmeticPayloadCols.reserve(this->info->payloadLength);
      // xi*xj
      for (size_t i = 0; i < this->info->verticalDV_numIVs; i++) {
        for (size_t j = i; j < this->info->verticalDV_numIVs; j++) {
          o.arithmeticPayloadCols.push_back(ff::mpc::modMul(
              o.arithmeticPayloadCols[i],
              o.arithmeticPayloadCols[j],
              this->info->startModulus));
        }
      }

      // xi*y
      for (size_t i = 0; i < this->info->verticalDV_numIVs; i++) {

        o.arithmeticPayloadCols.push_back(ff::mpc::modMul(
            o.arithmeticPayloadCols[i],
            o.arithmeticPayloadCols[this->info->verticalDV_numIVs],
            this->info->startModulus));
      }

      //y2
      o.arithmeticPayloadCols.push_back(ff::mpc::modMul(
          o.arithmeticPayloadCols[this->info->verticalDV_numIVs],
          o.arithmeticPayloadCols[this->info->verticalDV_numIVs],
          this->info->startModulus));

      // y
      o.arithmeticPayloadCols.push_back(
          o.arithmeticPayloadCols[this->info->verticalDV_numIVs]);

      // 1
      o.arithmeticPayloadCols.push_back(1U);
      o.arithmeticPayloadCols.resize(this->info->payloadLength);
    }
  } else {
    for (auto & o : this->ownList.elements) {
      o.arithmeticPayloadCols.reserve(this->info->payloadLength);
      for (size_t i = 0; i < this->info->verticalNonDV_numIVs; i++) {
        for (size_t j = i; j < this->info->verticalNonDV_numIVs; j++) {
          o.arithmeticPayloadCols.push_back(ff::mpc::modMul(
              o.arithmeticPayloadCols[i],
              o.arithmeticPayloadCols[j],
              this->info->startModulus));
        }
      }
      o.arithmeticPayloadCols.resize(this->info->payloadLength);
    }
  }

  log_debug(
      "This->ownList.elements.size() %zu, maxListSize %zu",
      this->ownList.elements.size(),
      this->globals->maxListSize);
  this->ownList.numArithmeticPayloadCols = this->info->payloadLength;
  this->ownList.numKeyCols = 2;

  while (this->ownList.elements.size() < this->globals->maxListSize) {
    ff::mpc::Observation<LargeNum> o;
    o.arithmeticPayloadCols =
        std::vector<LargeNum>(this->ownList.numArithmeticPayloadCols);
    o.keyCols = std::vector<LargeNum>(this->ownList.numKeyCols);
    for (size_t i = 0; i < this->ownList.numKeyCols; i++) {
      o.keyCols[i] =
          ff::mpc::randomModP<LargeNum>(this->info->keyModulus);
    }
    o.XORPayloadCols =
        std::vector<Boolean_t>(this->ownList.numXORPayloadCols);
    this->ownList.elements.push_back(o);
  }
}

void Regression::setupCrossParties() {
  log_debug("Calling setupCrossParties");
  this->indexOfCrossParties = std::map<Identity, size_t>();
  size_t i = 0;
  this->getPeers().forEachDataowner([&, this](const Identity & other) {
    if (other.vertical != this->getSelf().vertical) {
      this->indexOfCrossParties.insert(
          std::pair<Identity, size_t>(other, i));
      i++;
    }
  });

  this->numPartiesAwaiting = this->info->numCrossParties;
}

void Regression::setupCrossVerticalShares() {
  log_debug("Calling setupCrossVerticalShares");
  for (size_t i = 0; i < this->info->numCrossParties; i++) {
    this->outgoingListShares.emplace_back();
    this->outgoingListShares.back().elements.reserve(
        this->globals->maxListSize); // just theirs
    this->outgoingListShares.back().numKeyCols =
        this->ownList.numKeyCols;
    this->outgoingListShares.back().numArithmeticPayloadCols =
        this->ownList.numArithmeticPayloadCols;
    this->outgoingListShares.back().numXORPayloadCols =
        this->ownList.numXORPayloadCols;

    this->sharedLists.emplace_back();
    this->sharedLists.back().elements.resize(
        this->globals->maxListSize * 2); // my shares and theirs
    this->sharedLists.back().numKeyCols = this->ownList.numKeyCols;
    this->sharedLists.back().numArithmeticPayloadCols =
        this->ownList.numArithmeticPayloadCols;
    this->sharedLists.back().numXORPayloadCols =
        this->ownList.numXORPayloadCols;

    for (size_t j = 0; j < this->globals->maxListSize; j++) {
      ff::mpc::Observation<LargeNum> o;
      ff::mpc::Observation<LargeNum> o_my_share;
      for (size_t k = 0; k < this->ownList.numKeyCols; k++) {
        auto rand_val =
            ff::mpc::randomModP<LargeNum>(this->info->keyModulus);
        o.keyCols.push_back(rand_val);
        o_my_share.keyCols.push_back(ff::mpc::modSub(
            this->ownList.elements[j].keyCols[k],
            rand_val,
            this->info->keyModulus));
      }
      for (size_t k = 0; k < this->ownList.numArithmeticPayloadCols;
           k++) {
        auto rand_val =
            ff::mpc::randomModP<LargeNum>(this->info->startModulus);
        o.arithmeticPayloadCols.push_back(rand_val);
        o_my_share.arithmeticPayloadCols.push_back(ff::mpc::modSub(
            this->ownList.elements[j].arithmeticPayloadCols[k],
            rand_val,
            this->info->startModulus));
      }
      for (size_t k = 0; k < this->ownList.numXORPayloadCols; k++) {
        auto rand_val = ff::mpc::randomByte();
        o.XORPayloadCols.push_back(rand_val);
        o_my_share.XORPayloadCols.push_back(
            this->ownList.elements[j].XORPayloadCols[k] ^ rand_val);
      }
      this->sharedLists.back().elements
          [j +
           this->globals->maxListSize *
               ((this->info->selfVertical != this->info->verticalDV) ?
                    1 :
                    0)] = std::move(o_my_share);
      this->outgoingListShares.back().elements.push_back(o);
    }
  }
  for (size_t i = 0; i < this->info->numCrossParties; i++) {
    log_debug(
        "shared list size %zu and outgoing list size %zu ",
        this->sharedLists[i].elements.size(),
        this->outgoingListShares[i].elements.size());
  }
  log_debug("Done setupCrossVerticalShares");
}

void Regression::init() {
  log_debug("Calling init");
  log_debug(
      "start modulus: %s, end modulus: %s",
      ff::mpc::dec(this->info->startModulus).c_str(),
      ff::mpc::dec(this->info->endModulus).c_str());

  log_debug(
      "F %zu %s t %zu %s",
      this->F_cols_bits_of_precision,
      ff::mpc::dec(this->F_cols_step_size).c_str(),
      this->t_cols_bits_of_precision,
      ff::mpc::dec(this->t_cols_step_size).c_str());

  this->state = awaitingRandomnessAndSISOSort;
  this->setupCrossParties();

  this->shareWithCrossVerticalParties();

  this->F_info.r_modulus_ = this->info->endModulus;
  this->F_info.table_size_ = this->F_table_data.size();
  this->t_info.r_modulus_ = this->info->endModulus;
  this->t_info.table_size_ = this->t_table_data.size();

  this->invokeRandomnessPatron();
}

void Regression::shareWithCrossVerticalParties() {
  log_debug("Calling shareWithCrossVerticalParties");
  // Issue #220

  for (size_t i = 0; i < this->info->numCrossParties; i++) {
    log_debug(
        "shared list size %zu and outgoing list size %zu ",
        this->sharedLists[i].elements.size(),
        this->outgoingListShares[i].elements.size());
  }
  size_t i = 0;

  this->getPeers().forEachDataowner([&, this](const Identity & other) {
    if (other.vertical != this->getSelf().vertical) {
      std::unique_ptr<OutgoingMessage> omsg(new OutgoingMessage(other));
      for (size_t j = 0; j < this->globals->maxListSize; j++) {
        for (size_t k = 0; k < this->ownList.numKeyCols; k++) {
          omsg->write<LargeNum>(
              this->outgoingListShares[i].elements[j].keyCols[k]);
        }
        for (size_t k = 0; k < this->ownList.numArithmeticPayloadCols;
             k++) {
          omsg->write<LargeNum>(this->outgoingListShares[i]
                                    .elements[j]
                                    .arithmeticPayloadCols[k]);
        }
        for (size_t k = 0; k < this->ownList.numXORPayloadCols; k++) {
          omsg->write<Boolean_t>(this->outgoingListShares[i]
                                     .elements[j]
                                     .XORPayloadCols[k]);
        }
      }
      this->send(std::move(omsg));

      i++;
    }
  });
}

void Regression::invokeRandomnessPatron() {
  log_debug("Calling invokeRandomnessPatron");
  log_debug(
      "F_row_ids.size() and t_row_ids.size() %zu, %zu",
      this->F_row_ids.size(),
      this->t_row_ids.size());

  log_debug(
      "F_info.size(), t_info.size() %u, %u",
      this->F_info.table_size_,
      this->t_info.table_size_);
  std::unique_ptr<RegressionRandomnessPatron> patron(
      new RegressionRandomnessPatron(
          this->info.get(),
          this->info->dealer,
          this->F_row_ids
              .size(), // this isn't in the info object because it depends on the F_table input file
          this->t_row_ids
              .size(), // this isn't in the info object because it depends on the t_table input file
          &this->F_info,
          &this->t_info,
          1UL));
  PeerSet ps(this->getPeers());
  ps.removeRecipients();
  this->invoke(std::move(patron), ps);
}

void Regression::handlePromise(Fronctocol &) {
  log_error("Unexpected handle promise received in Regression");
}

void Regression::handleReceive(IncomingMessage & msg) {
  log_debug(
      "Calling handleReceive with %zu parties remaining",
      this->numPartiesAwaiting);
  // Issue #220
  /** TO-DO: Read in msg to sharedLists[crossPartyIndex][ind1..ind2] */

  for (size_t i = 0; i < this->info->numCrossParties; i++) {
    log_debug(
        "shared list size %zu and outgoing list size %zu ",
        this->sharedLists[i].elements.size(),
        this->outgoingListShares[i].elements.size());
  }

  for (size_t j = 0; j < this->globals->maxListSize; j++) {
    ff::mpc::Observation<LargeNum> o;
    for (size_t k = 0; k < this->ownList.numKeyCols; k++) {
      LargeNum rand_val;
      msg.read<LargeNum>(rand_val);

      o.keyCols.push_back(rand_val);
    }
    for (size_t k = 0; k < this->ownList.numArithmeticPayloadCols;
         k++) {
      LargeNum rand_val;
      msg.read<LargeNum>(rand_val);

      o.arithmeticPayloadCols.push_back(rand_val);
    }
    for (size_t k = 0; k < this->ownList.numXORPayloadCols; k++) {
      Boolean_t rand_val;
      msg.read<Boolean_t>(rand_val);

      o.XORPayloadCols.push_back(rand_val);
    }
    this->sharedLists[indexOfCrossParties[msg.sender]].elements
        [j +
         this->globals->maxListSize *
             ((this->info->selfVertical == this->info->verticalDV) ?
                  1 :
                  0)] = std::move(o);
  }

  this->numPartiesAwaiting--;
  if (this->numPartiesAwaiting == 0) {
    size_t i = 0;
    this->getPeers().forEachDataowner([&,
                                       this](const Identity & other) {
      if (other.vertical != this->getSelf().vertical) {
        log_debug("Num shared lists %zu", this->sharedLists.size());
        log_debug(
            "size before %zu", this->sharedLists[i].elements.size());

        safrn::Identity const * temp_revealer = nullptr;
        if (this->info->selfVertical == this->info->verticalDV) {
          temp_revealer = &this->getSelf();
        } else {
          temp_revealer = &other;
        }
        log_debug("preparingSISOSort");
        std::unique_ptr<Fronctocol> siso_sort(new SISOSort(
            this->sharedLists[i],
            this->info->startModulus,
            this->info->keyModulus,
            temp_revealer,
            this->info->dealer));

        PeerSet ps = PeerSet();
        ps.add(this->getSelf());
        ps.add(other);
        this->getPeers().forEachDealer(
            [&ps](const Identity & other2) { ps.add(other2); });
        this->invoke(std::move(siso_sort), ps);
        i++;
      }
    });
    this->numPartiesAwaiting = this->info->numCrossParties;
    this->state = awaitingSISOSort;
    log_debug("awaitingSISOSort");
  }
}

void Regression::handleComplete(Fronctocol & f) {
  log_debug("Calling handleComplete");
  if (!this->randomnessDone) {
    log_debug("randomness?");
    auto * patron = dynamic_cast<RegressionRandomnessPatron *>(&f);
    if (patron == nullptr) {
      log_debug("no");
      // i.e. fronctocol returned = SISOSORT
      this->numPartiesAwaiting--;
      if (this->numPartiesAwaiting == 0) {
        this->state = awaitingRandomnessOnly;
      }
      return;
    } else {
      log_debug("yes");
      randomness = std::move(patron->regressionDispenser->get());
      log_debug("Move onto awaitingSISOSort");
      this->randomnessDone = true;
      if (this->state == awaitingRandomnessOnly) {
        this->numPartiesAwaiting = 1; // i.e. this one
        this->state = awaitingSISOSort;
      } else {
        this->state = awaitingSISOSort;
        return;
      }
    }
  }
  switch (this->state) {
    case (awaitingSISOSort): {
      log_debug("awaitingSISOSort");

      //Issue #221
      this->numPartiesAwaiting--;
      if (this->numPartiesAwaiting == 0) {

        log_debug("and onto batchedPayloadCompute");
        size_t i = 0;
        this->zipAdjacentInfo.reserve(this->info->numCrossParties);

        this->getPeers().forEachDataowner([&, this](
                                              const Identity & other) {
          if (other.vertical != this->getSelf().vertical) {

            safrn::Identity const * temp_revealer = nullptr;
            if (this->info->selfVertical == this->info->verticalDV) {
              temp_revealer = &this->getSelf();
            } else {
              temp_revealer = &other;
            }
            log_assert(this->info != nullptr);

            zipAdjacentInfo.emplace_back(
                2 * this->globals->maxListSize,
                this->info->payloadLength,
                0,
                this->info->startModulus,
                temp_revealer);

            std::unique_ptr<Fronctocol> zipAdj(
                new ff::mpc::
                    ZipAdjacent<SAFRN_TYPES, LargeNum, SmallNum>(
                        sharedLists[i],
                        &zipAdjacentInfo.back(),
                        std::move(
                            this->randomness.zipAdjacentDispensers[i]
                                ->get())));
            PeerSet ps = PeerSet();
            ps.add(this->getSelf());
            ps.add(other);

            this->invoke(std::move(zipAdj), ps);
            i++;
          }
        });
        log_debug("Done invoking batchedPayloadCompute");
        this->numPartiesAwaiting = this->info->numCrossParties;

        vectorZippedAdjacent.resize(this->info->numCrossParties);
        this->state = awaitingZipAdjacent;
      }

    } break;
    case (awaitingZipAdjacent): {
      log_debug("awaitingZipAdjacent");

      PeerSet psOther = f.getPeers();

      Identity cross_party;

      psOther.forEachDataowner([&, this](const Identity & other) {
        if (other != this->getSelf()) {
          cross_party = other;
        }
      });

      //Issue #221
      this->vectorZippedAdjacent[indexOfCrossParties[cross_party]] =
          static_cast<
              ff::mpc::ZipAdjacent<SAFRN_TYPES, LargeNum, SmallNum> &>(
              f)
              .zippedAdjacentPairs;

      //Issue #221
      this->numPartiesAwaiting--;
      if (this->numPartiesAwaiting == 0) {
        log_debug("and onto zipReduce");
        size_t i = 0;
        this->fronctocolFactories.reserve(this->info->numCrossParties);
        this->factoryMultiplyInfo.reserve(this->info->numCrossParties);

        this->getPeers().forEachDataowner([&, this](
                                              const Identity & other) {
          if (other.vertical != this->getSelf().vertical) {

            safrn::Identity const * temp_revealer = nullptr;
            if (this->info->selfVertical == this->info->verticalDV) {
              temp_revealer = &this->getSelf();
            } else {
              temp_revealer = &other;
            }
            log_debug("About to crash?");
            log_assert(this->info != nullptr);
            this->factoryMultiplyInfo.emplace_back(
                temp_revealer,
                BeaverInfo<LargeNum>(this->info->startModulus));
            log_debug(
                "Address of dispenser %p",
                randomness.beaverTripleForFactoryDispensers[i].get());
            this->fronctocolFactories.emplace_back(
                this->info.get(),
                std::move(
                    randomness.beaverTripleForFactoryDispensers[i]),
                &this->factoryMultiplyInfo.back());
            log_assert(this->fronctocolFactories.size() == i + 1);
            log_debug("address %p", &this->fronctocolFactories[i]);

            std::unique_ptr<Fronctocol> zipRed(
                new ff::mpc::ZipReduce<SAFRN_TYPES, LargeNum>(
                    std::move(vectorZippedAdjacent[i]),
                    this->fronctocolFactories[i]));

            PeerSet ps = PeerSet();
            ps.add(this->getSelf());
            ps.add(other);

            this->invoke(std::move(zipRed), ps);
            i++;
          }
        });
        log_debug("Done invoking batchedZipreduce");
        this->numPartiesAwaiting = this->info->numCrossParties;
        this->state = awaitingZipReduce;
      }

    } break;
    case (awaitingZipReduce): {

      ff::mpc::ObservationList<LargeNum> payloadShares =
          static_cast<ff::mpc::ZipReduce<SAFRN_TYPES, LargeNum> &>(f)
              .outputList;

      for (size_t i = 0; i < this->globals->maxListSize * 2 - 1; i++) {
        for (size_t j = 0;
             j < this->info->num_IVs * this->info->num_IVs +
                 this->info->num_IVs + 3;
             j++) {
          this->startModulusPayloadVector[j] +=
              payloadShares.elements[i].arithmeticPayloadCols[j];
          this->startModulusPayloadVector[j] %=
              this->info->startModulus;
        }
      }

      this->numPartiesAwaiting--;

      if (this->numPartiesAwaiting == 0) {
        for (size_t i = 0; i < this->startModulusPayloadVector.size();
             i++) {

          this->startModulusPayloadVector[i] =
              this->startModulusPayloadVector[i] %
              this->info->startModulus;
          log_debug(
              "Shares of A^TA_and_A^Ty_mod %s :S[%zu]= %s",
              ff::mpc::dec(this->info->startModulus).c_str(),
              i,
              ff::mpc::dec(this->startModulusPayloadVector[i]).c_str());
        }

        log_debug("and onto modconvup");
        /** Issue #223 */
        std::unique_ptr<ff::mpc::Batch<SAFRN_TYPES>> batchedModConv(
            new ff::mpc::Batch<SAFRN_TYPES>());
        for (size_t i = 0;
             i < info->num_IVs * info->num_IVs + info->num_IVs + 3;
             i++) {
          batchedModConv->children.emplace_back(
              new ModConvUp<SmallNum, LargeNum, LargeNum>(
                  this->startModulusPayloadVector[i],
                  &this->info->modConvUpInfo,
                  std::move(
                      this->randomness.modConvUpDispenser->get())));
        }

        PeerSet ps(this->getPeers());
        ps.removeDealer();
        ps.removeRecipients();
        this->invoke(std::move(batchedModConv), ps);
        this->state = awaitingBatchedModConvUp;
      }
    } break;
    case (awaitingBatchedModConvUp): {
      log_debug("awaitingBatchedModConvUp");
      auto & batch = static_cast<ff::mpc::Batch<SAFRN_TYPES> &>(f);

      for (size_t i = 0; i < batch.children.size(); i++) {
        log_debug(
            "Shares of A^TA and A^T y mod %s : S[%zu]=\n%s",
            ff::mpc::dec(this->info->endModulus).c_str(),
            i,
            ff::mpc::dec(
                static_cast<ModConvUp<SmallNum, LargeNum, LargeNum> &>(
                    *batch.children[i])
                    .outputShare)
                .c_str());
      }

      for (size_t i = 0; i < info->num_IVs * info->num_IVs; i++) {
        matrixShare[i] =
            static_cast<ModConvUp<SmallNum, LargeNum, LargeNum> &>(
                *batch.children[i])
                .outputShare;
      }
      for (size_t i = 0; i < info->num_IVs; i++) {
        vectorShare[i] =
            static_cast<ModConvUp<SmallNum, LargeNum, LargeNum> &>(
                *batch.children[i + info->num_IVs * info->num_IVs])
                .outputShare;
      }

      ySquaredShare =
          static_cast<ModConvUp<SmallNum, LargeNum, LargeNum> &>(
              *batch.children[info->num_IVs * (info->num_IVs + 1)])
              .outputShare;
      yShare =
          static_cast<ModConvUp<SmallNum, LargeNum, LargeNum> &>(
              *batch.children[info->num_IVs * (info->num_IVs + 1) + 1])
              .outputShare;
      oneShare =
          static_cast<ModConvUp<SmallNum, LargeNum, LargeNum> &>(
              *batch.children[info->num_IVs * (info->num_IVs + 1) + 2])
              .outputShare;

      dealer::RandomSquareMatrix<LargeNum> random_matrix =
          this->randomness.randomMatrixAndDetInverseDispenser->get();
      this->detShare = random_matrix.det_of_inverse_;

      this->m.reserve(1);
      this->r.reserve(1);

      std::vector<LargeNum> matrix_and_vector_merged(
          this->info->num_IVs * (this->info->num_IVs + 1));

      for (size_t i = 0; i < this->info->num_IVs; i++) {
        for (size_t j = 0; j < this->info->num_IVs; j++) {
          matrix_and_vector_merged[i * (this->info->num_IVs + 1) + j] =
              this->matrixShare[i * this->info->num_IVs + j];
        }
      }

      for (size_t i = 0; i < this->info->num_IVs; i++) {
        matrix_and_vector_merged
            [i * (this->info->num_IVs + 1) + this->info->num_IVs] =
                this->vectorShare[i];
      }

      this->m.emplace_back(ff::mpc::Matrix<LargeNum>(
          matrix_and_vector_merged,
          this->info->num_IVs,
          this->info->num_IVs + 1));
      this->r.emplace_back(random_matrix.values_);

      // (d x d) * (d x (d+1)) output is (d x (d+1)) final column = R(A^Ty)

      std::unique_ptr<ff::mpc::MatrixMult<SAFRN_TYPES, LargeNum>>
          matMult(new ff::mpc::MatrixMult<SAFRN_TYPES, LargeNum>(
              &r.front(),
              &m.front(),
              &this->matMultiplyOutput,
              this->info->endModulusMultiplyInfo,
              std::move(this->randomness
                            .beaverTripleForMatrixMultiplyDispenser)));

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      this->invoke(std::move(matMult), ps);
      // kimee's invoke of MatrixMultiply

      /** invoke matrix multiplication, Issue #225 */
      this->state = awaitingMatrixMultiply;

    } break;
    case (awaitingMatrixMultiply): {
      log_debug("awaitingMatrixMultiply");
      // pointer to result now holds result

      std::unique_ptr<Batch> batchedReveal(new Batch());

      for (size_t i = 0; i < this->info->num_IVs; i++) {
        for (size_t j = 0; j < this->info->num_IVs; j++) {
          batchedReveal->children.emplace_back(
              new ff::mpc::Reveal<SAFRN_TYPES, LargeNum>(
                  this->matMultiplyOutput.at(i, j),
                  this->info->endModulus,
                  this->info->endModulusMultiplyInfo.revealer));
        }
      }

      for (size_t i = 0; i < this->info->num_IVs; i++) {
        this->vectorShare[i] =
            this->matMultiplyOutput.at(i, this->info->num_IVs);
      }

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      /** create Batch<Reveal> Fronctocol, Issue #228 */
      this->invoke(std::move(batchedReveal), ps);
      this->state = awaitingMatrixReveal;
    } break;
    case (awaitingMatrixReveal): {
      log_debug("awaitingMatrixReveal");
      auto & batch = static_cast<Batch &>(f);

      // call to row-reduce

      size_t vector_share_size_copy = this->vectorShare.size();

      ff::mpc::Matrix<LargeNum> output_of_reveal(
          vector_share_size_copy, vector_share_size_copy);

      for (size_t i = 0; i < this->info->num_IVs; i++) {
        for (size_t j = 0; j < this->info->num_IVs; j++) {
          output_of_reveal.at(i, j) =
              static_cast<ff::mpc::Reveal<SAFRN_TYPES, LargeNum> &>(
                  *batch.children[i * this->info->num_IVs + j])
                  .openedValue;

          log_debug(
              "i: %zu, j: %zu, revealed value: %s",
              i,
              j,
              ff::mpc::dec(output_of_reveal.at(i, j)).c_str());
        }
      }

      ff::mpc::Matrix<LargeNum> vectorShareAsMatrixObject(
          std::move(this->vectorShare), vector_share_size_copy, 1);

      log_debug("Calling row-reduce");
      output_of_reveal.MakeIdentity(
          this->info->endModulus, vectorShareAsMatrixObject);

      /** it's inefficient to split this into two calls to MakeIdentity
          TODO: Make this a single call
      */
      log_debug("Calling row-reduce a second time");
      output_of_reveal.MakeIdentity(
          this->info->endModulus, this->r.front());

      log_debug(
          "output string\n%s",
          vectorShareAsMatrixObject.Print().c_str());

      log_debug("Calling determinant");
      this->det = output_of_reveal.Det(this->info->endModulus);
      log_debug("getting ready to call division");
      // call to determinant
      log_debug(
          "\n%s %s\n det and detshare",
          ff::mpc::dec(this->det).c_str(),
          ff::mpc::dec(this->detShare).c_str());

      this->outputWeightShares.resize(
          vectorShareAsMatrixObject.getNumRows());
      log_debug(
          "this->outputWeightShares.size() %zu",
          this->outputWeightShares.size());

      log_debug(
          "this->info->startModulus %s ",
          ff::mpc::dec(this->info->startModulus).c_str());

      log_debug(
          "this->info->divideInfo.ell %zu", this->info->divideInfo.ell);

      std::unique_ptr<Batch> batchedMultiply(new Batch());

      this->regressionMultiplyOutput.resize(
          vectorShareAsMatrixObject.getNumRows());
      for (size_t i = 0; i < vectorShareAsMatrixObject.getNumRows();
           i++) {
        batchedMultiply->children.emplace_back(
            new ff::mpc::Multiply<
                SAFRN_TYPES,
                LargeNum,
                ff::mpc::BeaverInfo<LargeNum>>(
                this->det * this->detShare,
                vectorShareAsMatrixObject.at(i, 0),
                &this->regressionMultiplyOutput[i],
                std::move(
                    this->randomness
                        .beaverTripleForFinalMultiplyDispenser->get()),
                &this->info->endModulusMultiplyInfo));
      }

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      this->invoke(std::move(batchedMultiply), ps);
      this->state =
          awaitingRegressionMultiply; // i.e. final for regression coefficients

    } break;
    case awaitingRegressionMultiply: {
      log_debug("awaitingRegressionMultiply");

      std::unique_ptr<Batch> batchedFirstCompare(new Batch());

      for (size_t i = 0; i < this->info->num_IVs; i++) {
        batchedFirstCompare->children.emplace_back(
            new ff::mpc::Compare<SAFRN_TYPES, LargeNum, SmallNum>(
                this->regressionMultiplyOutput[i],
                LargeNum(0),
                &this->info->compareInfoEndModulus,
                this->randomness.compareEndModulusDispenser->get()));
      }

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      /** batched division constructor goes here, Issue # 163 */
      this->invoke(std::move(batchedFirstCompare), ps);
      this->state = awaitingFirstCompare;
    } break;

    case (awaitingFirstCompare): {
      log_debug("awaitingFirstCompare");
      auto & batch = static_cast<Batch &>(f);

      std::unique_ptr<Batch> batchedTypeCastFromBit(new Batch());

      std::vector<Boolean_t> negative_flag_shares;
      negative_flag_shares.resize(this->info->num_IVs);
      for (size_t i = 0; i < batch.children.size(); i++) {
        negative_flag_shares[i] =
            static_cast<
                ff::mpc::Compare<SAFRN_TYPES, LargeNum, SmallNum> &>(
                *batch.children[i])
                .outputShare %
            2;
        batchedTypeCastFromBit->children.emplace_back(
            new ff::mpc::TypeCastFromBit<SAFRN_TYPES, LargeNum>(
                negative_flag_shares[i],
                this->info->endModulus,
                this->info->revealer,
                this->randomness.typeCastFromBitDispenser->get()));
      }

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      this->invoke(std::move(batchedTypeCastFromBit), ps);
      this->state = awaitingFirstTypeCastFromBit;
    } break;
    case (awaitingFirstTypeCastFromBit): {
      log_debug("awaitingFirstTypeCastFromBit");
      auto & batch = static_cast<Batch &>(f);

      std::unique_ptr<Batch> batchedMultiply(new Batch());

      this->negativeOrPositiveOneShares.resize(this->info->num_IVs);
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        this->negativeOrPositiveOneShares[i] = ff::mpc::modMul(
            LargeNum(2),
            static_cast<
                ff::mpc::TypeCastFromBit<SAFRN_TYPES, LargeNum> &>(
                *batch.children[i])
                .outputBitShare,
            this->info->endModulus);
        if (this->getSelf() == *this->info->revealer) {
          this->negativeOrPositiveOneShares[i] = ff::mpc::modAdd(
              this->negativeOrPositiveOneShares[i],
              this->info->endModulus - LargeNum(1),
              this->info->endModulus);
        }
      }

      this->negativeCorrectionMultiplyOutput.resize(
          this->info->num_IVs);
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        batchedMultiply->children.emplace_back(
            new ff::mpc::Multiply<
                SAFRN_TYPES,
                LargeNum,
                ff::mpc::BeaverInfo<LargeNum>>(
                this->negativeOrPositiveOneShares[i],
                this->regressionMultiplyOutput[i],
                &this->negativeCorrectionMultiplyOutput[i],
                std::move(
                    this->randomness
                        .beaverTripleForFinalMultiplyDispenser->get()),
                &this->info->endModulusMultiplyInfo));
      }

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      this->invoke(std::move(batchedMultiply), ps);
      this->state =
          awaitingNegativeCorrectionMultiply; // i.e. final for regression coefficients

    } break;
    case awaitingNegativeCorrectionMultiply: {

      std::unique_ptr<Batch> batchedDivision(new Batch());

      for (size_t i = 0; i < this->info->num_IVs; i++) {
        batchedDivision->children.emplace_back(
            new ff::mpc::Divide<SAFRN_TYPES, LargeNum, SmallNum>(
                this->negativeCorrectionMultiplyOutput[i],
                this->det * this->detShare,
                &this->outputWeightShares[i],
                &this->info->divideInfo,
                std::move(this->randomness.divideDispenser->get())));
      }
      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      /** batched division constructor goes here, Issue # 163 */
      this->invoke(std::move(batchedDivision), ps);
      this->state = awaitingDivision;
    } break;
    case (awaitingDivision): {
      log_debug("awaitingDivision");

      for (size_t i = 0; i < this->info->num_IVs; i++) {
        log_debug(
            "this->outputWeightShares.at(%zu) is %s",
            i,
            ff::mpc::dec(this->outputWeightShares[i]).c_str());
      }

      std::unique_ptr<Batch> batchedMultiply(new Batch());

      this->beta_i.resize(this->info->num_IVs);
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        batchedMultiply->children.emplace_back(
            new ff::mpc::Multiply<
                SAFRN_TYPES,
                LargeNum,
                ff::mpc::BeaverInfo<LargeNum>>(
                this->outputWeightShares[i],
                this->negativeOrPositiveOneShares[i],
                &this->beta_i[i],
                std::move(
                    this->randomness
                        .beaverTripleForFinalMultiplyDispenser->get()),
                &this->info->endModulusMultiplyInfo));
      }

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      /** batched division constructor goes here, Issue # 163 */
      this->invoke(std::move(batchedMultiply), ps);
      this->state = awaitingNegativeRecorrectionMultiply;
    } break;
    case (awaitingNegativeRecorrectionMultiply): {

      std::unique_ptr<Batch> batchedMultiply(new Batch());

      this->beta_ibeta_j.resize(
          this->info->num_IVs * this->info->num_IVs);

      size_t outputCounter = 0;

      // all pairwise products of coefficients
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        for (size_t j = i; j < this->info->num_IVs; j++) {
          batchedMultiply->children.emplace_back(
              new ff::mpc::Multiply<
                  SAFRN_TYPES,
                  LargeNum,
                  ff::mpc::BeaverInfo<LargeNum>>(
                  this->beta_i[i],
                  this->beta_i[j],
                  &this->beta_ibeta_j[i * this->info->num_IVs + j],
                  std::move(this->randomness
                                .beaverTripleForFinalMultiplyDispenser
                                ->get()),
                  &this->info->endModulusMultiplyInfo));
        }
      }

      this->x_ix_jn.resize(this->info->num_IVs * this->info->num_IVs);
      // all products of sum x_i x_j with n
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        for (size_t j = i; j < this->info->num_IVs; j++) {
          batchedMultiply->children.emplace_back(
              new ff::mpc::Multiply<
                  SAFRN_TYPES,
                  LargeNum,
                  ff::mpc::BeaverInfo<LargeNum>>(
                  this->m.front().at(i, j),
                  this->oneShare,
                  &this->x_ix_jn[i * this->info->num_IVs + j],
                  std::move(this->randomness
                                .beaverTripleForFinalMultiplyDispenser
                                ->get()),
                  &this->info->endModulusMultiplyInfo));
        }
      }

      this->beta_ix_iy.resize(this->info->num_IVs);
      // product of sum x_i y with beta_i
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        batchedMultiply->children.emplace_back(
            new ff::mpc::Multiply<
                SAFRN_TYPES,
                LargeNum,
                ff::mpc::BeaverInfo<LargeNum>>(
                this->m.front().at(i, this->info->num_IVs),
                this->beta_i[i],
                &this->beta_ix_iy[i],
                std::move(
                    this->randomness
                        .beaverTripleForFinalMultiplyDispenser->get()),
                &this->info->endModulusMultiplyInfo));
      }

      batchedMultiply->children.emplace_back(
          new ff::mpc::Multiply<
              SAFRN_TYPES,
              LargeNum,
              ff::mpc::BeaverInfo<LargeNum>>(
              this->ySquaredShare,
              this->oneShare,
              &this->ySquaredn,
              std::move(
                  this->randomness
                      .beaverTripleForFinalMultiplyDispenser->get()),
              &this->info->endModulusMultiplyInfo));

      batchedMultiply->children.emplace_back(
          new ff::mpc::Multiply<
              SAFRN_TYPES,
              LargeNum,
              ff::mpc::BeaverInfo<LargeNum>>(
              this->yShare,
              this->yShare,
              &this->ySumThenSquared,
              std::move(
                  this->randomness
                      .beaverTripleForFinalMultiplyDispenser->get()),
              &this->info->endModulusMultiplyInfo));

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      /** batched division constructor goes here, Issue # 163 */
      this->invoke(std::move(batchedMultiply), ps);
      this->state = awaitingFirstMultiplyForErrorTerms;
    } break;
    case (awaitingFirstMultiplyForErrorTerms): {
      log_debug("awaitingFirstMultiplyForErrorTerms");

      size_t outputCounter = 0;

      std::unique_ptr<Batch> batchedMultiply(new Batch());

      // all pairwise products of coefficients and x_ix_j * n
      this->beta_ibeta_jx_ix_jn.resize(
          this->info->num_IVs * this->info->num_IVs);
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        for (size_t j = i; j < this->info->num_IVs; j++) {
          batchedMultiply->children.emplace_back(new ff::mpc::Multiply<
                                                 SAFRN_TYPES,
                                                 LargeNum,
                                                 ff::mpc::BeaverInfo<
                                                     LargeNum>>(
              this->beta_ibeta_j[i * this->info->num_IVs + j],
              this->x_ix_jn[i * this->info->num_IVs + j],
              &this->beta_ibeta_jx_ix_jn[i * this->info->num_IVs + j],
              std::move(
                  this->randomness
                      .beaverTripleForFinalMultiplyDispenser->get()),
              &this->info->endModulusMultiplyInfo));
        }
      }

      // all pairwise products of coefficients and x_ix_j
      this->beta_ibeta_jx_ix_j.resize(
          this->info->num_IVs * this->info->num_IVs);
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        for (size_t j = i; j < this->info->num_IVs; j++) {
          batchedMultiply->children.emplace_back(new ff::mpc::Multiply<
                                                 SAFRN_TYPES,
                                                 LargeNum,
                                                 ff::mpc::BeaverInfo<
                                                     LargeNum>>(
              this->beta_ibeta_j[i * this->info->num_IVs + j],
              this->m.front().at(i, j),
              &this->beta_ibeta_jx_ix_j[i * this->info->num_IVs + j],
              std::move(
                  this->randomness
                      .beaverTripleForFinalMultiplyDispenser->get()),
              &this->info->endModulusMultiplyInfo));
        }
      }

      // product of sum x_i y with n and with beta_i
      this->beta_ix_iyn.resize(this->info->num_IVs);
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        batchedMultiply->children.emplace_back(
            new ff::mpc::Multiply<
                SAFRN_TYPES,
                LargeNum,
                ff::mpc::BeaverInfo<LargeNum>>(
                this->oneShare,
                this->beta_ix_iy[i],
                &this->beta_ix_iyn[i],
                std::move(
                    this->randomness
                        .beaverTripleForFinalMultiplyDispenser->get()),
                &this->info->endModulusMultiplyInfo));
      }

      // product of diagonal terms of (X^TX)^inv with determinant to clear denominators
      this->X_T_X_inv_diag_reweighted.resize(this->info->num_IVs);
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        batchedMultiply->children.emplace_back(
            new ff::mpc::Multiply<
                SAFRN_TYPES,
                LargeNum,
                ff::mpc::BeaverInfo<LargeNum>>(
                this->det * this->detShare,
                this->r.front().at(i, i),
                &this->X_T_X_inv_diag_reweighted[i],
                std::move(
                    this->randomness
                        .beaverTripleForFinalMultiplyDispenser->get()),
                &this->info->endModulusMultiplyInfo));
      }

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      /** batched division constructor goes here, Issue # 163 */
      this->invoke(std::move(batchedMultiply), ps);
      this->state = awaitingSecondMultiplyForErrorTerms;
    } break;
    case (awaitingSecondMultiplyForErrorTerms): {
      log_debug("awaitingSecondMultiplyForErrorTerms");

      /** MSE = numer_MSE/denom_MSE
        * numer_MSE = (y_i - y_pred)^2 = sum y_i^2 + sum beta_i^2 x_i^2 + 2sum beta_i beta_j x_i x_j - 2 sum beta_i x_i y
        */
      this->numer_MSE = this->ySquaredShare;
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        for (size_t j = i; j < this->info->num_IVs; j++) {
          if (i == j) {
            this->numer_MSE +=
                this->beta_ibeta_jx_ix_j[i * this->info->num_IVs + i];
          } else {
            this->numer_MSE += LargeNum(2) *
                this->beta_ibeta_jx_ix_j[i * this->info->num_IVs + j];
          }
          this->numer_MSE %= this->info->endModulus;
        }
        this->numer_MSE += LargeNum(2) *
            (this->info->endModulus - this->beta_ix_iy[i]);
        this->numer_MSE %= this->info->endModulus;
      }

      /* denom_MSE = n-num_IVs-1 */

      this->denom_MSE = this->oneShare;
      if (this->getSelf() == *this->info->revealer) {
        this->denom_MSE = ff::mpc::modAdd<LargeNum>(
            this->denom_MSE,
            this->info->endModulus - this->info->num_IVs,
            this->info
                ->endModulus); // NOTE: subtract 1 if we're adding a constant term
      }

      /** R^2 = numer_RSquared/denom_RSquared
        * numer_RSquared = denom_RSquared - n*(y_i-y_pred)^2 = sum y_i^2 n + sum beta_i^2 x_i^2 n +
        * 2sum beta_i beta_j x_i x_j n - 2 sum beta_i x_i y n
        */

      this->numer_RSquared = this->ySquaredn;
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        for (size_t j = i; j < this->info->num_IVs; j++) {
          if (i == j) {
            this->numer_RSquared = ff::mpc::modAdd<LargeNum>(
                this->numer_RSquared,
                this->beta_ibeta_jx_ix_jn[i * this->info->num_IVs + i],
                this->info->endModulus);
          } else {
            this->numer_RSquared = ff::mpc::modAdd<LargeNum>(
                this->numer_RSquared,
                LargeNum(2) *
                    this->beta_ibeta_jx_ix_jn
                        [i * this->info->num_IVs + j],
                this->info->endModulus);
          }
        }
        this->numer_RSquared = ff::mpc::modAdd<LargeNum>(
            this->numer_RSquared,
            LargeNum(2) *
                (this->info->endModulus - this->beta_ix_iyn[i]),
            this->info->endModulus);
      }

      /** denom_RSquared = n*sum y_i^2 - (sum y_i)^2
        * numer_F_statistic/denom_F_statistic = F_statistic */

      this->denom_RSquared = this->ySquaredn;
      /** TODO: Fix next two lines with switch bit for affine case */
      if (this->info->fitIntercept) {
        this->denom_RSquared +=
            (this->info->endModulus - this->ySumThenSquared);
        this->denom_RSquared %= this->info->endModulus;
      }
      this->denom_F_statistic = this->numer_RSquared *
          this->info
              ->num_IVs; // this assumes we have a constant term and it's counted in num_IVs
      this->denom_F_statistic %= this->info->endModulus;
      this->numer_RSquared =
          this->info->endModulus - this->numer_RSquared;
      this->numer_RSquared += this->denom_RSquared;
      this->numer_RSquared %= this->info->endModulus;

      std::unique_ptr<Batch> batchedMultiply(new Batch());

      // product of diagonal terms of (X^TX)^inv with numer_MSE
      this->X_T_X_inv_diag_s_e.resize(this->info->num_IVs);
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        batchedMultiply->children.emplace_back(
            new ff::mpc::Multiply<
                SAFRN_TYPES,
                LargeNum,
                ff::mpc::BeaverInfo<LargeNum>>(
                X_T_X_inv_diag_reweighted[i],
                this->numer_MSE,
                &this->X_T_X_inv_diag_s_e[i],
                std::move(
                    this->randomness
                        .beaverTripleForFinalMultiplyDispenser->get()),
                &this->info->endModulusMultiplyInfo));
      }

      batchedMultiply->children.emplace_back(
          new ff::mpc::Multiply<
              SAFRN_TYPES,
              LargeNum,
              ff::mpc::BeaverInfo<LargeNum>>(
              this->det * this->detShare,
              this->denom_MSE,
              &denom_MSE_reweighted,
              std::move(
                  this->randomness
                      .beaverTripleForFinalMultiplyDispenser->get()),
              &this->info->endModulusMultiplyInfo));

      batchedMultiply->children.emplace_back(
          new ff::mpc::Multiply<
              SAFRN_TYPES,
              LargeNum,
              ff::mpc::BeaverInfo<LargeNum>>(
              this->numer_RSquared,
              this->denom_MSE,
              &this->numer_F_statistic,
              std::move(
                  this->randomness
                      .beaverTripleForFinalMultiplyDispenser->get()),
              &this->info->endModulusMultiplyInfo));

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      /** batched division constructor goes here, Issue # 163 */
      this->invoke(std::move(batchedMultiply), ps);
      this->state = awaitingThirdMultiplyForErrorTerms;
    } break;
    case (awaitingThirdMultiplyForErrorTerms): {
      log_debug("awaitingThirdMultiplyForErrorTerms");

      std::unique_ptr<Batch> batchedMultiply(new Batch());

      // product of diagonal terms of (X^TX)^inv with numer_MSE
      this->t_statisticNumerators.resize(this->info->num_IVs);
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        batchedMultiply->children.emplace_back(
            new ff::mpc::Multiply<
                SAFRN_TYPES,
                LargeNum,
                ff::mpc::BeaverInfo<LargeNum>>(
                denom_MSE_reweighted,
                beta_ibeta_j[i * this->info->num_IVs + i],
                &this->t_statisticNumerators[i],
                std::move(
                    this->randomness
                        .beaverTripleForFinalMultiplyDispenser->get()),
                &this->info->endModulusMultiplyInfo));
      }

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      /** batched division constructor goes here, Issue # 163 */
      this->invoke(std::move(batchedMultiply), ps);
      this->state = awaitingFourthMultiplyForErrorTerms;
    } break;
    case (awaitingFourthMultiplyForErrorTerms): {
      log_debug("awaitingFourthMultiplyForErrorTerms");

      std::unique_ptr<Batch> batchedDivision(new Batch());
      log_debug(
          "%s %s %s %s",
          ff::mpc::dec(this->oneShare).c_str(),
          ff::mpc::dec(this->ySquaredn).c_str(),
          ff::mpc::dec(this->numer_RSquared).c_str(),
          ff::mpc::dec(this->denom_RSquared).c_str());

      batchedDivision->children.emplace_back(
          new ff::mpc::Divide<SAFRN_TYPES, LargeNum, SmallNum>(
              this->numer_MSE *
                  (LargeNum(1) << this->globals->bitsOfPrecision),
              this->denom_MSE,
              &this->meanSquareErrorShare,
              &this->info->divideInfo,
              std::move(this->randomness.divideDispenser->get())));
      batchedDivision->children.emplace_back(
          new ff::mpc::Divide<SAFRN_TYPES, LargeNum, SmallNum>(
              this->numer_RSquared *
                  (LargeNum(1) << this->globals->bitsOfPrecision),
              this->denom_RSquared,
              &this->RSquaredShare,
              &this->info->divideInfo,
              std::move(this->randomness.divideDispenser->get())));

      this->meanSquareErrorCoeffs.resize(this->info->num_IVs);
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        batchedDivision->children.emplace_back(
            new ff::mpc::Divide<SAFRN_TYPES, LargeNum, SmallNum>(
                this->X_T_X_inv_diag_s_e[i] *
                    (LargeNum(1)
                     << (2 * this->globals->bitsOfPrecision)),
                this->denom_MSE_reweighted,
                &this->meanSquareErrorCoeffs[i],
                &this->info->divideInfo,
                std::move(this->randomness.divideDispenser->get())));
      }

      batchedDivision->children.emplace_back(
          new ff::mpc::Divide<SAFRN_TYPES, LargeNum, SmallNum>(
              this->numer_F_statistic *
                  (LargeNum(1) << this->F_cols_bits_of_precision),
              this->denom_F_statistic * this->F_cols_step_size,
              &this->F_statistic,
              &this->info->divideInfo,
              std::move(this->randomness.divideDispenser->get())));

      this->t_statisticsSquared.resize(this->info->num_IVs);
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        batchedDivision->children.emplace_back(
            new ff::mpc::Divide<SAFRN_TYPES, LargeNum, SmallNum>(
                this->t_statisticNumerators[i] *
                    (LargeNum(1) << this->t_cols_bits_of_precision),
                this->X_T_X_inv_diag_s_e[i] * this->t_cols_step_size,
                &this->t_statisticsSquared[i],
                &this->info->divideInfo,
                std::move(this->randomness.divideDispenser->get())));
      }

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      /** batched division constructor goes here, Issue # 163 */
      this->invoke(std::move(batchedDivision), ps);
      this->state = awaitingDivisionForErrorTerms;
    } break;
    case (awaitingDivisionForErrorTerms): {
      log_debug("awaitingDivisionForErrorTerms");

      std::unique_ptr<Batch> batchedFinalCompare(new Batch());

      for (size_t i = 0; i < this->F_row_ids.size() - 1; i++) {
        if (this->getSelf() == *this->info->revealer) {
          batchedFinalCompare->children.emplace_back(
              new ff::mpc::Compare<SAFRN_TYPES, LargeNum, SmallNum>(
                  ff::mpc::modSub(
                      this->startModulusPayloadVector
                          [this->info->num_IVs *
                               (this->info->num_IVs + 1) +
                           2],
                      static_cast<LargeNum>(info->num_IVs),
                      this->info->startModulus), // count
                  this->F_row_ids[i],
                  &this->info->compareInfo,
                  this->randomness.compareDispenser->get()));
        } else {
          batchedFinalCompare->children.emplace_back(
              new ff::mpc::Compare<SAFRN_TYPES, LargeNum, SmallNum>(
                  this->startModulusPayloadVector
                      [info->num_IVs * (info->num_IVs + 1) +
                       2], // count
                  LargeNum(0),
                  &this->info->compareInfo,
                  this->randomness.compareDispenser->get()));
        }
      }

      for (size_t i = 0; i < this->t_row_ids.size() - 1; i++) {
        if (this->getSelf() == *this->info->revealer) {
          batchedFinalCompare->children.emplace_back(
              new ff::mpc::Compare<SAFRN_TYPES, LargeNum, SmallNum>(
                  ff::mpc::modSub(
                      this->startModulusPayloadVector
                          [this->info->num_IVs *
                               (this->info->num_IVs + 1) +
                           2],
                      static_cast<LargeNum>(info->num_IVs),
                      this->info->startModulus), // count
                  this->t_row_ids[i],
                  &this->info->compareInfo,
                  this->randomness.compareDispenser->get()));
        } else {
          batchedFinalCompare->children.emplace_back(
              new ff::mpc::Compare<SAFRN_TYPES, LargeNum, SmallNum>(
                  this->startModulusPayloadVector
                      [info->num_IVs * (info->num_IVs + 1) +
                       2], // count
                  LargeNum(0),
                  &this->info->compareInfo,
                  this->randomness.compareDispenser->get()));
        }
      }

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      /** batched division constructor goes here, Issue # 163 */
      this->invoke(std::move(batchedFinalCompare), ps);
      this->state = awaitingCompareForFtStatistics;
    } break;

    case (awaitingCompareForFtStatistics): {
      log_debug("awaitingCompareForFtStatistics");
      auto & batch = static_cast<Batch &>(f);

      std::unique_ptr<Batch> batchedTypeCastFromBit(new Batch());

      for (size_t i = 0; i < batch.children.size(); i++) {

        batchedTypeCastFromBit->children.emplace_back(
            new ff::mpc::TypeCastFromBit<SAFRN_TYPES, LargeNum>(
                static_cast<
                    ff::mpc::
                        Compare<SAFRN_TYPES, LargeNum, SmallNum> &>(
                    *batch.children[i])
                        .outputShare %
                    2,
                this->info->endModulus,
                this->info->revealer,
                this->randomness.typeCastFromBitDispenser->get()));
      }

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      this->invoke(std::move(batchedTypeCastFromBit), ps);
      this->state = awaitingBatchedTypeCastFromBit;
    } break;
    case (awaitingBatchedTypeCastFromBit): {
      log_debug("awaitingBatchedTypeCastFromBit");
      auto & batch = static_cast<Batch &>(f);

      size_t counter = 0;
      for (size_t i = 0; i < this->F_row_ids.size() - 1; i++) {
        this->F_row_id_share +=
            static_cast<
                ff::mpc::TypeCastFromBit<SAFRN_TYPES, LargeNum> &>(
                *batch.children[counter++])
                .outputBitShare;
        this->F_row_id_share %= this->info->endModulus;
      }

      t_row_id_share = 0;
      for (size_t j = 0; j < this->t_row_ids.size() - 1; j++) {
        this->t_row_id_share = ff::mpc::modAdd(
            this->t_row_id_share,
            static_cast<
                ff::mpc::TypeCastFromBit<SAFRN_TYPES, LargeNum> &>(
                *batch.children[counter++])
                .outputBitShare,
            this->info->endModulus);
      }

      std::unique_ptr<Batch> batchedFinalCompareEndModulus(new Batch());

      if (this->getSelf() == *this->info->revealer) {
        batchedFinalCompareEndModulus->children.emplace_back(
            new ff::mpc::Compare<SAFRN_TYPES, LargeNum, SmallNum>(
                this->F_statistic, // count
                LargeNum(this->num_F_cols),
                &this->info->compareInfoEndModulus,
                this->randomness.compareEndModulusDispenser->get()));

        for (size_t i = 0; i < this->info->num_IVs; i++) {
          batchedFinalCompareEndModulus->children.emplace_back(
              new ff::mpc::Compare<SAFRN_TYPES, LargeNum, SmallNum>(
                  this->t_statisticsSquared[i], // count
                  LargeNum(this->num_t_cols),
                  &this->info->compareInfoEndModulus,
                  this->randomness.compareEndModulusDispenser->get()));
        }
      } else {
        batchedFinalCompareEndModulus->children.emplace_back(
            new ff::mpc::Compare<SAFRN_TYPES, LargeNum, SmallNum>(
                this->F_statistic, // count
                LargeNum(0),
                &this->info->compareInfoEndModulus,
                this->randomness.compareEndModulusDispenser->get()));

        for (size_t i = 0; i < this->info->num_IVs; i++) {
          batchedFinalCompareEndModulus->children.emplace_back(
              new ff::mpc::Compare<SAFRN_TYPES, LargeNum, SmallNum>(
                  this->t_statisticsSquared[i], // count
                  LargeNum(0),
                  &this->info->compareInfoEndModulus,
                  this->randomness.compareEndModulusDispenser->get()));
        }
      }

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      this->invoke(std::move(batchedFinalCompareEndModulus), ps);
      this->state = awaitingCompareForFtStatisticsEndModulus;
    } break;

    case (awaitingCompareForFtStatisticsEndModulus): {
      log_debug("awaitingCompareForFtStatisticsEndModulus");
      auto & batch = static_cast<Batch &>(f);

      std::unique_ptr<Batch> batchedTypeCastFromBit(new Batch());

      for (size_t i = 0; i < batch.children.size(); i++) {

        batchedTypeCastFromBit->children.emplace_back(
            new ff::mpc::TypeCastFromBit<SAFRN_TYPES, LargeNum>(
                static_cast<
                    ff::mpc::
                        Compare<SAFRN_TYPES, LargeNum, SmallNum> &>(
                    *batch.children[i])
                        .outputShare %
                    2,
                this->info->endModulus,
                this->info->revealer,
                this->randomness.typeCastFromBitDispenser->get()));
      }

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      this->invoke(std::move(batchedTypeCastFromBit), ps);
      this->state = awaitingBatchedTypeCastFromBitLast;
    } break;
    case (awaitingBatchedTypeCastFromBitLast): {
      log_debug("awaitingBatchedTypeCastFromBitLast");
      auto & batch = static_cast<Batch &>(f);

      size_t counter = 0;

      LargeNum F_statistic_overflow_bit =
          static_cast<
              ff::mpc::TypeCastFromBit<SAFRN_TYPES, LargeNum> &>(
              *batch.children[counter++])
              .outputBitShare;

      std::vector<LargeNum> t_statistic_overflow_bits;
      t_statistic_overflow_bits.resize(this->info->num_IVs);

      for (size_t i = 0; i < this->info->num_IVs; i++) {
        t_statistic_overflow_bits[i] =
            static_cast<
                ff::mpc::TypeCastFromBit<SAFRN_TYPES, LargeNum> &>(
                *batch.children[counter++])
                .outputBitShare;
      }

      LargeNum revealer_val_shift = 0;
      if (this->getSelf() == *this->info->revealer) {
        revealer_val_shift = (this->num_F_cols - 1);
      }

      LargeNum revealer_val_shift_t = 0;
      if (this->getSelf() == *this->info->revealer) {
        revealer_val_shift_t = (this->num_t_cols - 1);
      }

      std::unique_ptr<Batch> batchedMultiply(new Batch());

      batchedMultiply->children.emplace_back(
          new ff::mpc::Multiply<
              SAFRN_TYPES,
              LargeNum,
              ff::mpc::BeaverInfo<LargeNum>>(
              F_statistic_overflow_bit,
              revealer_val_shift + this->info->endModulus -
                  this->F_statistic,
              &this->F_statistic_col_index,
              std::move(
                  this->randomness
                      .beaverTripleForFinalMultiplyDispenser->get()),
              &this->info->endModulusMultiplyInfo));

      // product of diagonal terms of (X^TX)^inv with numer_MSE
      this->t_statistic_col_indices.resize(this->info->num_IVs);
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        batchedMultiply->children.emplace_back(
            new ff::mpc::Multiply<
                SAFRN_TYPES,
                LargeNum,
                ff::mpc::BeaverInfo<LargeNum>>(
                t_statistic_overflow_bits[i],
                revealer_val_shift_t + this->info->endModulus -
                    this->t_statisticsSquared[i],
                &this->t_statistic_col_indices[i],
                std::move(
                    this->randomness
                        .beaverTripleForFinalMultiplyDispenser->get()),
                &this->info->endModulusMultiplyInfo));
      }

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      /** batched division constructor goes here, Issue # 163 */
      this->invoke(std::move(batchedMultiply), ps);
      this->state = awaitingFifthMultiplyForErrorTerms;
    } break;
    case (awaitingFifthMultiplyForErrorTerms): {
      log_debug("awaitingFifthMultiplyForErrorTerms");

      this->F_statistic_col_index = ff::mpc::modAdd(
          this->F_statistic_col_index,
          this->F_statistic,
          this->info->endModulus);

      for (size_t i = 0; i < this->info->num_IVs; i++) {
        this->t_statistic_col_indices[i] = ff::mpc::modAdd(
            this->t_statistic_col_indices[i],
            this->t_statisticsSquared[i],
            this->info->endModulus);
      }

      log_debug(
          "F: %s %s %s",
          ff::mpc::dec(this->F_statistic_col_index).c_str(),
          ff::mpc::dec(this->F_row_id_share * this->num_F_cols).c_str(),
          ff::mpc::dec(this->num_F_cols).c_str());

      std::unique_ptr<Batch> batchedLookup(new Batch());

      batchedLookup->children.emplace_back(new Lookup(
          this->F_statistic_col_index +
              this->F_row_id_share * this->num_F_cols,
          this->F_p_value,
          this->F_table_data,
          this->randomness.F_lookupDispenser->get(),
          &this->F_info,
          this->info->bytesInLookupTableCells,
          this->info->revealer));

      this->t_p_values.resize(this->info->num_IVs);
      for (size_t i = 0; i < this->info->num_IVs; i++) {
        batchedLookup->children.emplace_back(new Lookup(
            this->t_statistic_col_indices[i] +
                this->t_row_id_share * this->num_t_cols,
            this->t_p_values[i],
            this->t_table_data,
            this->randomness.t_lookupDispenser->get(),
            &this->t_info,
            this->info->bytesInLookupTableCells,
            this->info->revealer));
      }

      PeerSet ps(this->getPeers());
      ps.removeDealer();
      ps.removeRecipients();
      this->invoke(std::move(batchedLookup), ps);
      this->state = awaitingBatchedLookup;
    } break;
    case (awaitingBatchedLookup): {
      log_debug("awaitingBatchedLookup");

      for (size_t i = 0; i < this->info->num_IVs; i++) {
        log_debug(
            "this->outputWeightShares[%zu] is %s",
            i,
            ff::mpc::dec(this->beta_i[i]).c_str());
      }

      log_debug(
          "this->endModulus %s",
          ff::mpc::dec(this->info->endModulus).c_str());

      /** Send to recipients */

      this->getPeers().forEachRecipient([&,
                                         this](const Identity & other) {
        std::unique_ptr<OutgoingMessage> omsg(
            new OutgoingMessage(other));
        for (size_t i = 0; i < this->info->num_IVs; i++) {
          omsg->write<LargeNum>(
              this->beta_i
                  [i]); // NOTE: divide by (2**info->bits_of_precision) for result as double
        }
        omsg->write<LargeNum>(this->meanSquareErrorShare);
        omsg->write<LargeNum>(this->RSquaredShare);
        for (size_t i = 0; i < this->info->num_IVs; i++) {
          log_debug("Writing s_e_coeff[%zu] ", i);
          omsg->write<LargeNum>(
              this->meanSquareErrorCoeffs
                  [i]); // NOTE: divide by (2**(2*info->bits_of_precision)) for result as double
        }

        log_debug("writing F_table p-value");
        for (size_t i = 0; i < this->info->bytesInLookupTableCells;
             i++) {
          omsg->write<Boolean_t>(this->F_p_value[i]);
        }
        for (size_t i = 0; i < this->info->num_IVs; i++) {
          log_debug("writing t_table[%zu] p-value", i);
          for (size_t j = 0; j < this->info->bytesInLookupTableCells;
               j++) {
            omsg->write<Boolean_t>(this->t_p_values[i][j]);
          }
        }

        log_debug(
            "Num f cols: %s", ff::mpc::dec(this->num_F_cols).c_str());

        this->send(std::move(omsg));
      });

      this->complete();
    } break;
    default:
      log_error("Regression state machine in unexpected state");
  }
}

std::string Regression::name() {
  return std::string("Regression");
}

} // namespace dataowner
} // namespace safrn
