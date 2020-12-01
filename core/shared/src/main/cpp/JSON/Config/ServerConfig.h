/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/* C and POSIX Headers */

/* C++ Headers */
#include <cstdint>
#include <string>
#include <vector>

/* 3rd Party Heades */

/* SAFRN Headers */
#include <Util/Utils.h>
#ifndef SAFRN_SERVER_CONFIG_H_
#define SAFRN_SERVER_CONFIG_H_

namespace safrn {

/**
   * helper class for carrying file names of a study's config files.
   */
struct ApprovedStudy {
  std::string study;
  std::string peer;
  std::string database;
};

struct ServerConfig {
  /**
     * Network device to listen on.
     */
  std::string address = "0.0.0.0";

  /**
     * TCP Port number to listen for connections on.
     */
  uint16_t port;

  /**
   * TLS configuration stuff.
   */
  std::string certificateAuthorityFile;
  std::string certificateAuthorityDirectory;
  std::string certificateFile;
  std::string certificateKeyFile;

  /**
     * list of configuration files for approved studies.
     */
  std::vector<ApprovedStudy> approvedStudies;

  /**
     * Organization Id which this server represents.
     */
  dbuid_t organizationId;

  /**
   * Name of the organization which this server represents. Nonauthoratitive
   * convenience attribute.
   */
  std::string organizationName = "unknown";
};

void readServerConfigFromFile(
    std::string const & fname, ServerConfig & cfg);
} // namespace safrn

#endif //SAFRN_SERVER_CONFIG_H_
