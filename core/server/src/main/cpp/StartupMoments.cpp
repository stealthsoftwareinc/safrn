/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <StartupMoments.h>
#include <StartupUtils.h>

/* Logging Config */
#include <ff/logging.h>

namespace safrn {

bool findPayloadMoment(
    MomentFunction const & func,
    std::vector<size_t> & leftPayloads,
    std::vector<size_t> & rightPayloads,
    size_t leftVert,
    size_t rightVert,
    size_t * data_vert,
    StudyConfig const & scfg) {
  if (func.col.vertical == leftVert &&
      func.col.column < scfg.lexicon[leftVert].columns.size()) {
    leftPayloads.push_back(func.col.column);
    *data_vert = leftVert;
    return true;
  } else if (
      func.col.vertical == rightVert &&
      func.col.column < scfg.lexicon[rightVert].columns.size()) {
    rightPayloads.push_back(func.col.column);
    *data_vert = rightVert;
    return true;
  }

  log_error("unrecognized column for moment");
  return false;
}

dataowner::MomentsInfo const * setupMomentsInfo(
    safrn::dataowner::GlobalInfo const * const global_info,
    std::vector<size_t> const & left_payloads,
    std::vector<size_t> const & right_payloads,
    const size_t dataVertical,
    const size_t leftVertical,
    const bool include_count,
    const size_t moment,
    PeerSet const & peers,
    Identity const & id) {
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

  return new dataowner::MomentsInfo(
      global_info,
      numCrossParties,
      id.vertical,
      dataVertical,
      include_count,
      moment,
      dealer,
      revealer);
}

std::unique_ptr<Fronctocol> setupMoments(
    safrn::dataowner::GlobalInfo const * const global_info,
    std::vector<size_t> const & keys,
    std::vector<size_t> const & left_payloads,
    std::vector<size_t> const & right_payloads,
    const size_t dataVertical,
    const size_t leftVertical,
    const bool include_count,
    const size_t moment,
    Identity const & id,
    StudyConfig const & scfg,
    PeerSet const & peers,
    std::string const & csvFile) {

  log_debug("Launching setupMomentsInfo");
  std::unique_ptr<dataowner::MomentsInfo const> rinfo(setupMomentsInfo(
      global_info,
      left_payloads,
      right_payloads,
      dataVertical,
      leftVertical,
      include_count,
      moment,
      peers,
      id));

  ff::mpc::ObservationList<dataowner::LargeNum> oList;

  log_debug("set up info, reading CSV");

  if (!readCSV(
          csvFile,
          oList,
          keys,
          (leftVertical == id.vertical) ? left_payloads :
                                          right_payloads,
          SIZE_MAX,
          scfg,
          id,
          rinfo->startModulus,
          global_info->bitsOfPrecision)) {
    return nullptr;
  }

  /* TODO: Sam
  if (fit_intercept && id.vertical == dataVertical) {
    for (size_t i = 0; i < oList.elements.size(); i++) {
      ArithmeticShare_t dv =
          oList.elements[i].arithmeticPayloadCols.back();
      oList.elements[i].arithmeticPayloadCols.back() = 1;
      oList.elements[i].arithmeticPayloadCols.push_back(dv);
    }
  }
  */

  log_debug("Read CSV, setting up payloads");

  if (id.vertical == dataVertical) {
    for (size_t i = 0; i < oList.elements.size(); i++) {
      oList.elements[i].arithmeticPayloadCols.resize(moment + 1);
      oList.elements[i].arithmeticPayloadCols[1] =
          oList.elements[i].arithmeticPayloadCols[0];
      oList.elements[i].arithmeticPayloadCols[0] = 1;
      for (size_t j = 1; j < moment; j++) {
        oList.elements[i].arithmeticPayloadCols[j + 1] =
            (oList.elements[i].arithmeticPayloadCols[1] *
             oList.elements[i].arithmeticPayloadCols[j]);
      }
    }
  } else {
    for (size_t i = 0; i < oList.elements.size(); i++) {
      oList.elements[i].arithmeticPayloadCols.resize(moment + 1, 0);
    }
  }

  std::unique_ptr<Fronctocol> ret(new dataowner::Moments(
      std::move(oList), global_info, std::move(rinfo)));
  return ret;
}

} // namespace safrn
