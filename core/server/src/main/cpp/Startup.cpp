/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <Startup.h>
#include <StartupMoments.h>
#include <StartupRegression.h>
#include <StartupUtils.h>

#include <dataowner/GlobalInfo.h>
#include <dealer/MomentsHouse.h>
#include <dealer/RegressionHouse.h>
#include <recipient/MomentsReceiver.h>
#include <recipient/RegressionReceiver.h>

/* Logging Config */
#include <ff/logging.h>

namespace safrn {

static inline bool findKeyCols(
    JoinStatement const & join,
    std::vector<size_t> & leftKeys,
    std::vector<size_t> & rightKeys,
    size_t * leftVert,
    size_t * rightVert,
    StudyConfig const & scfg) {
  *leftVert = join.joinOns[0].first.col.vertical;
  *rightVert = join.joinOns[0].second.col.vertical;
  Vertical const & lvert = scfg.lexicon[*leftVert];
  Vertical const & rvert = scfg.lexicon[*rightVert];

  for (size_t i = 0; i < join.joinOns.size(); i++) {
    if (join.joinOns[i].first.col.vertical != lvert.verticalIndex) {
      log_error("column of wrong vertical");
      return false;
    }
    if (join.joinOns[i].second.col.vertical != rvert.verticalIndex) {
      log_error("column of wrong vertical");
      return false;
    }

    if (lvert.columns.size() > join.joinOns[i].first.col.column) {
      leftKeys.push_back(join.joinOns[i].first.col.column);
    } else {
      log_error("unrecognized column");
      return false;
    }

    if (rvert.columns.size() > join.joinOns[i].second.col.column) {
      rightKeys.push_back(join.joinOns[i].second.col.column);
    } else {
      log_error("unrecognized column");
      return false;
    }
  }
  return true;
}

std::unique_ptr<Fronctocol> startup(
    std::string const & csvFile,
    std::string const & lookupTableDirectory,
    Query const & q,
    StudyConfig const & scfg,
    Identity const & id,
    PeerSet & peers) {
  std::vector<size_t> left_keys;
  std::vector<size_t> right_keys;

  size_t leftVert;
  size_t rightVert;

  if (!findKeyCols(
          *q.joinStatement,
          left_keys,
          right_keys,
          &leftVert,
          &rightVert,
          scfg)) {
    return nullptr;
  }

  for (std::pair<dbuid_t, Peer> const & pair : scfg.peers) {
    dbuid_t dbuid = pair.first;
    Peer const & peer = pair.second;

    if (peer.isRecipient()) {
      Identity recip_id(dbuid, ROLE_RECIPIENT, SIZE_MAX);
      peers.add(recip_id);
    }
    if (peer.isDealer()) {
      Identity dealer_id(dbuid, ROLE_DEALER, SIZE_MAX);
      peers.add(dealer_id);
    }
    if (peer.isDataowner()) {
      Identity dataowner_id(
          dbuid, ROLE_DATAOWNER, peer.dataowner.verticalIdx);
      if (leftVert == dataowner_id.vertical ||
          rightVert == dataowner_id.vertical) {
        peers.add(dataowner_id);
      }
    }
  }

  if (!peers.hasPeer(id)) {
    return nullptr;
  }

  // Set GlobalInfo from config.

  safrn::dataowner::GlobalInfo global_info =
      safrn::dataowner::generateGlobals(q, scfg);
  safrn::dataowner::GlobalInfo * global_info_pointer =
      new safrn::dataowner::GlobalInfo(global_info);

  std::vector<size_t> left_payloads;
  std::vector<size_t> right_payloads;

  if (q.function->type == FunctionType::LIN_REGRESSION) {
    size_t dep_vert;
    if (!findPayloadRegression(
            static_cast<LinearRegressionFunction &>(*q.function),
            left_payloads,
            right_payloads,
            leftVert,
            rightVert,
            &dep_vert,
            scfg)) {
      return nullptr;
    }

    bool fit_intercept =
        static_cast<LinearRegressionFunction &>(*q.function)
            .fit_intercept;

    if (id.role == ROLE_DATAOWNER) {

      size_t num_true_ivs =
          left_payloads.size() + right_payloads.size() - 1;
      std::string F_table_file = lookupTableDirectory +
          "f_table_num_ivs_" + std::to_string(num_true_ivs) + ".csv";
      std::string t_table_file = lookupTableDirectory + "t_table.csv";
      return setupRegression(
          global_info_pointer,
          (id.vertical == leftVert ? left_keys : right_keys),
          left_payloads,
          right_payloads,
          dep_vert,
          leftVert,
          fit_intercept,
          id,
          scfg,
          peers,
          csvFile,
          F_table_file,
          t_table_file);
    } else if (id.role == ROLE_DEALER) {
      std::unique_ptr<dataowner::RegressionInfo const> rinfo(
          setupRegressionInfo(
              global_info_pointer,
              left_payloads,
              right_payloads,
              dep_vert,
              leftVert,
              fit_intercept,
              peers,
              id));

      std::unique_ptr<Fronctocol> ret(
          new dealer::RegressionRandomnessHouse(
              global_info_pointer, std::move(rinfo)));
      return ret;
    } else if (id.role == ROLE_RECIPIENT) {
      std::unique_ptr<dataowner::RegressionInfo const> rinfo(
          setupRegressionInfo(
              global_info_pointer,
              left_payloads,
              right_payloads,
              dep_vert,
              leftVert,
              fit_intercept,
              peers,
              id));

      std::unique_ptr<Fronctocol> ret(new recipient::RegressionReceiver(
          std::move(rinfo),
          left_payloads,
          right_payloads,
          leftVert,
          rightVert,
          scfg,
          global_info_pointer->bitsOfPrecision));
      return ret;
    }
  } else if (q.function->type == FunctionType::MOMENT) {
    size_t data_vert;
    if (!findPayloadMoment(
            static_cast<MomentFunction &>(*q.function),
            left_payloads,
            right_payloads,
            leftVert,
            rightVert,
            &data_vert,
            scfg)) {
      return nullptr;
    }

    const MomentFunction & function =
        *((MomentFunction *)q.function.get());
    size_t moment = static_cast<size_t>(function.momentType.value);
    // Determine if Count (Moment 0) should be revealed to Analyst.
    // This is determined via study config.
    // For now, just look through all functions of study config,
    // and see if any of them turn this on.
    // TODO: Later, as our whitelist/blacklist/rules for allowing/disallowing
    // queries is implemented, this logic will need to be updated.
    bool include_count = false;
    for (const auto & f : scfg.allowedQueries) {
      if (f->type == FunctionType::MOMENT) {
        const MomentFunction & func = *((MomentFunction *)f.get());
        if (func.revealCount) {
          include_count = true;
          break;
        }
      }
    }

    if (id.role == ROLE_DATAOWNER) {
      return setupMoments(
          global_info_pointer,
          (id.vertical == leftVert ? left_keys : right_keys),
          left_payloads,
          right_payloads,
          data_vert,
          leftVert,
          include_count,
          moment,
          id,
          scfg,
          peers,
          csvFile);
    } else if (id.role == ROLE_DEALER) {
      std::unique_ptr<dataowner::MomentsInfo const> rinfo(
          setupMomentsInfo(
              global_info_pointer,
              left_payloads,
              right_payloads,
              data_vert,
              leftVert,
              include_count,
              moment,
              peers,
              id));

      std::unique_ptr<Fronctocol> ret(
          new dealer::MomentsRandomnessHouse(
              global_info_pointer, std::move(rinfo)));
      return ret;
    } else if (id.role == ROLE_RECIPIENT) {
      /* TODO */

      std::unique_ptr<dataowner::MomentsInfo const> rinfo(
          setupMomentsInfo(
              global_info_pointer,
              left_payloads,
              right_payloads,
              data_vert,
              leftVert,
              include_count,
              moment,
              peers,
              id));

      std::unique_ptr<Fronctocol> ret(new recipient::MomentsReceiver(
          global_info_pointer->bitsOfPrecision, std::move(rinfo)));

      return ret;
    }
  } else {
    log_error("unsupported function");
    return nullptr;
  }

  return nullptr;
}

} // namespace safrn
