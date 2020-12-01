/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <Util/read_file_utils.h> // Paul's stuff, if we ff::mpc::decide it's worth it
#include <Util/string_utils.h> // Paul's string stuff
#include <dataowner/Moments.h>

#include <mpc/ObservationList.h>

/* logging configuration */
#include <ff/logging.h>

namespace safrn {
namespace dataowner {

Moments::Moments(
    ff::mpc::ObservationList<LargeNum> && olist,
    GlobalInfo const * const g_info,
    std::unique_ptr<const MomentsInfo> m_info) :
    ownList(olist), globals(g_info), info(std::move(m_info)) {

  if (this->abortFlag) {
    return;
  }
  this->computePayloadVectorAndPadList();

  this->setupCrossVerticalShares();
}

/* PHB
void Moments::readInputFile() {
  log_debug("Calling readInputFile");
  // Issue #220
  this->ownList = ff::mpc::ObservationList();
  // First column holds the key, second column holds the vertical
  this->ownList.numKeyCols = this->info->numKeyCols;
  this->ownList.numArithmeticPayloadCols = this->info->payloadLength;
  this->ownList.numXORPayloadCols = 0;

  std::ifstream input_stream(this->csvFile.c_str());
  if (!input_stream.is_open()) {
    log_error("Error opening file %s", this->csvFile.c_str());
    this->abortFlag = true;
    return;
  }

  this->ownList.elements.reserve(this->globals->maxListSize);
  std::string line;

  log_debug("about to read");
  while (getline(input_stream, line)) {
    log_debug("Hi, line is %s", line.c_str());
    file_reader_utils::RemoveWindowsTrailingCharacters(&line);
    line = string_utils::RemoveAllWhitespace(line);
    if (string_utils::HasPrefixString(line, "#")) {
      continue;
    }
    std::vector<std::string> split_line;
    string_utils::Split(line, ",", &split_line);
    std::vector<SmallNum> parsed_line;
    parsed_line.reserve(split_line.size());
    size_t i = 0;
    for (std::string & s : split_line) {
      if (i != (this->info->columnIndexKey)) {
        double rounding_offset = 0.5;
        parsed_line.push_back(static_cast<SmallNum>(floor(
            rounding_offset +
            static_cast<double>((1 << (globals->bitsOfPrecision))) *
                atof(s.c_str()))));
        // adjusting parsed_line.back()
        parsed_line.back() = ff::mpc::modAdd(
            this->info->startModulus,
            parsed_line.back(),
            this->info->startModulus);
      } else {
        parsed_line.push_back(static_cast<SmallNum>(atoi(s.c_str())));
      }
      i++;
    }

    std::vector<SmallNum> keys;
    keys.reserve(this->info->numKeyCols);
    keys.emplace_back(parsed_line.at(this->info->columnIndexKey));
    keys.emplace_back(this->info->selfVertical);
    std::vector<SmallNum> payloads;
    payloads.reserve(this->info->payloadLength);
    if (this->info->selfVertical == this->info->dataVertical) {
      payloads.emplace_back(1);
      payloads.emplace_back(
          parsed_line.at(this->info->dataColumnIndex));
      for (size_t j = 1; j < this->info->highest_moment; j++) {
        payloads.emplace_back(payloads.at(1) * payloads.at(j));
      }
    } else {
      payloads.resize(this->info->payloadLength);
      for (size_t i = 0; i < payloads.size(); i++) {
        payloads.at(i) = 0;
      }
    }
    log_debug("payloads.size() %zu", payloads.size());
    std::vector<Boolean_t> XOR_payloads; // empty
    this->ownList.elements.emplace_back(ff::mpc::Observation());
    ff::mpc::Observation & o = this->ownList.elements.back();

    o.keyCols = std::move(keys);
    o.arithmeticPayloadCols = std::move(payloads);
    o.XORPayloadCols = std::move(XOR_payloads);
  }
}
*/

void Moments::computePayloadVectorAndPadList() {
  log_debug("Calling computePayloadVectorAndPadList");
  // Issue #220
  log_debug(
      "This->ownList.elements.size() %zu, maxListSize %zu",
      this->ownList.elements.size(),
      this->globals->maxListSize);
  log_debug("this->info->payloadLength %zu", this->info->payloadLength);

  log_debug(
      "This->ownList.elements.size() %zu, maxListSize %zu",
      this->ownList.elements.size(),
      this->globals->maxListSize);
  /** These are already set above, but we re-set them to follow
    * the pattern laid out in Regression::computePayloadVectorAndPadlist
    * and to remind the reader what the values *should* be, since
    * numArithmeticPayloadCols, etc, are not binding on an ObservationList
    */
  this->ownList.numArithmeticPayloadCols = this->info->payloadLength;
  this->ownList.numKeyCols = this->info->numKeyCols;
  this->ownList.numXORPayloadCols = 0;

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

void Moments::setupCrossParties() {
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

void Moments::setupCrossVerticalShares() {
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
        this->globals->maxListSize *
        2); // twice as long to hold my shares and theirs
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
            ff::mpc::randomModP<LargeNum>(this->info->startModulus);
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
               ((this->info->selfVertical != this->info->dataVertical) ?
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

void Moments::init() {
  log_debug("Calling init");
  if (this->abortFlag) {
    this->abort();
    return;
  }
  log_debug(
      "start modulus: %s, end modulus: %s",
      ff::mpc::dec(this->info->startModulus).c_str(),
      ff::mpc::dec(this->info->endModulus).c_str());

  log_debug("maxListSize? %zu", this->globals->maxListSize);

  this->state = awaitingRandomnessAndSISOSort;
  this->setupCrossParties();

  this->shareWithCrossVerticalParties();
  this->invokeRandomnessPatron();
}

void Moments::shareWithCrossVerticalParties() {
  log_debug("Calling shareWithCrossVerticalParties");

  log_debug("maxListSize? %zu", this->globals->maxListSize);

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

void Moments::invokeRandomnessPatron() {
  log_debug("Calling invokeRandomnessPatron");
  std::unique_ptr<MomentsRandomnessPatron> patron(
      new MomentsRandomnessPatron(
          this->info.get(), this->info->dealer, 1UL));
  PeerSet ps(this->getPeers());
  ps.removeRecipients();
  this->invoke(std::move(patron), ps);
}

void Moments::handlePromise(Fronctocol &) {
  log_error("Unexpected handle promise received in Moments");
}

void Moments::handleReceive(IncomingMessage & msg) {
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

  log_debug("maxListSize? %zu", this->globals->maxListSize);

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
             ((this->info->selfVertical == this->info->dataVertical) ?
                  1 :
                  0)] = std::move(o);
  }

  log_debug("Did we get here?");

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
        if (this->info->selfVertical == this->info->dataVertical) {
          temp_revealer = &this->getSelf();
        } else {
          temp_revealer = &other;
        }

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
  }
}

void Moments::handleComplete(Fronctocol & f) {
  log_debug("Calling handleComplete");
  if (!this->randomnessDone) {
    log_debug("randomness?");
    auto * patron = dynamic_cast<MomentsRandomnessPatron *>(&f);
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
      randomness = std::move(patron->MomentsDispenser->get());
      log_debug("Move onto awaitingSISOSort");
      this->randomnessDone = true;
      if (this->state == awaitingRandomnessOnly) {
        this->numPartiesAwaiting =
            1; // so that we move on to zipAdjacent below
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

        log_debug("and onto zipAdjacent");
        size_t i = 0;
        this->zipAdjacentInfo.reserve(this->info->numCrossParties);

        this->getPeers().forEachDataowner([&, this](
                                              const Identity & other) {
          if (other.vertical != this->getSelf().vertical) {

            safrn::Identity const * temp_revealer = nullptr;
            if (this->info->selfVertical == this->info->dataVertical) {
              temp_revealer = &this->getSelf();
            } else {
              temp_revealer = &other;
            }
            log_assert(this->info != nullptr);

            /**
              * The * 2 here is because the list doubles in
              * length after sharing with one cross-vertical party
              */
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

        log_debug("and onto modconvup");

        this->powerSumsStartModulus.resize(this->info->payloadLength);
        for (ff::mpc::ObservationList<LargeNum> o_list :
             this->vectorZippedAdjacent) {
          for (ff::mpc::Observation<LargeNum> o : o_list.elements) {
            for (size_t i = 0; i < this->info->payloadLength; i++) {
              this->powerSumsStartModulus[i] = ff::mpc::modAdd(
                  this->powerSumsStartModulus[i],
                  o.arithmeticPayloadCols[i],
                  this->info->startModulus);
            }
          }
        }

        /** Issue #223 */
        std::unique_ptr<ff::mpc::Batch<SAFRN_TYPES>> batchedModConv(
            new ff::mpc::Batch<SAFRN_TYPES>());
        for (size_t i = 0; i < this->info->payloadLength; i++) {
          batchedModConv->children.emplace_back(
              new ModConvUp<SmallNum, LargeNum, LargeNum>(
                  this->powerSumsStartModulus[i],
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

      this->powerSums.resize(this->info->payloadLength);
      for (size_t i = 0; i < batch.children.size(); i++) {
        log_debug(
            "Shares of X^i mod %s : S[%zu]=\n%s",
            ff::mpc::dec(this->info->endModulus).c_str(),
            i,
            ff::mpc::dec(
                static_cast<ModConvUp<SmallNum, LargeNum, LargeNum> &>(
                    *batch.children[i])
                    .outputShare)
                .c_str());

        this->powerSums[i] =
            static_cast<ModConvUp<SmallNum, LargeNum, LargeNum> &>(
                *batch.children[i])
                .outputShare;
      }

      log_debug("and onto batchedDivide");

      std::unique_ptr<Batch> batchedDivision(new Batch());

      this->expectationOfNthPow.resize(this->powerSums.size());
      this->expectationOfNthPow.front() = this->powerSums.front();
      for (size_t i = 1; i < this->powerSums.size(); i++) {
        batchedDivision->children.emplace_back(
            new ff::mpc::Divide<SAFRN_TYPES, LargeNum, SmallNum>(
                this->powerSums[i] *
                    LargeNum(1 << this->globals->bitsOfPrecision),
                this->powerSums.front(),
                &this->expectationOfNthPow[i],
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

      /** Send to recipients */

      this->getPeers().forEachRecipient([&,
                                         this](const Identity & other) {
        std::unique_ptr<OutgoingMessage> omsg(
            new OutgoingMessage(other));
        if (this->info->includeZerothMoment) {
          omsg->write<LargeNum>(this->expectationOfNthPow.front());
        }
        for (size_t i = 1; i < this->expectationOfNthPow.size(); i++) {
          omsg->write<LargeNum>(this->expectationOfNthPow[i]);
        }
        this->send(std::move(omsg));
      });

      this->complete();
    } break;
    default:
      log_error("Moments state machine in unexpected state");
  }
}

std::string Moments::name() {
  return std::string("Moments");
}

} // namespace dataowner
} // namespace safrn
