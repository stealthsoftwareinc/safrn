/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */
#include <cstdint>
#include <string>
#include <vector>

/* 3rd Party Headers */
#include <mpc/templates.h>

/* SAFRN Headers */
#include <dataowner/fortissimo.h>
#include <framework/Framework.h>

#ifndef SAFRN_DATAOWNER_SHARE_H_
#define SAFRN_DATAOWNER_SHARE_H_

using SmallNum = uint32_t;

namespace safrn {
namespace dataowner {

template<typename Number_t, typename Info_t>
class Share : public Fronctocol {
public:
  Number_t myShare;

  Share<Number_t, Info_t>(
      Number_t share, Info_t info, const Identity * sha) :
      myShare(share), info(info), sharer(sha) {
  }

  void init() override;
  void handleReceive(IncomingMessage & imsg) override;
  void handleComplete(Fronctocol & f) override;
  void handlePromise(Fronctocol & f) override;
  std::string name() override;

private:
  Info_t info;
  const Identity * sharer;
};

} // namespace dataowner
} // namespace safrn

#endif //SAFRN_DATAOWNER_SHARE_H_
