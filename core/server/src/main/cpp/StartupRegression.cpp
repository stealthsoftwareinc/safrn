/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <StartupRegression.h>
#include <StartupUtils.h>

#include <dataowner/Regression.h>
#include <dataowner/RegressionInfo.h>
#include <dataowner/fortissimo.h>
#include <dealer/RegressionHouse.h>

/* Logging Config */
#include <ff/logging.h>

namespace safrn {

bool findPayloadRegression(
    LinearRegressionFunction const & func,
    std::vector<size_t> & leftPayloads,
    std::vector<size_t> & rightPayloads,
    size_t leftVert,
    size_t rightVert,
    size_t * depVertical,
    StudyConfig const & scfg) {
  for (size_t i = 0; i < func.indep_vars.size(); i++) {
    if (func.indep_vars[i].vertical == leftVert) {
      if (func.indep_vars[i].column <
          scfg.lexicon[leftVert].columns.size()) {
        leftPayloads.push_back(func.indep_vars[i].column);
      } else {
        log_error("unrecognized independent variable in left vertical");
        return false;
      }
    } else if (func.indep_vars[i].vertical == rightVert) {
      if (func.indep_vars[i].column <
          scfg.lexicon[rightVert].columns.size()) {
        rightPayloads.push_back(func.indep_vars[i].column);
      } else {
        log_error(
            "unrecognized independent variable in right vertical");
        return false;
      }
    } else {
      log_error("independent variable outside of joined verticals");
      return false;
    }
  }

  if (func.dep_var.vertical == leftVert) {
    *depVertical = leftVert;
    if (func.dep_var.column < scfg.lexicon[leftVert].columns.size()) {
      leftPayloads.push_back(func.dep_var.column);
    } else {
      log_error("unrecognized dependent variable column");
      return false;
    }
  } else if (func.dep_var.vertical == rightVert) {
    *depVertical = rightVert;
    if (func.dep_var.column < scfg.lexicon[rightVert].columns.size()) {
      rightPayloads.push_back(func.dep_var.column);
    } else {
      log_error("unrecognized dependent variable column");
    }
  } else {
    *depVertical = 0;
    log_error("dependent varialbe outside of joined verticals");
    return false;
  }
  return true;
}

dataowner::RegressionInfo const * setupRegressionInfo(
    safrn::dataowner::GlobalInfo const * const global_info_pointer,
    std::vector<size_t> const & left_payloads,
    std::vector<size_t> const & right_payloads,
    size_t dependentVertical,
    size_t leftVertical,
    const bool fit_intercept,
    PeerSet const & peers,
    Identity const & id) {
  size_t vertDV_len;
  size_t vertnDV_len;

  if (leftVertical == dependentVertical) {
    vertDV_len = left_payloads.size() - 1;
    vertnDV_len = right_payloads.size();
  } else {
    vertDV_len = right_payloads.size() - 1;
    vertnDV_len = left_payloads.size();
  }

  if (fit_intercept) {
    vertDV_len++;
  }

  size_t numCrossParties = 0;
  peers.forEachDataowner([&numCrossParties, id](Identity const & oid) {
    if (id.vertical != oid.vertical) {
      numCrossParties++;
    }
  });

  Identity const * dealer = nullptr;
  Identity const * revealer = nullptr;
  peers.forEach([&dealer, &revealer](Identity const & oid) {
    if (dealer == nullptr && oid.role == ROLE_DEALER) {
      dealer = &oid;
    }
    if (revealer == nullptr && oid.role == ROLE_DATAOWNER) {
      revealer = &oid;
    }
  });

  if (dealer == nullptr) {
    log_error("missing dealer party");
    return nullptr;
  }
  if (revealer == nullptr) {
    log_error("missing revealer party");
    return nullptr;
  }

  return new dataowner::RegressionInfo(
      global_info_pointer,
      id.vertical,
      dependentVertical,
      vertDV_len,
      vertnDV_len,
      numCrossParties,
      fit_intercept,
      revealer,
      dealer);
}

std::unique_ptr<Fronctocol> setupRegression(
    safrn::dataowner::GlobalInfo const * const global_info_pointer,
    std::vector<size_t> const & keys,
    std::vector<size_t> const & left_payloads,
    std::vector<size_t> const & right_payloads,
    size_t dependentVertical,
    size_t leftVertical,
    const bool fit_intercept,
    Identity const & id,
    StudyConfig const & scfg,
    PeerSet const & peers,
    std::string const & csvFile,
    std::string const & F_table_file,
    std::string const & t_table_file) {
  std::unique_ptr<dataowner::RegressionInfo const> rinfo(
      setupRegressionInfo(
          global_info_pointer,
          left_payloads,
          right_payloads,
          dependentVertical,
          leftVertical,
          fit_intercept,
          peers,
          id));

  ff::mpc::ObservationList<dataowner::LargeNum> oList;

  size_t extra_precis_col = SIZE_MAX;
  if (id.vertical == dependentVertical) {
    extra_precis_col =
        ((leftVertical == id.vertical) ? left_payloads : right_payloads)
            .back();
  }

  if (!readCSV(
          csvFile,
          oList,
          keys,
          (leftVertical == id.vertical) ? left_payloads :
                                          right_payloads,
          extra_precis_col,
          scfg,
          id,
          rinfo->startModulus,
          global_info_pointer->bitsOfPrecision)) {
    return nullptr;
  }

  if (fit_intercept && id.vertical == dependentVertical) {
    for (size_t i = 0; i < oList.elements.size(); i++) {
      dataowner::LargeNum dv =
          oList.elements[i].arithmeticPayloadCols.back();
      oList.elements[i].arithmeticPayloadCols.back() =
          (dataowner::LargeNum(1)
           << global_info_pointer->bitsOfPrecision);
      oList.elements[i].arithmeticPayloadCols.push_back(dv);
    }
  }

  std::unique_ptr<Fronctocol> ret(new dataowner::Regression(
      std::move(oList),
      F_table_file,
      t_table_file,
      global_info_pointer,
      std::move(rinfo)));
  return ret;
}

} // namespace safrn
