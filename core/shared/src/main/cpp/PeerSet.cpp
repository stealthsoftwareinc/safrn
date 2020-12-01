/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */
#include <algorithm>
#include <functional>
#include <iterator>
#include <vector>

/* 3rd Party Headers */

/* Fortissimo Headers */
#include <ff/Message.h>
#include <ff/PeerSet.h>
#include <ff/Util.h>

/* SAFRN Headers */
#include <Identity.h>
#include <PeerSet.h>
#include <Util/Utils.h>

/* Logging Configuration */
#include <ff/logging.h>

namespace safrn {

PeerSet::PeerWrapper::PeerWrapper() :
    peer(), id(ff::FRONCTOCOLID_INVALID), completion(false) {
}
PeerSet::PeerWrapper::PeerWrapper(Identity const & p) :
    peer(p), id(ff::FRONCTOCOLID_INVALID), completion(false) {
}
PeerSet::PeerWrapper::PeerWrapper(PeerWrapper const & other) :
    peer(other.peer), id(ff::FRONCTOCOLID_INVALID), completion(false) {
}

bool PeerSet::PeerWrapper::operator==(PeerWrapper const & other) const {
  return this->peer == other.peer;
}

bool PeerSet::PeerWrapper::operator!=(PeerWrapper const & other) const {
  return !(*this == other);
}

bool PeerSet::PeerWrapper::operator<(PeerWrapper const & other) const {
  return this->peer < other.peer;
}

PeerSet::PeerSet(PeerSet const & other) :
    ff::PeerSet<Identity>(other),
    hasDealer(other.hasDealer),
    dealer(other.dealer),
    recipients(other.recipients),
    dataowners(other.dataowners) {
  this->enforceSort();
}

void PeerSet::add(Identity const & peer) {
  log_assert(peer.role != ROLE_INVALID && peer.role != ROLE_ANALYST);
  if (peer.role == ROLE_DEALER) {
    log_assert(!this->hasDealer);
    this->hasDealer = true;
    this->dealer.peer = peer;
    this->dealer.id = ff::FRONCTOCOLID_INVALID;
    this->dealer.completion = false;
    return;
  } else if (peer.role == ROLE_RECIPIENT) {
    auto place = this->recipients.end();
    while (place > this->recipients.begin() &&
           peer < (place - 1)->peer) {
      place--;
    }
    this->recipients.insert(place, PeerWrapper(peer));
    return;
  } else if (peer.role == ROLE_DATAOWNER) {
    log_assert(peer.vertical != SIZE_MAX);

    while (peer.vertical >= this->dataowners.size()) {
      this->dataowners.emplace_back(0);
    }

    auto place = this->dataowners[peer.vertical].end();
    while (place > this->dataowners[peer.vertical].begin() &&
           peer < (place - 1)->peer) {
      place--;
    }
    this->dataowners[peer.vertical].insert(place, PeerWrapper(peer));
    return;
  }
  log_fatal("Unrecognized role");
  return;
}

void PeerSet::remove(Identity const & peer) {
  log_assert(peer.role != ROLE_INVALID);

  if (peer.role == ROLE_DEALER) {
    log_assert(this->hasDealer);
    this->hasDealer = false;
    PeerWrapper empty;
    this->dealer = empty;
  } else if (peer.role == ROLE_RECIPIENT) {
    auto place = this->recipients.begin();
    while (place != this->recipients.end() && place->peer != peer) {
      place++;
    }
    log_assert(place != this->recipients.end());
    this->recipients.erase(place);
  } else if (peer.role == ROLE_DATAOWNER) {
    log_assert(peer.vertical < this->dataowners.size());
    auto place = this->dataowners[peer.vertical].begin();
    while (place != this->dataowners[peer.vertical].end() &&
           place->peer != peer) {
      place++;
    }
    log_assert(place != this->dataowners[peer.vertical].end());
    this->dataowners[peer.vertical].erase(place);
  }
}

size_t PeerSet::size() const {
  size_t ret = 0;
  if (this->hasDealer) {
    ret++;
  }
  ret += this->recipients.size();
  for (size_t i = 0; i < this->dataowners.size(); i++) {
    ret += this->dataowners[i].size();
  }

  return ret;
}

void PeerSet::enforceSort() {
  std::sort(this->recipients.begin(), this->recipients.end());
  for (std::vector<PeerWrapper> & vert : this->dataowners) {
    std::sort(vert.begin(), vert.end());
  }

  /* remove trailing empty verticals. */
  while (this->dataowners[this->dataowners.size() - 1].size() == 0) {
    this->dataowners.erase(this->dataowners.end() - 1);
  }
}

bool PeerSet::operator==(PeerSet const & other) const {
  if (this->hasDealer != other.hasDealer) {
    return false;
  }
  if (this->hasDealer && this->dealer != other.dealer) {
    return false;
  }

  if (this->recipients.size() != other.recipients.size()) {
    return false;
  }
  for (size_t i = 0; i < this->recipients.size(); i++) {
    if (this->recipients[i] != other.recipients[i]) {
      return false;
    }
  }

  if (this->dataowners.size() != other.dataowners.size()) {
    return false;
  }

  for (size_t i = 0; i < this->dataowners.size(); i++) {
    if (this->dataowners[i].size() != other.dataowners[i].size()) {
      return false;
    }

    for (size_t j = 0; j < this->dataowners[i].size(); j++) {
      if (this->dataowners[i][j] != other.dataowners[i][j]) {
        return false;
      }
    }
  }

  return true;
}

bool PeerSet::operator!=(PeerSet const & other) const {
  return !(*this == other);
}

void PeerSet::forEachDealer(
    std::function<void(Identity const &)> f) const {
  log_debug("for each dealer");
  if (this->hasDealer) {
    log_assert(this->dealer.peer.role == ROLE_DEALER);
    f(this->dealer.peer);
  }
}

void PeerSet::forEachRecipient(
    std::function<void(Identity const &)> f) const {
  log_debug("for each recipient");
  for (size_t i = 0; i < this->recipients.size(); i++) {
    log_assert(this->recipients[i].peer.role == ROLE_RECIPIENT);
    f(this->recipients[i].peer);
  }
}

void PeerSet::forEachDataowner(
    std::function<void(Identity const &)> f) const {
  log_debug("for each dataowner");
  log_debug("this->dataowners.size() %zu", this->dataowners.size());
  for (size_t i = 0; i < this->dataowners.size(); i++) {
    for (size_t j = 0; j < this->dataowners[i].size(); j++) {
      log_assert(this->dataowners[i][j].peer.role == ROLE_DATAOWNER);
      f(this->dataowners[i][j].peer);
    }
  }
}

void PeerSet::forEach(std::function<void(Identity const &)> f) const {
  this->forEachDealer(f);
  this->forEachRecipient(f);
  this->forEachDataowner(f);
}

void PeerSet::forEach(
    std::function<void(Identity const &, ff::fronctocolId_t &, bool &)>
        f) {
  if (this->hasDealer) {
    log_assert(this->dealer.peer.role == ROLE_DEALER);
    f(this->dealer.peer, this->dealer.id, this->dealer.completion);
  }

  for (size_t i = 0; i < this->recipients.size(); i++) {
    log_assert(this->recipients[i].peer.role == ROLE_RECIPIENT);
    f(this->recipients[i].peer,
      this->recipients[i].id,
      this->recipients[i].completion);
  }

  for (size_t i = 0; i < this->dataowners.size(); i++) {
    for (size_t j = 0; j < this->dataowners[i].size(); j++) {
      log_assert(this->dataowners[i][j].peer.role == ROLE_DATAOWNER);
      f(this->dataowners[i][j].peer,
        this->dataowners[i][j].id,
        this->dataowners[i][j].completion);
    }
  }
}

void PeerSet::removeDealer() {
  this->remove(this->dealer.peer);
}

void PeerSet::removeRecipients() {
  this->recipients.clear();
}

void PeerSet::removeDataowners() {
  this->dataowners.clear();
}

void PeerSet::removeVertical(size_t vertical) {
  log_assert(this->dataowners.size() > vertical);
  this->dataowners[vertical].clear();
}

} // namespace safrn
