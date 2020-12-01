/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */

/* 3rd Party Headers */
#include <mpc/Randomness.h>

/* SAFRN Headers */
#include <Identity.h>
#include <PeerSet.h>
#include <Util/Utils.h>
#include <framework/Framework.h>

#ifndef SAFRN_FORTISSIMO_RANDOMNESS_H_
#define SAFRN_FORTISSIMO_RANDOMNESS_H_

namespace safrn {

template<typename Rand_T, typename Info_T>
using RandomnessDispenser =
    ff::mpc::RandomnessDispenser<Rand_T, Info_T>;

template<typename Rand_T, typename Info_T>
using RandomnessGenerator = ff::mpc::RandomnessGenerator<
    Identity,
    PeerSet,
    IncomingMessage,
    OutgoingMessage,
    Rand_T,
    Info_T>;

} // namespace safrn

#endif // SAFRN_FORTISSIMO_RANDOMNESS_H_
