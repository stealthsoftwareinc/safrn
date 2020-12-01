/**
 * Copyright Stealth Software Technologies, Inc.
 */

/* C and POSIX headers */

/* C++ Headers */
#include <fstream>
#include <string>
#include <vector>

/* 3rd Party Headers */
#include <nlohmann/json.hpp>

/* SAFRN Headers */
#include <Util/read_file_utils.h>
#include <Util/socket.h>
#include <Util/socket_utils.h>
#include <Util/string_utils.h>

/* Logging Configuration */
#include <ff/logging.h>

//PHB using namespace safrn;
using namespace networking;
using namespace std;

void ParsePeers(
    nlohmann::json const & peers_json,
    vector<pair<string, unsigned long>> * peers) {
  for (nlohmann::json const & peer : peers_json["peers"]) {
    peers->push_back(make_pair(peer["domainOrIp"], peer["port"]));
  }
}

vector<unsigned char>
ReceiveQuery(const std::string & ip, const unsigned long & port) {
  unique_ptr<Socket> connection;
  CreateSocket(true, ip, port, &connection);
  ListenParams params;
  params.receive_data_ = &ReceiveBigEndianInt64Bytes;
  connection->SetListenParams(params);
  set<ListenReturnCode> return_codes = connection->Listen();
  if (return_codes.size() != 1 ||
      *(return_codes.begin()) != ListenReturnCode::OK) {
    log_error("Failed to receive query.");
    exit(1);
  }

  vector<char> received_data;
  if (connection->GetReceivedBytes().size() != 1) {
    log_fatal("Unexpected number of Servers");
  }
  connection->SwapBuffer(0, &received_data);

  vector<unsigned char> q(received_data.size() - sizeof(uint64_t));
  memcpy(q.data(), received_data.data() + sizeof(uint64_t), q.size());

  return q;
}

void GetQuery(
    const std::string & ip,
    const unsigned long & port,
    const string & filename) {

  // Write Query to file.
  std::ofstream queryFile(filename.c_str());
  if (!queryFile.is_open()) {
    log_warn("Query file cannot be opened.");
    exit(1);
  }
}

int main(int argc, char * argv[]) {
  LOG_FILE = stderr;
  if (argc != 4) {
    log_warn("Usage: server_receive_query <ip> <port> <query.json>");
    exit(1);
  }

  // Parse connection info.
  const std::string ip = argv[1];
  const std::string port_str = argv[2];
  unsigned long port;
  if (!string_utils::Stoi(port_str, &port)) {
    log_warn("Usage: server_receive_query <ip> <port> <query.json>");
    exit(1);
  }

  // Receive Query from analyst.
  vector<unsigned char> query_bytes = ReceiveQuery(ip, port);

  // Write query to file.
  std::string qFile(argv[3]);
  file_reader_utils::WriteBinaryFile(
      qFile, query_bytes.size(), (char *)query_bytes.data());

  return 0;
}
