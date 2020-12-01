/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_PEER_SET_H_
#define SAFRN_PEER_SET_H_

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
#include <Util/Utils.h>

namespace safrn {

struct PeerSet;

} // namespace safrn

namespace ff {

template<typename Identity_T>
bool msg_read(
    IncomingMessage<Identity_T> & imsg, safrn::PeerSet & peerset);

template<typename Identity_T>
bool msg_write(
    OutgoingMessage<Identity_T> & omsg, safrn::PeerSet const & peerset);

} // namespace ff

namespace safrn {

/**
 * The PeerSet struct breaks down a group of peers by their role,
 * organizing them into vectors of each role.
 *
 * The vectors are intended, but not entirely enforced to be in sorted
 * order. In line with Fortissimo's requirements, the copy constructor
 * will enforce a sorted order.
 */
struct PeerSet : public ff::PeerSet<Identity> {
private:
  struct PeerWrapper {
    Identity peer;
    ff::fronctocolId_t id;
    bool completion;

    PeerWrapper();
    PeerWrapper(Identity const & p);
    PeerWrapper(PeerWrapper const & other);

    bool operator==(PeerWrapper const & other) const;
    bool operator!=(PeerWrapper const & other) const;
    bool operator<(PeerWrapper const & other) const;
  };

  /**
   * Exactly one dealer.
   */
  bool hasDealer = false;
  PeerWrapper dealer;

  /**
   * Zero or more recipients.
   */
  std::vector<PeerWrapper> recipients;

  /**
   * Lists of dataowners, categorized by vertical, and sorted.
   *
   * The outermost vector is indexed similarly to StudyConfig::lexicon.
   * In the case of a vertical having no dataowners participating, then
   * an empty vector is used in its place.
   * The innermost vector is sorted by dataowner's organization id.
   */
  std::vector<std::vector<PeerWrapper>> dataowners;

public:
  PeerSet() = default;
  PeerSet(PeerSet const &);

  /**
   * Add/remove a participant to the set
   */
  void add(Identity const & peer) override;
  void remove(Identity const & peer) override;

  size_t size() const override;

  /**
   * Helper Functions for maintining the sort order.
   */
  void enforceSort();

  /**
   * Comparison operators.
   */
  bool operator==(PeerSet const & other) const;
  bool operator!=(PeerSet const & other) const;

  void forEachDealer(std::function<void(Identity const &)> f) const;
  void forEachRecipient(std::function<void(Identity const &)> f) const;
  void forEachDataowner(std::function<void(Identity const &)> f) const;

  void forEach(std::function<void(Identity const &)> f) const override;
  void forEach(
      std::function<void(
          Identity const &, ff::fronctocolId_t &, bool &)> f) override;

  void removeDealer();
  void removeRecipients();
  void removeDataowners();
  void removeVertical(size_t vertical);

  template<typename Identity_T>
  friend bool
  ff::msg_read(ff::IncomingMessage<Identity_T> &, PeerSet &);

  template<typename Identity_T>
  friend bool
  ff::msg_write(ff::OutgoingMessage<Identity_T> &, PeerSet const &);
};

} // namespace safrn

#include <PeerSet.t.h>

#endif //SAFRN_PEER_SET_H_
