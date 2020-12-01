/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#include <recipient/RegressionReceiver.h>

#include <ff/logging.h>

namespace safrn {
namespace recipient {

double castToDouble(
    dataowner::LargeNum val,
    size_t num_bits_to_divide_by,
    dataowner::LargeNum modulus) {
  if (dataowner::LargeNum(2) * val > modulus) {
    return (
        -1 *
        castToDouble(modulus - val, num_bits_to_divide_by, modulus));
  }

  dataowner::LargeNum fullPowerOfTwo = dataowner::LargeNum(1)
      << (num_bits_to_divide_by);

  double integer_part =
      static_cast<double>(static_cast<uint64_t>(val / fullPowerOfTwo));

  dataowner::LargeNum remainder = val % fullPowerOfTwo;

  if (num_bits_to_divide_by <= 64) {
    double ret = static_cast<double>(static_cast<uint64_t>(remainder));
    ret /= pow(2.0, num_bits_to_divide_by);
    return integer_part + ret;
  } else {
    dataowner::LargeNum powerOfTwo = dataowner::LargeNum(1)
        << (num_bits_to_divide_by - 64);
    val /= powerOfTwo;
    double ret = static_cast<double>(static_cast<uint64_t>(val));
    ret /= pow(2.0, 64);
    return integer_part + ret;
  }
}

double convertBytesToDouble(
    std::vector<Boolean_t> const & bytes, const size_t numBytes) {
  log_debug(
      "Calling convertBytesToDouble with bytes.size() = %zu",
      bytes.size());
  double ret = 0.0;
  dataowner::LargeNum ret_shifted = 0;
  for (size_t i = numBytes; i != 0; i--) {
    log_debug("i = %zu", i);
    ret *= 256.0;
    ret_shifted *= 256;
    ret += bytes[i - 1];
    log_debug("bytes.at(i-1) = %hhu", bytes[i - 1]);
    ret_shifted += bytes[i - 1];
    log_debug("ret_shifted: %s", ff::mpc::dec(ret_shifted).c_str());
  }
  ret /= pow(2, 8 * numBytes);
  return ret;
}

std::vector<std::string> findColumnNames(
    std::vector<size_t> const & leftPayloads,
    std::vector<size_t> const & rightPayloads,
    size_t leftVert,
    size_t rightVert,
    size_t depVert,
    bool fitIntercept,
    StudyConfig const & scfg) {
  std::vector<size_t> const & first_payloads =
      (leftVert == depVert) ? rightPayloads : leftPayloads;
  std::vector<size_t> const & second_payloads =
      (leftVert == depVert) ? leftPayloads : rightPayloads;

  size_t first_vert = (leftVert == depVert) ? rightVert : leftVert;
  size_t second_vert = (leftVert == depVert) ? leftVert : rightVert;

  std::vector<std::string> ret;

  for (size_t i = 0; i < first_payloads.size(); i++) {
    ret.push_back(
        scfg.lexicon[first_vert].columns[first_payloads[i]]->name);
  }

  size_t second_len = second_payloads.size() - 1;
  for (size_t i = 0; i < second_len; i++) {
    ret.push_back(
        scfg.lexicon[second_vert].columns[second_payloads[i]]->name);
  }

  if (fitIntercept) {
    ret.push_back(std::string("<strong>Intercept</strong>"));
  }

  return ret;
}

void regressionPrettyPrint(
    std::vector<std::string> const & result_names,
    std::vector<double> const & results,
    double const error,
    double const rsquare,
    std::vector<double> const & standardErrorCoeffs,
    double const & F_p_value,
    std::vector<double> const & t_p_values,
    size_t const bytesInLookupTableCells) {
  log_debug(
      "result_names.size() %zu and standardErrorCoeffs.size() %zu",
      result_names.size(),
      standardErrorCoeffs.size());
  log_assert(result_names.size() == results.size());
  log_assert(standardErrorCoeffs.size() == results.size());
  bool yep = false;
  size_t result_num = 0;
  do {
#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
    int fd = _open(
        (std::string("regression-result-") +
         std::to_string(result_num) + ".html")
            .c_str(),
        _O_CREAT | _O_WRONLY | _O_EXCL,
        _S_IREAD | _S_IWRITE);
#else
    int fd = open(
        (std::string("regression-result-") +
         std::to_string(result_num) + ".html")
            .c_str(),
        O_CREAT | O_WRONLY | O_EXCL,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
    if (fd < 0) {
      log_debug("tried %zu", result_num);
      result_num++;
      yep = false;
    } else {
      yep = true;
      log_info(
          "Writing result to \"regression-result-%zu.html\"",
          result_num);
      FILE * file = fdopen(fd, "w");
      fprintf(
          file,
          "<!DOCTYPE html>\n<html>\n<body>\n<table>\n  <tr>\n    "
          "<th>Variable</th>\n    <th>Coefficient</th>\n    "
          "<th>StdErr</th>\n    <th>p-value (t-test)</th>  </tr>\n");

      for (size_t i = 0; i < results.size(); i++) {
        fprintf(
            file,
            "  <tr>\n    <td>%s</td>\n    <td>%lf</td>\n    "
            "<td>%lf</td>\n    <td>%1.9f</td>\n  </tr>\n",
            result_names[i].c_str(),
            results[i],
            sqrt(standardErrorCoeffs[i]),
            t_p_values[i]);
        log_info(
            "Coeff,stdErr,p-val(t-test)[%zu] := %lf, %lf, %1.9f",
            i,
            results[i],
            sqrt(standardErrorCoeffs[i]),
            t_p_values[i]);
      }

      fprintf(
          file,
          "</table>\n<table>\n  <tr>\n    <th>Root MSE</th>\n    "
          "<th>R Squared</th>\n    <th>p-value (F-test)</th>\n  </tr>");
      fprintf(
          file,
          "  <tr>\n    <td>%lf</td>\n    <td>%lf</td>\n    "
          "<td>%1.9f</td>\n  </tr>\n",
          sqrt(error),
          rsquare,
          F_p_value);
      log_info("Root MSE: %lf", sqrt(error));
      log_info("R Squared: %lf", rsquare);
      log_info("p-value (F-test): %1.9f", F_p_value);

      fprintf(file, "</table>\n</body>\n</html>\n");
      fclose(file);
    }
  } while (!yep);
}

void RegressionReceiver::init() {
  log_info(
      "start mod %s, end mod %s, key mod %s",
      ff::mpc::dec(this->info->startModulus).c_str(),
      ff::mpc::dec(this->info->endModulus).c_str(),
      ff::mpc::dec(this->info->keyModulus).c_str());
  this->getPeers().forEachDataowner(
      [this](Identity const & other) { this->numDataowners++; });
}

void RegressionReceiver::handleReceive(IncomingMessage & imsg) {
  log_debug("handleReceive");
  // sum/mod results until no more dataowners
  for (size_t i = 0; i < this->results.size(); i++) {
    dataowner::LargeNum res = 0;
    imsg.read<dataowner::LargeNum>(res);

    this->results[i] =
        ff::mpc::modAdd(this->results[i], res, this->info->endModulus);
  }

  dataowner::LargeNum err = 0;
  imsg.read<dataowner::LargeNum>(err);
  this->rootMSE =
      ff::mpc::modAdd(this->rootMSE, err, this->info->endModulus);

  dataowner::LargeNum rsq = 0;
  imsg.read<dataowner::LargeNum>(rsq);
  this->rsquare =
      ff::mpc::modAdd(this->rsquare, rsq, this->info->endModulus);

  // sum/mod results until no more dataowners
  for (size_t i = 0; i < this->results.size(); i++) {
    dataowner::LargeNum res = 0;
    imsg.read<dataowner::LargeNum>(res);

    this->standardErrorCoeffs[i] = ff::mpc::modAdd(
        this->standardErrorCoeffs[i], res, this->info->endModulus);
  }

  for (size_t i = 0; i < this->info->bytesInLookupTableCells; i++) {
    Boolean_t res;
    imsg.read<Boolean_t>(res);

    this->F_p_value[i] ^= res;
  }

  for (size_t i = 0; i < this->results.size(); i++) {
    for (size_t j = 0; j < this->info->bytesInLookupTableCells; j++) {
      Boolean_t res;
      imsg.read<Boolean_t>(res);

      this->t_p_values[i][j] ^= res;
    }
  }

  this->numDataowners--;
  if (this->numDataowners == 0) {
    std::vector<double> results_cast;
    for (dataowner::LargeNum num : this->results) {
      results_cast.push_back(castToDouble(
          num, this->bitsOfPrecision, this->info->endModulus));
    }
    std::vector<double> s_e_coeffs_cast;
    for (dataowner::LargeNum num : this->standardErrorCoeffs) {
      s_e_coeffs_cast.push_back(castToDouble(
          num, 4 * this->bitsOfPrecision, this->info->endModulus));
    }

    std::vector<double> t_p_values_converted;
    for (size_t i = 0; i < this->results.size(); i++) {
      /** Multiply by 2 to get left and right half of symmetric t-distribution */
      log_debug(
          "About to call convertBytesToDouble with "
          "t_p_values.at(%zu).size() = %zu",
          i,
          t_p_values[i].size());
      t_p_values_converted.push_back(
          2.0 *
          convertBytesToDouble(
              t_p_values[i], this->info->bytesInLookupTableCells));
    }

    regressionPrettyPrint(
        findColumnNames(
            this->leftPayloads,
            this->rightPayloads,
            this->leftVert,
            this->rightVert,
            this->info->verticalDV,
            this->info->fitIntercept,
            this->studyCfg),
        results_cast,
        castToDouble(
            this->rootMSE,
            5 * this->bitsOfPrecision,
            this->info->endModulus),
        castToDouble(
            this->rsquare,
            this->bitsOfPrecision,
            this->info->endModulus),
        s_e_coeffs_cast,
        convertBytesToDouble(
            F_p_value, this->info->bytesInLookupTableCells),
        t_p_values_converted,
        this->info->bytesInLookupTableCells);
    this->complete();
  }
}

void RegressionReceiver::handleComplete(Fronctocol &) {
  log_info("Regression Receiver unexpected handle complete");
  this->abort();
}

void RegressionReceiver::handlePromise(Fronctocol &) {
  log_info("Regression Receiver unexpected handle promise");
  this->abort();
}

std::string RegressionReceiver::name() {
  return std::string("Regression Receiver");
}

} // namespace recipient
} // namespace safrn
