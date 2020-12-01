/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <StartupUtils.h>

#include <Util/read_file_utils.h> // Paul's stuff, if we decide it's worth it
#include <Util/string_utils.h> // Paul's string stuff

/* Logging Config */
#include <ff/logging.h>

namespace safrn {

dataowner::LargeNum convertDoubleToLargeNum(
    double val, size_t bitsOfPrecision, dataowner::LargeNum modulus) {
  if (val < 0) {
    return modulus -
        convertDoubleToLargeNum(-1 * val, bitsOfPrecision, modulus);
  }

  log_debug("Double value %lf", val);

  /** to round the last bit instead of truncating */
  val += pow(0.5, 1 + bitsOfPrecision);

  dataowner::LargeNum ret = static_cast<dataowner::LargeNum>(
      static_cast<uint64_t>(floor(val)));

  val -= floor(val);

  log_debug("Double value %lf", val);
  log_debug("Return value %s", ff::mpc::dec(ret).c_str());

  for (size_t i = 0; i < bitsOfPrecision; i++) {
    ret *= 2;
    val *= 2;
    log_debug("Double value %lf", val);
    log_debug("Return value %s", ff::mpc::dec(ret).c_str());
    ret += static_cast<uint64_t>(floor(val));
    val -= floor(val);
    log_debug("Double value %lf", val);
    log_debug("Return value %s", ff::mpc::dec(ret).c_str());
  }
  return ret;
}

bool readCSV(
    std::string const & file,
    ff::mpc::ObservationList<dataowner::LargeNum> & oList,
    std::vector<size_t> const & keyCols,
    std::vector<size_t> const & payloadCols,
    size_t extra_precis_col,
    StudyConfig const & scfg,
    Identity const & id,
    dataowner::LargeNum const mod,
    size_t bitsOfPrecision) {
  log_debug("Calling readCSV");
  log_assert(id.role == ROLE_DATAOWNER);

  // Issue #220
  oList.numKeyCols = keyCols.size() + 1;
  oList.numArithmeticPayloadCols = payloadCols.size();
  oList.numXORPayloadCols = 0;

  oList.elements.clear();

  std::ifstream input_stream(file);
  if (!input_stream.is_open()) {
    log_error("Error opening file %s", file.c_str());
    return false;
  }

  std::string line;
  std::vector<std::string> split_line;

  std::vector<std::string> headlines;
  std::vector<size_t> key_places;
  std::vector<size_t> payload_places;

  size_t adjust_extra_precis_col = SIZE_MAX;

  if (getline(input_stream, line)) {
    file_reader_utils::RemoveWindowsTrailingCharacters(&line);
    line = string_utils::RemoveAllWhitespace(line);

    split_line.clear();
    string_utils::Split(line, ",", &split_line);

    key_places.resize(split_line.size(), SIZE_MAX);
    payload_places.resize(split_line.size(), SIZE_MAX);

    for (size_t k = 0; k < split_line.size(); k++) {
      std::string & split = split_line[k];
      bool col_found = false;
      for (size_t i = 0; i < scfg.lexicon[id.vertical].columns.size();
           i++) {
        std::unique_ptr<ColumnBase> const & col =
            scfg.lexicon[id.vertical].columns[i];
        if (split == col->name) {
          headlines.push_back(split);
          col_found = true;

          for (size_t j = 0; j < payloadCols.size(); j++) {
            if (payloadCols[j] == i) {
              payload_places[k] = j;
            }
            if (extra_precis_col == i) {
              adjust_extra_precis_col = k;
            }
          }

          for (size_t j = 0; j < keyCols.size(); j++) {
            if (keyCols[j] == i) {
              key_places[k] = j;
            }
          }
          break;
        }
      }
      if (!col_found) {
        log_error("Unknown Column \"%s\"", split.c_str());
        return false;
      }
    }
  } else {
    log_error("could not read header line");
    return false;
  }

  log_debug("about to read");

  while (getline(input_stream, line)) {
    log_debug("Hi, line is %s", line.c_str());
    file_reader_utils::RemoveWindowsTrailingCharacters(&line);
    line = string_utils::RemoveAllWhitespace(line);
    if (string_utils::HasPrefixString(line, "#")) {
      continue;
    }
    split_line.clear();
    string_utils::Split(line, ",", &split_line);

    if (split_line.size() != headlines.size()) {
      log_error("data line mismatches head line length");
      return false;
    }

    std::vector<dataowner::LargeNum> keys;
    keys.resize(keyCols.size() + 1);
    keys[keyCols.size()] = id.vertical;
    std::vector<dataowner::LargeNum> payloads;
    payloads.resize(payloadCols.size());

    for (size_t i = 0; i < split_line.size(); i++) {
      std::string & s = split_line[i];
      if (payload_places[i] != SIZE_MAX) {
        log_debug("Let's start here: %f", atof(s.c_str()));

        size_t adjust_bits_of_precis = bitsOfPrecision;
        if (i == adjust_extra_precis_col) {
          adjust_bits_of_precis *= 2;
        }

        dataowner::LargeNum ln = convertDoubleToLargeNum(
            atof(s.c_str()), adjust_bits_of_precis, mod);

        log_debug("place %zu", payload_places[i]);
        payloads[payload_places[i]] = ln;
      }
      if (key_places[i] != SIZE_MAX) {
        log_debug("Here instead");
        keys[key_places[i]] =
            static_cast<dataowner::LargeNum>(s.c_str());
      }
    }

    std::vector<Boolean_t> XOR_payloads; // empty
    oList.elements.emplace_back();
    ff::mpc::Observation<dataowner::LargeNum> & o =
        oList.elements.back();

    o.keyCols = std::move(keys);
    o.arithmeticPayloadCols = std::move(payloads);
    o.XORPayloadCols = std::move(XOR_payloads);
  }

  return true;
}

} // namespace safrn
