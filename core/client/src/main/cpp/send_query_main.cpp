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
#include <Util/char_casting_utils.h>
#include <Util/read_file_utils.h>
#include <Util/socket.h>
#include <Util/socket_utils.h>

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

// DEPRECATED: See comment below.
/*
void SendQuery(const safrn::Query& query, 
               const vector<pair<string, unsigned long>>& peers) {
  vector<unsigned char> q = query.Serialize();
  vector<unsigned char> to_send;
  ValueToByteString<uint64_t>(q.size(), &to_send);
  to_send.push_back(std::move(q));
  for (const pair<string, unsigned long>& peer : peers) {
    unique_ptr<Socket> s;
    CreateSocket(false, peer.first, peer.second, &s);
    const SendReturnCode send_status = s->SendData(to_send.data(), to_send.size());
    if (send_status != SendReturnCode::SUCCESS) {
      LOG_ERROR("Client failed to send query to peer at " + peer.first +
                ":" + Itoa(peer.second) + ". Error code: " +
                Itoa(static_cast<int>(send_status)));
    }
  }
}
*/

void SendQuery(
    vector<unsigned char> & query_bytes,
    const vector<pair<string, unsigned long>> & peers) {
  vector<unsigned char> to_send;
  ValueToByteString<uint64_t>(query_bytes.size(), &to_send);
  to_send.insert(
      to_send.end(),
      std::make_move_iterator(query_bytes.begin()),
      std::make_move_iterator(query_bytes.end()));
  for (const pair<string, unsigned long> & peer : peers) {
    unique_ptr<Socket> s;
    CreateSocket(false, peer.first, peer.second, &s);
    const SendReturnCode send_status =
        s->SendData(to_send.data(), to_send.size());
    if (send_status != SendReturnCode::SUCCESS) {
      log_error(
          "Client failed to send query to peer at %s:%lu. Error "
          "code:%d",
          peer.first.c_str(),
          peer.second,
          static_cast<int>(send_status));
    }
  }
}

int main(int argc, char * argv[]) {
  LOG_FILE = stderr;
  if (argc != 3) {
    log_warn("Usage: client_send_query <peers.json> <query.json>");
    exit(1);
  }

  // Parse connection info.
  std::ifstream peersFile(argv[1]);
  if (!peersFile.is_open()) {
    log_warn("Peers file cannot be opened.");
    exit(1);
  }
  const nlohmann::json peersJson = nlohmann::json::parse(peersFile);
  vector<pair<string, unsigned long>> peers;
  ParsePeers(peersJson, &peers);

  // DEPRECATED: Instead of parsing the json file into a Query object
  // and then calling Query.Serialize() to convert this to bytes to
  // send over a socket; just read query.json file as bytes and
  // send these directly.
  /*
  // Parse query.
  std::stringifstream queryFile(argv[2]);
  if (!queryFile.is_open()) {
    log_warn("Query file cannot be opened.");
    exit(1);
  }
  const nlohmann::json queryJson = nlohmann::json::parse(queryFile);
  safrn::Query query(queryJson);

  SendQuery(query, peers);
  */
  std::string qFile(argv[2]);
  uint64_t num_bytes_read = 0;
  char * read_bytes = nullptr;
  file_reader_utils::ReadBinaryFile(
      qFile, &num_bytes_read, &read_bytes);
  vector<unsigned char> query_bytes(num_bytes_read);
  memcpy(
      query_bytes.data(), (unsigned char *)read_bytes, num_bytes_read);
  SendQuery(query_bytes, peers);

  return 0;
}
