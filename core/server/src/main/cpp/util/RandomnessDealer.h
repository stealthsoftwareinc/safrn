/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */

/* 3rd Party Headers */
#include <mpc/RandomnessDealer.h>

/* SAFRN Headers */
#include <Identity.h>
#include <PeerSet.h>
#include <Util/Utils.h>
#include <framework/Framework.h>
#include <util/Randomness.h>

#ifndef SAFRN_FORTISSIMO_RANDOMNESS_DEALER_H_
#define SAFRN_FORTISSIMO_RANDOMNESS_DEALER_H_

namespace safrn {

template<typename Rand_T, typename Info_T>
using RandomnessHouse = ff::mpc::RandomnessHouse<
    Identity,
    PeerSet,
    IncomingMessage,
    OutgoingMessage,
    Rand_T,
    Info_T>;

template<typename Rand_T, typename Info_T>
using RandomnessPatron = ff::mpc::RandomnessPatron<
    Identity,
    PeerSet,
    IncomingMessage,
    OutgoingMessage,
    Rand_T,
    Info_T>;

} // namespace safrn

#endif // SAFRN_FORTISSIMO_RANDOMNESS_DEALER_H_
