/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */

/* 3rd Party Headers */
#include <ff/Fronctocol.h>
#include <ff/Promise.h>
#include <ff/posixnet/posixnet.h>

/* SAFRN Headers */
#include <Identity.h>
#include <PeerSet.h>
#include <Util/Utils.h>

#ifndef SAFRN_FORTISSIMO_FRAMEWORK_H_
#define SAFRN_FORTISSIMO_FRAMEWORK_H_

namespace safrn {

using IncomingMessage = ff::posixnet::IncomingMessage<Identity>;
using OutgoingMessage = ff::posixnet::OutgoingMessage<Identity>;

#define SAFRN_TYPES \
  ::safrn::Identity, ::safrn::PeerSet, ::safrn::IncomingMessage, \
      ::safrn::OutgoingMessage

using Fronctocol = ff::Fronctocol<SAFRN_TYPES>;

template<typename Result_T>
using Promise = ff::Promise<SAFRN_TYPES, Result_T>;

template<typename Result_T>
using PromiseFronctocol = ff::PromiseFronctocol<SAFRN_TYPES, Result_T>;

} // namespace safrn

#endif // SAFRN_FORTISSIMO_FRAMEWORK_H_
