/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */

/* 3rd Party Headers */
#include <ff/posixnet/posixnet.h>
#include <ff/tester/Tester.h>
#include <ff/tester/runTests.h>

/* SAFRN Headers */
#include <Identity.h>
#include <PeerSet.h>
#include <Util/Utils.h>
#include <framework/Framework.h>
#include <framework/TestRunner.h>

namespace safrn {

static std::function<std::unique_ptr<IncomingMessage>(
    Identity const &, OutgoingMessage &)>
    message_converter =
        ff::posixnet::outgoingToIncomingMessage<Identity>;

bool runTests(
    std::map<Identity, std::unique_ptr<Fronctocol>> & tests,
    uint64_t seed) {
  return ff::tester::
      runTests<Identity, PeerSet, IncomingMessage, OutgoingMessage>(
          tests, message_converter, seed);
}

bool runTests(std::map<Identity, std::unique_ptr<Fronctocol>> & tests) {
  return ff::tester::
      runTests<Identity, PeerSet, IncomingMessage, OutgoingMessage>(
          tests, message_converter);
}

const ::std::function<void(IncomingMessage &, Fronctocol *)>
    failTestOnReceive = ff::tester::failTestOnReceive<
        Identity,
        PeerSet,
        IncomingMessage,
        OutgoingMessage>();

const ::std::function<void(Fronctocol &, Fronctocol *)>
    failTestOnComplete = ff::tester::failTestOnComplete<
        Identity,
        PeerSet,
        IncomingMessage,
        OutgoingMessage>();

const ::std::function<void(Fronctocol &, Fronctocol *)>
    finishTestOnComplete = ff::tester::finishTestOnComplete<
        Identity,
        PeerSet,
        IncomingMessage,
        OutgoingMessage>();

const ::std::function<void(Fronctocol &, Fronctocol *)>
    failTestOnPromise = ff::tester::failTestOnPromise<
        Identity,
        PeerSet,
        IncomingMessage,
        OutgoingMessage>();

const ::std::function<void(Fronctocol &, Fronctocol *)>
    finishTestOnPromise = ff::tester::finishTestOnPromise<
        Identity,
        PeerSet,
        IncomingMessage,
        OutgoingMessage>();

} // namespace safrn
