/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <JSON/Config/ServerConfig.h>

#include <fstream>
#include <iostream>

namespace safrn {
void readServerConfigFromFile(
    std::string const & fname, ServerConfig & cfg) {
  std::ifstream fish(fname);

  if (fish.is_open()) {
    json j = json::parse(fish);

    cfg.port = j["port"];
    strToDbuid(j["organizationId"], cfg.organizationId);
    if (json_contains(j, "address")) {
      cfg.address = j["address"];
    }
    if (json_contains(j, "organizationName")) {
      cfg.organizationName = j["organizationName"];
    }

    for (json const & s : j["approvedStudies"]) {
      ApprovedStudy as;
      as.study = s["study"];
      as.peer = s["peer"];
      as.database = s["database"];
      cfg.approvedStudies.push_back(as);
    }

    auto const certificateAuthorityFile =
        j.find("certificateAuthorityFile");
    auto const certificateAuthorityDirectory =
        j.find("certificateAuthorityDirectory");
    if ((certificateAuthorityFile == j.end() ||
         !certificateAuthorityFile->is_string() ||
         certificateAuthorityFile->get_ref<json::string_t &>()
             .empty()) &&
        (certificateAuthorityDirectory == j.end() ||
         !certificateAuthorityDirectory->is_string() ||
         certificateAuthorityDirectory->get_ref<json::string_t &>()
             .empty())) {
      throw std::runtime_error(
          "At least one of certificateAuthorityFile and "
          "certificateAuthorityDirectory must be specified");
    }
    if (certificateAuthorityFile != j.end()) {
      cfg.certificateAuthorityFile = *certificateAuthorityFile;
    }
    if (certificateAuthorityDirectory != j.end()) {
      cfg.certificateAuthorityDirectory =
          *certificateAuthorityDirectory;
    }

    auto const certificateFile = j.find("certificateFile");
    if (certificateFile == j.end() || !certificateFile->is_string() ||
        certificateFile->get_ref<json::string_t &>().empty()) {
      throw std::runtime_error("certificateFile must be specified");
    }
    cfg.certificateFile = *certificateFile;

    auto const certificateKeyFile = j.find("certificateKeyFile");
    if (certificateKeyFile == j.end() ||
        !certificateKeyFile->is_string() ||
        certificateKeyFile->get_ref<json::string_t &>().empty()) {
      throw std::runtime_error("certificateKeyFile must be specified");
    }
    cfg.certificateKeyFile = *certificateKeyFile;
  } else {
    throw std::runtime_error("Cannot open server config file.");
  }
}
} // namespace safrn
