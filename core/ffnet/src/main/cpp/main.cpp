/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include <nlohmann/json.hpp>

#include <ff/posixnet/posixnet.h>

#include <Identity.h>
#include <Startup.h>
#include <Util/Utils.h>
#include <framework/Framework.h>

#include <ff/logging.h>

using namespace safrn;

void printHelp() {
  fprintf(stderr, "SAFRN FFnet runner.\n");
  fprintf(
      stderr,
      "Copyright (C) 2020 Stealth Software Technologies Commercial, "
      "Inc.\n\n");
  fprintf(stderr, "USAGE:\n");
  fprintf(
      stderr,
      "  safrnffnet --orgid {ORG ID} --port {portnum} [ --role {ROLE} "
      "] [ --study {study.json} ] [ --peers {peers.json} ] [ --query "
      "{query.json} ] [ --data {data.csv} ] [ --lookups {lookupsdir/} "
      "]\n\n");
  fprintf(stderr, "OPTIONS:\n");
  fprintf(
      stderr,
      "--orgid        (required) Organization ID component of "
      "identity.\n");
  fprintf(
      stderr,
      "          {128 bit hex-encoded number with leading zeros}\n");
  fprintf(
      stderr,
      "--port         (required) This participant\'s server port "
      "number.\n");
  fprintf(stderr, "          {TCP/IP Port Number}\n");
  fprintf(
      stderr,
      "--role         (if ambiguous) The role component of "
      "identity.\n");
  fprintf(stderr, "          dataowner|dealer|recipient\n");
  fprintf(
      stderr,
      "--study        (default \"./study.json\") study config path.\n");
  fprintf(
      stderr,
      "--peers        (default \"./peers.json\") peers config path.\n");
  fprintf(
      stderr,
      "--data         (default \"./data.csv\") dataowner's CSV data "
      "file.\n");
  fprintf(
      stderr,
      "--lookups      (default \"./lookups/\") dataowner's lookup "
      "table "
      "files.\n");
  fprintf(
      stderr,
      "--query        (default \"./query.json\") the query definition "
      "file.\n");
  fprintf(stderr, "--help         prints the help text.\n");
}

std::string orgstr = "";
dbuid_t orgid = DBUID_INVALID;
std::string role = "";
uint16_t portno;

std::string study = "study.json";
std::string peers = "peers.json";
std::string data = "data.csv";
std::string lookups = "lookups/";
std::string query = "query.json";

void argsParse(size_t const argc, char const * const argv[]) {
  bool invalid = false;
  bool has_org = false;
  bool has_port = false;
  for (size_t i = 1; i < argc; i++) {
    std::string arg(argv[i]);
    if (arg == "--orgid") {
      if (i + 1 == argc) {
        fprintf(stderr, "Missing orgid value\n");
        invalid = true;
        break;
      }
      orgstr = std::string(argv[++i]);
      has_org = true;
    } else if (arg == "--port") {
      try {
        portno = (uint16_t)stoi(std::string(argv[++i]));
        has_port = true;
      } catch (std::logic_error le) {
        fprintf(stderr, "Invalid port number, %s\n", le.what());
        has_port = false;
        invalid = false;
      }
    } else if (arg == "--role") {
      if (i + 1 == argc) {
        fprintf(stderr, "Missing role value\n");
        invalid = true;
        break;
      }
      role = std::string(argv[++i]);
      if (role != "dataowner" && role != "dealer" &&
          role != "recipient") {
        invalid = true;
        fprintf(
            stderr,
            "Invalid role string. expected one of \"dataowner\", "
            "\"dealer\", or \"recipient\"\n");
      }
    } else if (arg == "--study") {
      if (i + 1 == argc) {
        fprintf(stderr, "Missing study file\n");
        invalid = true;
        break;
      }
      study = std::string(argv[++i]);
    } else if (arg == "--peers") {
      if (i + 1 == argc) {
        fprintf(stderr, "Missing peers file\n");
        invalid = true;
        break;
      }
      peers = std::string(argv[++i]);
    } else if (arg == "--data") {
      if (i + 1 == argc) {
        fprintf(stderr, "Missing data file\n");
        invalid = true;
        break;
      }
      data = std::string(argv[++i]);
    } else if (arg == "--lookups") {
      if (i + 1 == argc) {
        fprintf(stderr, "Missing lookup table directory\n");
        invalid = true;
        break;
      }
      lookups = std::string(argv[++i]);
    } else if (arg == "--query") {
      if (i + 1 == argc) {
        fprintf(stderr, "Missing query file\n");
        invalid = true;
        break;
      }
      query = std::string(argv[++i]);
    } else if (arg == "--help") {
      printHelp();
      exit(0);
    } else {
      fprintf(stderr, "Unrecognized argument \"%s\"\n", arg.c_str());
      invalid = true;
      break;
    }
  }

  if (!has_org) {
    fprintf(stderr, "Missing organization ID.\n");
    invalid = true;
  } else {
    try {
      strToDbuid(orgstr, orgid);
    } catch (std::runtime_error e) {
      fprintf(stderr, "Invalid orgid: %s\n", e.what());
      invalid = true;
    }
  }

  if (!has_port) {
    fprintf(stderr, "missing port number\n");
    invalid = true;
  }

  if (invalid) {
    printHelp();
    exit(1);
  }
}

