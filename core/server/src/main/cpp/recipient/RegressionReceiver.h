/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_RECIPIENT_REGRESSION_H_
#define SAFRN_RECIPIENT_REGRESSION_H_

#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
#include <io.h>
#else
#include <unistd.h>
#endif

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstdio>
#include <string>
#include <vector>

#include <JSON/Config/StudyConfig.h>
#include <dataowner/Regression.h>
#include <dataowner/RegressionInfo.h>
#include <dataowner/fortissimo.h>
#include <framework/Framework.h>

namespace safrn {
namespace recipient {

std::vector<std::string> findColumnNames(
    std::vector<size_t> const & leftPayloads,
    std::vector<size_t> const & rightPayloads,
    size_t const leftVert,
    size_t const rightVert,
    size_t const depVert,
    bool const fitIntercept,
    StudyConfig const & scfg);

void regressionPrettyPrint(
    std::vector<std::string> const & result_names,
    std::vector<dataowner::LargeNum> const & results,
    dataowner::LargeNum const error,
    dataowner::LargeNum const rsquare,
    size_t const bits_of_precision);

class RegressionReceiver : public Fronctocol {
public:
  std::unique_ptr<dataowner::RegressionInfo const> info;
  std::vector<size_t> const leftPayloads;
  std::vector<size_t> const rightPayloads;
  size_t const leftVert;
  size_t const rightVert;

  StudyConfig const & studyCfg;

  size_t const bitsOfPrecision;

  std::vector<dataowner::LargeNum> results;
  std::vector<dataowner::LargeNum> standardErrorCoeffs;

  std::vector<Boolean_t> F_p_value;
  std::vector<std::vector<Boolean_t>> t_p_values;
  dataowner::LargeNum rootMSE = 0;
  dataowner::LargeNum rsquare = 0;
  size_t numDataowners = 0;

  RegressionReceiver(
      std::unique_ptr<dataowner::RegressionInfo const> i,
      std::vector<size_t> const & l_payloads,
      std::vector<size_t> const & r_payloads,
      size_t lv,
      size_t rv,
      StudyConfig const & scfg,
      size_t const bitsOfPrecision) :
      info(std::move(i)),
      leftPayloads(l_payloads),
      rightPayloads(r_payloads),
      leftVert(lv),
      rightVert(rv),
      studyCfg(scfg),
      bitsOfPrecision(bitsOfPrecision),
      results(
          this->info->verticalDV_numIVs +
              this->info->verticalNonDV_numIVs,
          0),
      standardErrorCoeffs(
          this->info->verticalDV_numIVs +
              this->info->verticalNonDV_numIVs,
          0),
      F_p_value(this->info->bytesInLookupTableCells, 0x00),
      t_p_values(
          this->info->verticalDV_numIVs +
              this->info->verticalNonDV_numIVs,
          std::vector<Boolean_t>(
              this->info->bytesInLookupTableCells, 0x00)) {
  }

  void init() override;

  void handleReceive(IncomingMessage & imsg) override;
  void handleComplete(Fronctocol &) override;
  void handlePromise(Fronctocol &) override;
  std::string name() override;
};

} // namespace recipient
} // namespace safrn

#endif // SAFRN_RECIPIENT_REGRESSION_H_
