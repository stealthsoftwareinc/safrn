/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */

/* 3rd Party Headers */
#include <ff/tester/Tester.h>
#include <ff/tester/runTests.h>

/* SAFRN Headers */
#include <Identity.h>
#include <PeerSet.h>
#include <Util/Utils.h>
#include <framework/Framework.h>

#ifndef SAFRN_FORTISSIMO_TEST_RUNNER_H_
#define SAFRN_FORTISSIMO_TEST_RUNNER_H_

namespace safrn {

using Tester = ff::tester::
    Tester<Identity, PeerSet, IncomingMessage, OutgoingMessage>;

/* Wrappers for the runTests function. */
bool runTests(
    std::map<Identity, std::unique_ptr<Fronctocol>> & tests,
    uint64_t seed);
bool runTests(std::map<Identity, std::unique_ptr<Fronctocol>> & tests);

extern const ::std::function<void(IncomingMessage &, Fronctocol *)>
    failTestOnReceive;

extern const ::std::function<void(Fronctocol &, Fronctocol *)>
    failTestOnComplete;

extern const ::std::function<void(Fronctocol &, Fronctocol *)>
    finishTestOnComplete;

extern const ::std::function<void(Fronctocol &, Fronctocol *)>
    failTestOnPromise;

extern const ::std::function<void(Fronctocol &, Fronctocol *)>
    finishTestOnPromise;

} // namespace safrn

#endif // SAFRN_FORTISSIMO_TEST_RUNNER_H_