bool singleRole(role_t const r) {
  size_t n_roles = 0;
  if ((r & ROLE_DATAOWNER) != 0) {
    n_roles++;
  }
  if ((r & ROLE_RECIPIENT) != 0) {
    n_roles++;
  }
  if ((r & ROLE_DEALER) != 0) {
    n_roles++;
  }
  return n_roles == 1;
}

Identity chooseMyId(StudyConfig const & scfg) {
  auto const & find_peer = scfg.peers.find(orgid);
  role_t actual_role = ROLE_INVALID;
  size_t actual_vertical = SIZE_MAX;
  if (find_peer == scfg.peers.end()) {
    log_error(
        "Organization \"%s\" not a participatn to study.",
        dbuidToStr(orgid).c_str());
    exit(1);
  } else {
    Peer const & peer = find_peer->second;
    if (role == "" && singleRole(peer.role)) {
      actual_role = peer.role;
    } else if (role == "dataowner" && peer.hasRole(ROLE_DATAOWNER)) {
      actual_role = ROLE_DATAOWNER;
    } else if (role == "dealer" && peer.hasRole(ROLE_DEALER)) {
      actual_role = ROLE_DEALER;
    } else if (role == "recipient" && peer.hasRole(ROLE_RECIPIENT)) {
      actual_role = ROLE_RECIPIENT;
    } else {
      log_info("Could not determine role.");
      exit(1);
    }
    if (actual_role == ROLE_DATAOWNER) {
      actual_vertical = peer.dataowner.verticalIdx;
    }
  }

  return Identity(orgid, actual_role, actual_vertical);
}

void setupPeersInfo(
    std::vector<ff::posixnet::PeerInfo<Identity>> & peers_info,
    StudyConfig const & scfg,
    Identity const & my_id,
    PeerSet const & ps) {
  ps.forEach([&](Identity const & other) {
    log_debug("adding peer %s", dbuidToStr(other.orgId).c_str());

    peers_info.emplace_back();
    Peer const & peer = scfg.peers.find(other.orgId)->second;

    peers_info.back().identity = other;

    uint16_t actual_portno = (my_id == other) ? portno : peer.port;
    std::string ip_str =
        (my_id == other) ? std::string("0.0.0.0") : peer.domainOrIp;
    log_debug(
        "adding address %s, port %hu", ip_str.c_str(), actual_portno);

    peers_info.back().address.ss_family = AF_INET;
    ((sockaddr_in *)&peers_info.back().address)->sin_port =
        htons(actual_portno);

    if (1 !=
        inet_pton(
            AF_INET,
            ip_str.c_str(),
            &((sockaddr_in *)&peers_info.back().address)->sin_addr)) {
      log_error("invalid IPv4 Address \"%s\"", ip_str.c_str());
      exit(1);
    }
  });
}

int main(int argc, char const * const argv[]) {
  LOG_FILE = stderr;

  argsParse(size_t(argc), argv);

  try {
    std::ifstream study_stream(study);
    if (!study_stream.is_open()) {
      log_error("Could not open file \"%s\"", study.c_str());
      return 1;
    }
    nlohmann::json study_json = nlohmann::json::parse(study_stream);

    std::ifstream peers_stream(peers);
    if (!peers_stream.is_open()) {
      log_error("Could not open file \"%s\"", peers.c_str());
      return 1;
    }
    nlohmann::json peers_json = nlohmann::json::parse(peers_stream);

    std::ifstream query_stream(query);
    if (!query_stream.is_open()) {
      log_error("Could not open file \"%s\"", query.c_str());
      return 1;
    }
    nlohmann::json query_json = nlohmann::json::parse(query_stream);

    const StudyConfig scfg = readStudyFromJson(study_json, peers_json);

    const Query the_query(scfg, query_json);

    Identity my_id = chooseMyId(scfg);
    LOG_ORGANIZATION =
        scfg.peers.find(my_id.orgId)->second.organizationName;

    PeerSet ps;
    std::unique_ptr<Fronctocol> fronctocol =
        startup(data, lookups, the_query, scfg, my_id, ps);
    std::vector<ff::posixnet::PeerInfo<Identity>> peers_info;
    setupPeersInfo(peers_info, scfg, my_id, ps);
    ff::posixnet::runFortissimoPosixNet(
        std::move(fronctocol), peers_info, my_id);
  } catch (std::runtime_error re) {
    log_error("Error: %s", re.what());
    return 1;
  } catch (std::logic_error le) {
    log_error("Error: %s", le.what());
    return 1;
  } catch (std::exception e) {
    log_error("Error: %s", e.what());
    return 1;
  }

  return 0;
}
