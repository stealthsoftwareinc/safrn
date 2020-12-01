/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* c/c++ standard includes */
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

/* third-party library includes */
#include <nlohmann/json.hpp>
#include <openssl/evp.h>
#include <sst/rep/from_hex.h>

/* project-specific includes */
#include <EventWrapper/evwrapper.h>
#include <JSON/Columns/ColumnFactory.h>
#include <JSON/Query/FunctionFactory.h>
#include <Util/Utils.h>

/* same module include */
#include <JSON/Config/StudyConfig.h>

#include <ff/logging.h>

namespace safrn {

/**
   * Helper class for holding peer connection info during processing.
   */
struct PeerConnInfo {
  std::string domainOrIp;
  uint16_t port = 0;
  EVP_MD const * certificateFingerprintAlgorithm;
  std::vector<unsigned char> certificateFingerprint;
};

static void
configFromJson(json const & sjs, json const & pjs, StudyConfig & cfg) {
  /* First read connection info from peer config file into temp map */
  std::map<dbuid_t, PeerConnInfo> peerConnInfo;
  for (json const & peer : pjs["peers"]) {
    dbuid_t orgId;
    strToDbuid(peer["organizationId"], orgId);
    peerConnInfo[orgId].domainOrIp = peer["domainOrIp"];
    peerConnInfo[orgId].port = peer["port"];
    peerConnInfo[orgId].certificateFingerprintAlgorithm =
        &getHashAlgorithmByName(
            peer["certificateFingerprintAlgorithm"]);
    {
      auto const & src = peer["certificateFingerprint"]
                             .get_ref<json::string_t const &>();
      auto & dst = peerConnInfo[orgId].certificateFingerprint;
      sst::rep::from_hex(
          src.cbegin(), src.cend(), std::back_inserter(dst));
    }
  }

  /* Now start reading the Study Config */
  strToDbuid(sjs["studyId"], cfg.studyId);
  if (json_contains(sjs, "maxListSize")) {
    cfg.maxListSize = sjs["maxListSize"];
  } else {
    cfg.maxListSize = 100;
  }

  /* Read out the list of lexicons */
  VerticalIndex_t current_vertical_index = 0;
  for (json const & vertical : sjs["lexicon"]) {
    cfg.lexicon.emplace_back();
    Vertical & vert = cfg.lexicon.back();

    if (json_contains(vertical, "verticalIndex")) {
      vert.verticalIndex = vertical["verticalIndex"];
      if (vert.verticalIndex != current_vertical_index) {
        throw std::runtime_error("Incompatible vertical index.");
      }
    } else {
      vert.verticalIndex = current_vertical_index;
    }
    ++current_vertical_index;

    ColumnIndex_t current_col_index = 0;
    for (json const & column : vertical["columns"]) {
      vert.columns.emplace_back(ColumnFactory::createColumn(column));
      if (vert.columns.back()->index != current_col_index) {
        throw std::runtime_error("Incompatible vertical index.");
      }
      ++current_col_index;
    }
  }

  /* Read the allowed queries. */
  if (json_contains(sjs, "allowedQueries")) {
    //cfg.allowedQueries.push_back(FunctionFactory(sjs["allowedQueries"]));
    for (json const & query : sjs["allowedQueries"]) {
      cfg.allowedQueries.push_back(FunctionFactory(query));
    }
  }

  /* Read out the peer info from the study config file. */
  for (json const & peer : sjs["peers"]) {
    dbuid_t orgId;
    strToDbuid(peer["organizationId"], orgId);
    Peer & p = cfg.peers[orgId];

    if (!map_contains(peerConnInfo, orgId)) {
      throw std::runtime_error("Missing a peer from peer config file");
    }

    p.organizationId = orgId;
    p.domainOrIp = peerConnInfo[orgId].domainOrIp;
    p.port = peerConnInfo[orgId].port;
    p.certificateFingerprintAlgorithm =
        peerConnInfo[orgId].certificateFingerprintAlgorithm;
    p.certificateFingerprint =
        peerConnInfo[orgId].certificateFingerprint;
    if (json_contains(peer, "organizationName")) {
      p.organizationName = peer["organizationName"];
    }

    p.role = 0x00;
    if (json_contains(peer, "analyst")) {
      p.role |= ROLE_ANALYST;
      p.analyst.allowedQueryCount =
          peer["analyst"]["allowedQueryCount"];
    } else {
      p.analyst.allowedQueryCount = 0;
    }

    if (json_contains(peer, "dataowner")) {
      p.role |= ROLE_DATAOWNER;
      p.dataowner.verticalIdx = peer["dataowner"]["vertical"];
    } else {
      p.dataowner.verticalIdx =
          std::numeric_limits<VerticalIndex_t>::max();
    }

    if (json_contains(peer, "dealer")) {
      p.role |= ROLE_DEALER;
    }

    if (json_contains(peer, "recipient")) {
      p.role |= ROLE_RECIPIENT;
    }
  }
}

static void configFromJson(json const & sjs, StudyConfig & cfg) {
  strToDbuid(sjs["studyId"], cfg.studyId);
  if (json_contains(sjs, "maxListSize")) {
    cfg.maxListSize = sjs["maxListSize"];
  } else {
    cfg.maxListSize = 100;
  }

  /* Read out the list of lexicons */
  VerticalIndex_t current_vertical_index = 0;
  for (json const & vertical : sjs["lexicon"]) {
    cfg.lexicon.emplace_back();
    Vertical & vert = cfg.lexicon.back();

    if (json_contains(vertical, "verticalIndex")) {
      vert.verticalIndex = vertical["verticalIndex"];
      if (vert.verticalIndex != current_vertical_index) {
        throw std::runtime_error("Incompatible vertical index.");
      }
    } else {
      vert.verticalIndex = current_vertical_index;
    }
    ++current_vertical_index;

    ColumnIndex_t current_col_index = 0;
    for (json const & column : vertical["columns"]) {
      vert.columns.emplace_back(ColumnFactory::createColumn(column));
      if (vert.columns.back()->index != current_col_index) {
        throw std::runtime_error("Incompatible vertical index.");
      }
      ++current_col_index;
    }
  }

  /* Read the allowed queries. */
  if (json_contains(sjs, "allowedQueries")) {
    for (json const & query : sjs["allowedQueries"]) {
      cfg.allowedQueries.push_back(FunctionFactory(query));
    }
  }

  /* Read out the peer info from the study config file. */
  for (json const & peer : sjs["peers"]) {
    dbuid_t orgId;
    strToDbuid(peer["organizationId"], orgId);
    Peer & p = cfg.peers[orgId];
    p.organizationId = orgId;
    if (json_contains(peer, "organizationName")) {
      p.organizationName = peer["organizationName"];
    }

    p.role = 0x00;
    if (json_contains(peer, "analyst")) {
      p.role |= ROLE_ANALYST;
      p.analyst.allowedQueryCount =
          peer["analyst"]["allowedQueryCount"];
    } else {
      p.analyst.allowedQueryCount = 0;
    }

    if (json_contains(peer, "dataowner")) {
      p.role |= ROLE_DATAOWNER;
      p.dataowner.verticalIdx = peer["dataowner"]["vertical"];
    } else {
      p.dataowner.verticalIdx =
          std::numeric_limits<VerticalIndex_t>::max();
    }

    if (json_contains(peer, "dealer")) {
      p.role |= ROLE_DEALER;
    }

    if (json_contains(peer, "recipient")) {
      p.role |= ROLE_RECIPIENT;
    }
  }
}

StudyConfig readStudyFromJson(json const & study, json const & peers) {
  StudyConfig to_return;
  configFromJson(study, peers, to_return);
  return to_return;
}

StudyConfig readStudyFromJson(json const & study) {
  StudyConfig to_return;
  configFromJson(study, to_return);
  return to_return;
}

void readStudyFromFile(
    std::string const & sessFile,
    std::string const & peerFile,
    StudyConfig & cfg) {
  std::ifstream sfish(sessFile);
  std::ifstream pfish(peerFile);

  if (sfish.is_open() && pfish.is_open()) {
    configFromJson(json::parse(sfish), json::parse(pfish), cfg);
  } else {
    if (!(sfish.is_open() || pfish.is_open())) {
      throw std::runtime_error(
          "Cannot open study or peer config files.");
    } else if (!sfish.is_open()) {
      throw std::runtime_error("Cannot open study config file.");
    } else {
      throw std::runtime_error("Cannot open peer config file.");
    }
  }
}

bool Peer::isAnalyst() const {
  return this->hasRole(ROLE_ANALYST);
}

bool Peer::isDataowner() const {
  return this->hasRole(ROLE_DATAOWNER);
}

bool Peer::isDealer() const {
  return this->hasRole(ROLE_DEALER);
}

bool Peer::isRecipient() const {
  return this->hasRole(ROLE_RECIPIENT);
}

bool Peer::hasRole(role_t r) const {
  return (this->role & r) == r;
}

size_t StudyConfig::getVerticalIdx(dbuid_t const orgId) const {
  return (map_contains(this->peers, orgId) &&
          this->peers.at(orgId).isDataowner()) ?
      this->peers.at(orgId).dataowner.verticalIdx :
      SIZE_MAX;
}

} // namespace safrn
