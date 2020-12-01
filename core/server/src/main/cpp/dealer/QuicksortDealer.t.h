/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <ff/logging.h>

template<FF_TYPENAMES>
QuickSortFronctocol<FF_TYPES>::QuickSortFronctocol(
    CompareInputInfo<Identity_T> compareInfo,
    size_t numElements,
    size_t numKeyCols) :
    compareInfo(compareInfo),
    numElements(numElements),
    numKeyCols(numKeyCols) {
}

template<FF_TYPENAMES>
void QuickSortFronctocol<FF_TYPES>::init() {

  this->numPartiesWaiting = 0;
  this->getPeers().forEach([this](Identity_T const & peer) {
    if (this->getSelf() != peer) {
      this->numPartiesWaiting++;
    }
  });

  std::vector<size_t> UnboundedFaninOrRandomnessNeeds;

  size_t block_size = this->compareInfo.lambda;
  while (block_size < this->compareInfo.ell) {
    UnboundedFaninOrRandomnessNeeds.emplace_back(block_size);
    log_debug("block_size: %lu", block_size);
    block_size += this->compareInfo.lambda;
  }
  log_debug("block_size: %lu", this->compareInfo.ell);
  UnboundedFaninOrRandomnessNeeds.emplace_back(this->compareInfo.ell);

  block_size = 1;
  while ((block_size - 1) < this->compareInfo.lambda) {
    log_debug("block_size: %lu", block_size);
    UnboundedFaninOrRandomnessNeeds.emplace_back(block_size);
    block_size++;
  }
  log_debug("Total size: %lu", UnboundedFaninOrRandomnessNeeds.size());

  for (size_t i = 0; i < UnboundedFaninOrRandomnessNeeds.size(); i++) {
    std::unique_ptr<Fronctocol<FF_TYPES>> rd(new RandomnessHouse<
                                             FF_TYPES,
                                             ExponentSeries_t,
                                             ExponentSeriesInfo>());
    self->invoke(std::move(rd), self->getPeers());
  }
  std::unique_ptr<ff::Fronctocol<FF_TYPES>> rd2(
      new RandomnessHouse<
          FF_TYPES,
          BeaverTriple<SmallNum>,
          ArithmeticBeaverInfo>());
  self->invoke(std::move(rd2), self->getPeers());

  std::unique_ptr<ff::Fronctocol<FF_TYPES>> rd4(
      new RandomnessHouse<FF_TYPES, TypeCastTriple, TypeCastInfo>());
  self->invoke(std::move(rd4), self->getPeers());

  std::unique_ptr<ff::Fronctocol<FF_TYPES>> rd5(
      new RandomnessHouse<
          FF_TYPES,
          DecomposedBitSet_t,
          DecomposedBitSetInfo>());
  self->invoke(std::move(rd5), self->getPeers());
}

template<FF_TYPENAMES>
void QuickSortFronctocol<FF_TYPES>::handleReceive(
    IncomingMessage_T & imsg) {
  log_debug("Quicksort Fronctocol received handle receive");
  this->numPartiesWaiting--;
  if (this->numPartiesWaiting == 0) {
    this->complete();
  }
  (void)imsg;
}

template<FF_TYPENAMES>
void QuickSortFronctocol<FF_TYPES>::handleComplete(
    ff::Fronctocol<FF_TYPES> &) {
}

template<FF_TYPENAMES>
void QuickSortFronctocol<FF_TYPES>::handlePromise(
    ff::Fronctocol<FF_TYPES> &) {
  log_error("Quicksort Fronctocol received unexpected "
            "handle promise");
}
