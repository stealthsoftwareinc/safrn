/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 */

#include <cerrno>
#include <cfloat> // For DBL_MIN, DBL_MAX
#include <climits> // For ULLONG_MAX, etc.
#include <cmath>
#include <cstring> // For strerror.
#include <iostream>
#include <limits>
#include <map>
#include <memory> // For unique_ptr.
#include <set>
#include <string>
#include <tuple> // For pair.
#include <typeinfo> // For typeid, bad_cast
#include <vector>

#include "char_casting_utils.h" // For ValueToByteString.
#include "constants.h" // For slice.
#include "data_structures.h"
#include "map_utils.h" // For FindOrInsert().

/* Logging Configuration */
#include <ff/logging.h>

using namespace map_utils;
using namespace string_utils;
using namespace std;

namespace math_utils {

namespace {

// The size of 'kGroupingSymbols'. Be sure to keep these in sync.
static const int kNumGroupingSymbols = 5;
static const pair<char, char> kGroupingSymbols[kNumGroupingSymbols] = {
    make_pair('(', ')'),
    make_pair('{', '}'),
    make_pair('[', ']'),
    // WARNING: It may be a little dangerous to include angle brackets
    // among the set of grouping symbols, as these symbols are also used
    // to denote GT and LT. Users of 'kGroupingSymbols' must keep this in
    // mind; i.e. it will be safe to treat these characters as grouping
    // symbols (as opposed to GT, LT) if the string in question begins
    // with '<' and ends with '>'.
    // UPDATE: We no longer include <> as potential grouping symbols, because
    // it is too hard to distinguish when these should be treated as
    // GT/LT, and when they should be treated as a grouping symbol.
    // For example, consider the expression:
    //   2 < y AND x > -2
    // This could be interpretted as:
    //   (2 < y) AND (x > -2)
    // or as:
    //   (2 * (y AND x)) - 2
    //make_pair('<', '>'),
    // Some uses below assume the absoulte value grouping is the last one
    // stored in kGroupingSymbols. If new symbols are added later, be sure
    // to keep these symbols last.
    make_pair('|', '|')};

// Searches the input string for the first occurence of any of the characters
// in 'split_chars'. As long as this character does not lie within a grouping
// (e.g. within a parentheses), then this splits the input string into two
// strings, and stores them in 'terms'.
template<typename T>
bool GetTerms(
    const bool abort_after_found,
    string & suffix,
    string & current_term,
    const vector<pair<T, int>> & split_chars,
    vector<pair<string, int>> * terms,
    bool * done) {
  if (suffix.empty()) {
    if (!current_term.empty()) {
      terms->push_back(make_pair(current_term, -1));
    }
    *done = true;
    return true;
  }

  size_t split_pos = string::npos;
  int split_index = -1;
  for (int i = 0; i < split_chars.size(); ++i) {
    size_t current_split_pos = suffix.find(split_chars[i].first);
    if (current_split_pos < split_pos) {
      split_index = i;
      split_pos = current_split_pos;
    }
  }

  // If no split characters were found, append the suffix to 'terms', and return.
  if (split_index == -1) {
    terms->push_back(make_pair(current_term + suffix, -1));
    *done = true;
    return true;
  }

  // We reached here, so a splitting character was found. However,
  // it is only appropriate to split terms around this character if it
  // does *not* lie within a grouping (e.g. parentheses). First check that.
  size_t min_left_grouping_symbol_pos = string::npos;
  int min_symbol_index = -1;
  for (int i = 0; i < kNumGroupingSymbols; ++i) {
    const pair<char, char> & grouping_symbols = kGroupingSymbols[i];
    size_t current_left_pos = suffix.find(grouping_symbols.first);
    if (current_left_pos < min_left_grouping_symbol_pos) {
      min_left_grouping_symbol_pos = current_left_pos;
      min_symbol_index = i;
    }
  }
  if (min_symbol_index >= 0 &&
      min_left_grouping_symbol_pos < split_pos) {
    // The found splitting symbol lies in a grouping (so we won't split around it).
    // Jump to the end of that grouping, adding eveything up to that point
    // to 'current_term', and then searching the suffix for the next potential
    // split symbol.
    const string after_left_grouping_symbol =
        suffix.substr(min_left_grouping_symbol_pos);
    size_t closing_pos;
    if (!GetClosingSymbol(
            after_left_grouping_symbol,
            kGroupingSymbols[min_symbol_index],
            &closing_pos)) {
      log_error(
          "Unable to GetClosingSymbol for:\n\t'%s'\nOn original "
          "input:\n\t'%s'",
          after_left_grouping_symbol.c_str(),
          suffix.c_str());
      return false;
    }
    // Add the grouping to 'current' term, and everything to the right of it
    // becomes 'suffix'.
    current_term += suffix.substr(
        0, min_left_grouping_symbol_pos + closing_pos + 1);
    suffix =
        suffix.substr(min_left_grouping_symbol_pos + closing_pos + 1);
    return true;
  }

  // The split symbol does *not* lie within a grouping, so go ahead and split
  // string around it into two terms.
  terms->push_back(make_pair(
      current_term + suffix.substr(0, split_pos), split_index));
  if (abort_after_found) {
    terms->push_back(make_pair(
        suffix.substr(split_pos + split_chars[split_index].second),
        -1));
    *done = true;
    return true;
  }
  suffix = suffix.substr(split_pos + split_chars[split_index].second);
  current_term = "";
  return true;
}

template<typename T>
bool GetTerms(
    const bool abort_after_found,
    const string & suffix,
    const string & term,
    const vector<pair<T, int>> & split_chars,
    vector<pair<string, int>> * terms) {
  // Make local copies, that can be modified (so that we don't need to
  // introduce new variables each time GetTerms is called within the loop below).
  string current_term = term;
  string current_suffix = suffix;

  bool done = false;
  while (!done) {
    if (!GetTerms<T>(
            abort_after_found,
            current_suffix,
            current_term,
            split_chars,
            terms,
            &done)) {
      return false;
    }
  }
  return true;
}

} // namespace

// Constructor for GenericValue whose underlying value is a Vector (of GenericValues).
GenericValue::GenericValue(const std::vector<GenericValue> & value) :
    GenericValue() {
  type_ = DataType::VECTOR;
  value_.reset(new VectorDataType(value));
}

DataType StringToDataType(const string & input) {
  if (input == "STRING8")
    return DataType::STRING8;
  if (input == "STRING16")
    return DataType::STRING16;
  if (input == "STRING24")
    return DataType::STRING24;
  if (input == "STRING32")
    return DataType::STRING32;
  if (input == "STRING64")
    return DataType::STRING64;
  if (input == "STRING128")
    return DataType::STRING128;
  if (input == "BOOL")
    return DataType::BOOL;
  if (input == "INT2")
    return DataType::INT2;
  if (input == "UINT2")
    return DataType::UINT2;
  if (input == "INT4")
    return DataType::INT4;
  if (input == "UINT4")
    return DataType::UINT4;
  if (input == "INT8")
    return DataType::INT8;
  if (input == "UINT8")
    return DataType::UINT8;
  if (input == "INT16")
    return DataType::INT16;
  if (input == "UINT16")
    return DataType::UINT16;
  if (input == "INT32")
    return DataType::INT32;
  if (input == "UINT32")
    return DataType::UINT32;
  if (input == "INT64")
    return DataType::INT64;
  if (input == "UINT64")
    return DataType::UINT64;
  if (input == "SLICE")
    return DataType::SLICE;
  if (input == "DOUBLE")
    return DataType::DOUBLE;
  if (input == "VEC")
    return DataType::VECTOR;
  if (input == "VECTOR")
    return DataType::VECTOR;
  return DataType::UNKNOWN;
}

string GetDataTypeString(const DataType type) {
  if (type == DataType::STRING8)
    return "STRING8";
  if (type == DataType::STRING16)
    return "STRING16";
  if (type == DataType::STRING24)
    return "STRING24";
  if (type == DataType::STRING32)
    return "STRING32";
  if (type == DataType::STRING64)
    return "STRING64";
  if (type == DataType::STRING128)
    return "STRING128";
  if (type == DataType::BOOL)
    return "BOOL";
  if (type == DataType::INT2)
    return "INT2";
  if (type == DataType::UINT2)
    return "UINT2";
  if (type == DataType::INT4)
    return "INT4";
  if (type == DataType::UINT4)
    return "UINT4";
  if (type == DataType::INT8)
    return "INT8";
  if (type == DataType::UINT8)
    return "UINT8";
  if (type == DataType::INT16)
    return "INT16";
  if (type == DataType::UINT16)
    return "UINT16";
  if (type == DataType::INT32)
    return "INT32";
  if (type == DataType::UINT32)
    return "UINT32";
  if (type == DataType::INT64)
    return "INT64";
  if (type == DataType::UINT64)
    return "UINT64";
  if (type == DataType::SLICE)
    return "SLICE";
  if (type == DataType::DOUBLE)
    return "DOUBLE";
  if (type == DataType::VECTOR)
    return "VEC";
  return "UNKNOWN";
}

bool IsStringDataType(const DataType type) {
  return (
      type == DataType::STRING8 || type == DataType::STRING16 ||
      type == DataType::STRING24 || type == DataType::STRING32 ||
      type == DataType::STRING64 || type == DataType::STRING128);
}

bool IsIntegerDataType(const DataType type) {
  return (
      type == DataType::BOOL || type == DataType::SLICE ||
      type == DataType::INT2 || type == DataType::UINT2 ||
      type == DataType::INT4 || type == DataType::UINT4 ||
      type == DataType::INT8 || type == DataType::UINT8 ||
      type == DataType::INT16 || type == DataType::UINT16 ||
      type == DataType::INT32 || type == DataType::UINT32 ||
      type == DataType::INT64 || type == DataType::UINT64);
}

bool IsNumericDataType(const DataType type) {
  return type == DataType::DOUBLE || IsIntegerDataType(type);
}

bool IsDataTypeTwosComplement(const DataType type) {
  return (
      type == DataType::INT2 || type == DataType::INT4 ||
      type == DataType::INT8 || type == DataType::INT16 ||
      type == DataType::INT32 || type == DataType::INT64);
}

bool IsSignedDataType(const DataType type) {
  return IsDataTypeTwosComplement(type) || type == DataType::DOUBLE;
}

bool GenericValue::Equals(const GenericValue & other) const {
  return (type_ == other.type_) && (*this == other);
}

bool GenericValue::operator==(const GenericValue & other) const {
  if (IsStringDataType(type_) != IsStringDataType(other.type_))
    return false;

  // Handle String DataTypes.
  if (IsStringDataType(type_)) {
    return GetValue<string>(*((StringDataType *)value_.get())) ==
        GetValue<string>(*((StringDataType *)other.value_.get()));
  }

  // Handle Vector DataTypes.
  if (type_ == DataType::VECTOR || other.type_ == DataType::VECTOR) {
    if (type_ != DataType::VECTOR || other.type_ != DataType::VECTOR)
      return false;
    const vector<GenericValue> & self_cast =
        ((const VectorDataType *)value_.get())->value_;
    const vector<GenericValue> & other_cast =
        ((const VectorDataType *)other.value_.get())->value_;
    const size_t n = self_cast.size();
    if (n != other_cast.size())
      return false;
    for (size_t i = 0; i < n; ++i) {
      if (self_cast[i] != other_cast[i])
        return false;
    }
    return true;
  }

  // Handle Double DataTypes.
  if (type_ == DataType::DOUBLE || other.type_ == DataType::DOUBLE) {
    if (type_ == DataType::DOUBLE) {
      if (other.type_ == DataType::DOUBLE) {
        // TODO(PHB): Consider using one of the FloatEq() functions instead of ==.
        return GetValue<double>(*((DoubleDataType *)value_.get())) ==
            GetValue<double>(*((DoubleDataType *)other.value_.get()));
      } else {
        const double self_value =
            GetValue<double>(*((DoubleDataType *)value_.get()));
        if (other.type_ == DataType::UINT64 ||
            other.type_ == DataType::SLICE) {
          uint64_t other_value;
          if (!GetUnsignedIntegerValue(other, &other_value)) {
            return false;
          }
          return (
              (uint64_t)self_value == other_value &&
              self_value == (double)other_value);
        } else {
          int64_t other_value;
          if (!GetSignedIntegerValue(other, &other_value)) {
            return false;
          }
          return (
              (int64_t)self_value == other_value &&
              self_value == (double)other_value);
        }
      }
    } else {
      const double other_value =
          GetValue<double>(*((DoubleDataType *)other.value_.get()));
      if (type_ == DataType::UINT64 || type_ == DataType::SLICE) {
        uint64_t self_value;
        if (!GetUnsignedIntegerValue(*this, &self_value)) {
          return false;
        }
        return (
            (uint64_t)other_value == self_value &&
            other_value == (double)self_value);
      } else {
        int64_t self_value;
        if (!GetSignedIntegerValue(*this, &self_value)) {
          return false;
        }
        return (
            (int64_t)other_value == self_value &&
            other_value == (double)self_value);
      }
    }
  }

  // Handle Integer DataTypes.
  if (type_ == DataType::UINT64 || type_ == DataType::SLICE ||
      other.type_ == DataType::UINT64 ||
      other.type_ == DataType::SLICE) {
    if (type_ == DataType::UINT64 || type_ == DataType::SLICE) {
      if (other.type_ == DataType::UINT64 ||
          other.type_ == DataType::SLICE) {
        uint64_t self_value, other_value;
        if (!GetUnsignedIntegerValue(*this, &self_value) ||
            !GetUnsignedIntegerValue(other, &other_value)) {
          return false;
        }
        return self_value == other_value;
      } else {
        uint64_t self_value;
        int64_t other_value;
        if (!GetUnsignedIntegerValue(*this, &self_value) ||
            !GetSignedIntegerValue(other, &other_value)) {
          return false;
        }
        return other_value >= 0 && self_value == other_value;
      }
    } else {
      uint64_t other_value;
      int64_t self_value;
      if (!GetSignedIntegerValue(*this, &self_value) ||
          !GetUnsignedIntegerValue(other, &other_value)) {
        return false;
      }
      return self_value >= 0 && self_value == other_value;
    }
  } else {
    // Neither DataType is UINT64, so we can safely cast both as int64_t.
    int64_t self_value, other_value;
    if (!GetSignedIntegerValue(*this, &self_value) ||
        !GetSignedIntegerValue(other, &other_value)) {
      return false;
    }
    return self_value == other_value;
  }

  // Code should never reach here.
  return false;
}

bool GenericValue::operator!=(const GenericValue & other) const {
  return !(*this == other);
}

bool GenericValue::operator<(const GenericValue & other) const {
  if (IsStringDataType(type_) != IsStringDataType(other.type_))
    return false;

  // Handle String DataTypes.
  if (IsStringDataType(type_)) {
    return GetValue<string>(*((StringDataType *)value_.get())) <
        GetValue<string>(*((StringDataType *)other.value_.get()));
  }

  // Handle Vector DataTypes.
  if (type_ == DataType::VECTOR || other.type_ == DataType::VECTOR) {
    if (type_ != DataType::VECTOR || other.type_ != DataType::VECTOR)
      return false;
    const vector<GenericValue> & self_cast =
        ((const VectorDataType *)value_.get())->value_;
    const vector<GenericValue> & other_cast =
        ((const VectorDataType *)other.value_.get())->value_;
    const size_t n = self_cast.size();
    if (n != other_cast.size())
      return n < other_cast.size();
    for (size_t i = 0; i < n; ++i) {
      if (self_cast[i] != other_cast[i])
        return self_cast[i] < other_cast[i];
    }
    return true;
  }

  // Handle Double DataTypes.
  if (type_ == DataType::DOUBLE || other.type_ == DataType::DOUBLE) {
    if (type_ == DataType::DOUBLE) {
      if (other.type_ == DataType::DOUBLE) {
        return GetValue<double>(*((DoubleDataType *)value_.get())) <
            GetValue<double>(*((DoubleDataType *)other.value_.get()));
      } else {
        const double self_value =
            GetValue<double>(*((DoubleDataType *)value_.get()));
        if (other.type_ == DataType::UINT64 ||
            other.type_ == DataType::SLICE) {
          uint64_t other_value;
          if (!GetUnsignedIntegerValue(other, &other_value)) {
            return false;
          }
          return self_value < other_value;
        } else {
          int64_t other_value;
          if (!GetSignedIntegerValue(other, &other_value)) {
            return false;
          }
          return self_value < other_value;
        }
      }
    } else {
      const double other_value =
          GetValue<double>(*((DoubleDataType *)other.value_.get()));
      if (type_ == DataType::UINT64 || type_ == DataType::SLICE) {
        uint64_t self_value;
        if (!GetUnsignedIntegerValue(*this, &self_value)) {
          return false;
        }
        return self_value < other_value;
      } else {
        int64_t self_value;
        if (!GetSignedIntegerValue(*this, &self_value)) {
          return false;
        }
        return self_value < other_value;
      }
    }
  }

  // Handle Integer DataTypes.
  if (type_ == DataType::UINT64 || type_ == DataType::SLICE ||
      other.type_ == DataType::UINT64 ||
      other.type_ == DataType::SLICE) {
    if (type_ == DataType::UINT64 || type_ == DataType::SLICE) {
      if (other.type_ == DataType::UINT64 ||
          other.type_ == DataType::SLICE) {
        uint64_t self_value, other_value;
        if (!GetUnsignedIntegerValue(*this, &self_value) ||
            !GetUnsignedIntegerValue(other, &other_value)) {
          return false;
        }
        return self_value < other_value;
      } else {
        uint64_t self_value;
        int64_t other_value;
        if (!GetUnsignedIntegerValue(*this, &self_value) ||
            !GetSignedIntegerValue(other, &other_value)) {
          return false;
        }
        return other_value >= 0 && self_value < other_value;
      }
    } else {
      uint64_t other_value;
      int64_t self_value;
      if (!GetSignedIntegerValue(*this, &self_value) ||
          !GetUnsignedIntegerValue(other, &other_value)) {
        return false;
      }
      return self_value < 0 || self_value < other_value;
    }
  } else {
    // Neither DataType is UINT64, so we can safely cast both as int64_t.
    int64_t self_value, other_value;
    if (!GetSignedIntegerValue(*this, &self_value) ||
        !GetSignedIntegerValue(other, &other_value)) {
      return false;
    }
    return self_value < other_value;
  }

  // Code should never reach here.
  return false;
}

bool GenericValue::operator<=(const GenericValue & other) const {
  return (*this == other || *this < other);
}

bool GenericValue::operator>(const GenericValue & other) const {
  return !(*this <= other);
}

bool GenericValue::operator>=(const GenericValue & other) const {
  return (*this == other || *this > other);
}

// TODO(PHB): These operations may be used over-and-over again for circuits,
// and so minor efficiency/optimizations may make a difference. Consider
// either writing specific code for each operator below, and/or cleaning-up
// MergeValuesViaOperator() so that it doesn't make extra checks and that
// the code flow is optimized for the most likely use-cases.
GenericValue & GenericValue::operator+=(const GenericValue & other) {
  GenericValue temp;
  if (!MergeValuesViaOperator(
          MathOperation::ADD, *this, other, &temp)) {
    log_fatal("Unable to add generic values.");
  }
  *this = temp;
  return *this;
}

GenericValue & GenericValue::operator-=(const GenericValue & other) {
  GenericValue temp;
  if (!MergeValuesViaOperator(
          MathOperation::SUB, *this, other, &temp)) {
    log_fatal("Unable to subtract generic values.");
  }
  *this = temp;
  return *this;
}

GenericValue & GenericValue::operator*=(const GenericValue & other) {
  GenericValue temp;
  if (!MergeValuesViaOperator(
          MathOperation::MULT, *this, other, &temp)) {
    log_fatal("Unable to multiply generic values.");
  }
  *this = temp;
  return *this;
}

GenericValue & GenericValue::operator/=(const GenericValue & other) {
  GenericValue temp;
  if (!MergeValuesViaOperator(
          MathOperation::DIV, *this, other, &temp)) {
    log_fatal("Unable to divide generic values.");
  }
  *this = temp;
  return *this;
}

GenericValue & GenericValue::operator%=(const GenericValue & other) {
  GenericValue temp;
  if (!MergeValuesViaOperator(
          MathOperation::MODULUS, *this, other, &temp)) {
    log_fatal("Unable to find modulus of generic values.");
  }
  *this = temp;
  return *this;
}

BooleanOperation GetBooleanOperation(const string & input) {
  if (input == "UNKNOWN")
    return BooleanOperation::UNKNOWN;
  if (input == "IDENTITY")
    return BooleanOperation::IDENTITY;
  if (input == "NOT")
    return BooleanOperation::NOT;
  if (input == "OR")
    return BooleanOperation::OR;
  if (input == "NOR")
    return BooleanOperation::NOR;
  if (input == "XOR")
    return BooleanOperation::XOR;
  if (input == "AND")
    return BooleanOperation::AND;
  if (input == "NAND")
    return BooleanOperation::NAND;
  if (input == "EQ")
    return BooleanOperation::EQ;
  if (input == "GT")
    return BooleanOperation::GT;
  if (input == "GTE")
    return BooleanOperation::GTE;
  if (input == "LT")
    return BooleanOperation::LT;
  if (input == "LTE")
    return BooleanOperation::LTE;

  log_error(
      "Unable to GetBooleanOperation() for input: '%s'.",
      input.c_str());
  return BooleanOperation::UNKNOWN;
}

string GetBooleanOperationString(const BooleanOperation type) {
  if (type == BooleanOperation::IDENTITY)
    return "IDENTITY";
  if (type == BooleanOperation::NOT)
    return "NOT";
  if (type == BooleanOperation::OR)
    return "OR";
  if (type == BooleanOperation::NOR)
    return "NOR";
  if (type == BooleanOperation::XOR)
    return "XOR";
  if (type == BooleanOperation::AND)
    return "AND";
  if (type == BooleanOperation::NAND)
    return "NAND";
  if (type == BooleanOperation::EQ)
    return "EQ";
  if (type == BooleanOperation::GT)
    return "GT";
  if (type == BooleanOperation::GTE)
    return "GTE";
  if (type == BooleanOperation::LT)
    return "LT";
  if (type == BooleanOperation::LTE)
    return "LTE";
  return "UNKNOWN";
}

string GetOpString(const MathOperation op) {
  switch (op) {
    case MathOperation::ADD:
      return " + ";
    case MathOperation::SUB:
      return " - ";
    case MathOperation::MULT:
      return (" * ");
    case MathOperation::DIV:
      return " / ";
    case MathOperation::POW:
      return "^";
    case MathOperation::MODULUS:
      return "%";
    case MathOperation::EQ:
      return " == ";
    case MathOperation::NEQ:
      return " != ";
    case MathOperation::FLOAT_EQ:
      return " ~= ";
    case MathOperation::GT:
      return " > ";
    case MathOperation::GTE:
      return " >= ";
    case MathOperation::LT:
      return " < ";
    case MathOperation::LTE:
      return " <= ";
    case MathOperation::INC_GAMMA_FN:
      return "IncGamma";
    case MathOperation::REG_INC_GAMMA_FN:
      return "RegIncGamma";
    default:
      cout << "Unknown operation, or unexpected self-operation: "
           << static_cast<int>(op) << "\n";
  }
  return "";
}

string GetOpString(const MathOperation op, const string & argument) {
  if (op == MathOperation::FACTORIAL)
    return "(" + argument + ")!";
  else if (op == MathOperation::ABS)
    return "|" + argument + "|";
  else if (op == MathOperation::EXP)
    return "exp(" + argument + ")";
  else if (op == MathOperation::LOG)
    return "log(" + argument + ")";
  else if (op == MathOperation::SQRT)
    return "sqrt(" + argument + ")";
  else if (op == MathOperation::SIN)
    return "sin(" + argument + ")";
  else if (op == MathOperation::COS)
    return "cos(" + argument + ")";
  else if (op == MathOperation::TAN)
    return "tan(" + argument + ")";
  else if (op == MathOperation::INDICATOR)
    return "I_(" + argument + ")";
  else if (op == MathOperation::PHI_FN)
    return "Phi(" + argument + ")";
  else if (op == MathOperation::GAMMA_FN)
    return "Gamma(" + argument + ")";
  else
    return "";
}

string GetOpString(const BooleanOperation op) {
  switch (op) {
    case BooleanOperation::IDENTITY:
      return " IDENTITY ";
    case BooleanOperation::OR:
      return " OR ";
    case BooleanOperation::NOR:
      return " NOR ";
    case BooleanOperation::XOR:
      return " XOR ";
    case BooleanOperation::AND:
      return " AND ";
    case BooleanOperation::NAND:
      return " NAND ";
    case BooleanOperation::EQ:
      return " EQ ";
    case BooleanOperation::GT:
      return " GT ";
    case BooleanOperation::GTE:
      return " GTE ";
    case BooleanOperation::LT:
      return " LT ";
    case BooleanOperation::LTE:
      return " LTE ";
    case BooleanOperation::NOT:
      return " NOT ";
    default:
      log_fatal(
          "Unknown operation, or unexpected self-operation: %s",
          Itoa(static_cast<int>(op)).c_str());
  }
  return "";
}

string GetOpString(const ComparisonOperation op) {
  switch (op) {
    case ComparisonOperation::COMP_EQ:
      return " == ";
    case ComparisonOperation::COMP_NEQ:
      return " != ";
    case ComparisonOperation::COMP_GT:
      return " > ";
    case ComparisonOperation::COMP_GTE:
      return " >= ";
    case ComparisonOperation::COMP_LT:
      return " < ";
    case ComparisonOperation::COMP_LTE:
      return " <= ";
    default:
      log_fatal(
          "Unknown operation, or unexpected self-operation: %s",
          Itoa(static_cast<int>(op)).c_str());
  }
  return "";
}

string GetOpString(const ArithmeticOperation op) {
  switch (op) {
    case ArithmeticOperation::ADD:
      return " + ";
    case ArithmeticOperation::SUB:
      return " - ";
    case ArithmeticOperation::MULT:
      return (" * ");
    case ArithmeticOperation::DIV:
      return " / ";
    case ArithmeticOperation::POW:
      return "^";
    case ArithmeticOperation::ABS:
      return "abs";
    case ArithmeticOperation::FLIP_SIGN:
      return "-";
    case ArithmeticOperation::FACTORIAL:
      return "!";
    case ArithmeticOperation::SQRT:
      return "sqrt";
    case ArithmeticOperation::MIN:
      return "min";
    case ArithmeticOperation::MAX:
      return "max";
    case ArithmeticOperation::ARGMIN:
      return "argmin";
    case ArithmeticOperation::ARGMAX:
      return "argmax";
    case ArithmeticOperation::ARGMIN_INTERNAL:
      return "argmin_internal";
    case ArithmeticOperation::ARGMAX_INTERNAL:
      return "argmax_internal";
    case ArithmeticOperation::VEC:
      return "vec";
    case ArithmeticOperation::INNER_PRODUCT:
      return "inner_product";
    default:
      log_fatal(
          "Unknown operation, or unexpected self-operation: %s",
          Itoa(static_cast<int>(op)).c_str());
  }
  return "";
}

string GetOpString(const CircuitOperation op) {
  switch (op) {
    case CircuitOperation::UNKNOWN: {
      return "UNKNOWN";
    }
    case CircuitOperation::IDENTITY: {
      return "ID";
    }
    case CircuitOperation::NOT: {
      return "NOT";
    }
    case CircuitOperation::AND: {
      return "AND";
    }
    case CircuitOperation::NAND: {
      return "NAND";
    }
    case CircuitOperation::OR: {
      return "OR";
    }
    case CircuitOperation::NOR: {
      return "NOR";
    }
    case CircuitOperation::XOR: {
      return "XOR";
    }
    case CircuitOperation::EQ: {
      return "EQ";
    }
    case CircuitOperation::GT: {
      return "GT";
    }
    case CircuitOperation::GTE: {
      return "GTE";
    }
    case CircuitOperation::LT: {
      return "LT";
    }
    case CircuitOperation::LTE: {
      return "LTE";
    }
    case CircuitOperation::SELF: {
      return "ID";
    }
    case CircuitOperation::ABS: {
      return "Abs";
    }
    case CircuitOperation::FLIP_SIGN: {
      return "FLIP_SIGN";
    }
    case CircuitOperation::FACTORIAL: {
      return "FACTORIAL";
    }
    case CircuitOperation::SQRT: {
      return "SQRT";
    }
    case CircuitOperation::ADD: {
      return "ADD";
    }
    case CircuitOperation::SUB: {
      return "SUB";
    }
    case CircuitOperation::MULT: {
      return "MULT";
    }
    case CircuitOperation::DIV: {
      return "DIV";
    }
    case CircuitOperation::POW: {
      return "POW";
    }
    case CircuitOperation::ARGMIN: {
      return "ARGMIN";
    }
    case CircuitOperation::ARGMAX: {
      return "ARGMAX";
    }
    case CircuitOperation::MIN: {
      return "MIN";
    }
    case CircuitOperation::MAX: {
      return "MAX";
    }
    case CircuitOperation::INNER_PRODUCT: {
      return "INNER_PRODUCT";
    }
    default: {
      log_error(
          "Unsupported operation %s",
          Itoa(static_cast<int>(op)).c_str());
      return Itoa(static_cast<int>(op));
      ;
    }
  }

  return "UNKNOWN";
}

string GetOpString(const OperationHolder op) {
  if (op.type_ == OperationType::MATH)
    return GetOpString(op.math_op_);
  if (op.type_ == OperationType::BOOLEAN)
    return GetOpString(op.gate_op_);
  if (op.type_ == OperationType::ARITHMETIC)
    return GetOpString(op.arithmetic_op_);
  if (op.type_ == OperationType::COMPARISON)
    return GetOpString(op.comparison_op_);
  return "";
}

string GetOpString(const BooleanOperation op, const string & argument) {
  if (op == BooleanOperation::IDENTITY)
    return argument;
  else if (op == BooleanOperation::NOT)
    return "!(" + argument + ")";

  log_fatal(
      "Unknown operation, or unexpected self-operation: %s",
      Itoa(static_cast<int>(op)).c_str());
}

string
GetOpString(const ArithmeticOperation op, const string & argument) {
  if (op == ArithmeticOperation::ABS)
    return "|" + argument + "|";
  else if (op == ArithmeticOperation::FLIP_SIGN)
    return "-(" + argument + ")";
  else if (op == ArithmeticOperation::FACTORIAL)
    return "(" + argument + ")!";
  else if (op == ArithmeticOperation::SQRT)
    return "sqrt(" + argument + ")";
  log_fatal(
      "Unknown operation, or unexpected self-operation: %s",
      Itoa(static_cast<int>(op)).c_str());
}

bool IsDataTypeSubType(
    const DataType subtype, const DataType parent_type) {
  const int expected_num_bits = GetValueNumBits(parent_type);
  const int min_num_bits = GetValueNumBits(subtype);
  if (min_num_bits > expected_num_bits) {
    return false;
  }
  if (!IsDataTypeTwosComplement(parent_type) &&
      IsDataTypeTwosComplement(subtype)) {
    return false;
  }
  return true;
}

bool GetBit(const uint64_t & i, const GenericValue & input) {
  switch (input.type_) {
    case DataType::STRING8: {
      if (i >= CHAR_BIT * 8) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const string & value =
          GetValue<string>(*((StringDataType *)input.value_.get()));
      const size_t num_bytes = value.length();
      if (i >= num_bytes * CHAR_BIT)
        return false;
      char relevant_byte = value.at(num_bytes - 1 - i / CHAR_BIT);
      return (relevant_byte >> (i % CHAR_BIT)) & 1;
    }
    case DataType::STRING16: {
      if (i >= CHAR_BIT * 16) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const string & value =
          GetValue<string>(*((StringDataType *)input.value_.get()));
      const size_t num_bytes = value.length();
      if (i >= num_bytes * CHAR_BIT)
        return false;
      char relevant_byte = value.at(num_bytes - 1 - i / CHAR_BIT);
      return (relevant_byte >> (i % CHAR_BIT)) & 1;
    }
    case DataType::STRING24: {
      if (i >= CHAR_BIT * 24) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const string & value =
          GetValue<string>(*((StringDataType *)input.value_.get()));
      const size_t num_bytes = value.length();
      if (i >= num_bytes * CHAR_BIT)
        return false;
      char relevant_byte = value.at(num_bytes - 1 - i / CHAR_BIT);
      return (relevant_byte >> (i % CHAR_BIT)) & 1;
    }
    case DataType::STRING32: {
      if (i >= CHAR_BIT * 32) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const string & value =
          GetValue<string>(*((StringDataType *)input.value_.get()));
      const size_t num_bytes = value.length();
      if (i >= num_bytes * CHAR_BIT)
        return false;
      char relevant_byte = value.at(num_bytes - 1 - i / CHAR_BIT);
      return (relevant_byte >> (i % CHAR_BIT)) & 1;
    }
    case DataType::STRING64: {
      if (i >= CHAR_BIT * 64) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const string & value =
          GetValue<string>(*((StringDataType *)input.value_.get()));
      const size_t num_bytes = value.length();
      if (i >= num_bytes * CHAR_BIT)
        return false;
      char relevant_byte = value.at(num_bytes - 1 - i / CHAR_BIT);
      return (relevant_byte >> (i % CHAR_BIT)) & 1;
    }
    case DataType::STRING128: {
      if (i >= CHAR_BIT * 128) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const string & value =
          GetValue<string>(*((StringDataType *)input.value_.get()));
      const size_t num_bytes = value.length();
      if (i >= num_bytes * CHAR_BIT)
        return false;
      char relevant_byte = value.at(num_bytes - 1 - i / CHAR_BIT);
      return (relevant_byte >> (i % CHAR_BIT)) & 1;
    }
    case DataType::BOOL: {
      if (i != 0) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      return GetValue<bool>(*((BoolDataType *)input.value_.get()));
    }
    case DataType::INT2: {
      if (i >= 2) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const char value =
          GetValue<char>(*((Int2DataType *)input.value_.get()));
      if (i == 1)
        return value < 0;
      return value == -1 || value == 1;
    }
    case DataType::UINT2: {
      if (i >= 2) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const unsigned char value = GetValue<unsigned char>(
          *((Uint2DataType *)input.value_.get()));
      return (value >> i) & 1;
    }
    case DataType::INT4: {
      if (i >= 4) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const char value =
          GetValue<char>(*((Int4DataType *)input.value_.get()));
      if (i == 3)
        return value < 0;
      if (value >= 0)
        return (value >> i) & 1;
      // Negative values x in 2's complement have the same bits as 2^n + x.
      unsigned char as_positive = (((unsigned char)1) << 3) + value;
      return (as_positive >> i) & 1;
    }
    case DataType::UINT4: {
      if (i >= 4) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const unsigned char value = GetValue<unsigned char>(
          *((Uint4DataType *)input.value_.get()));
      return (value >> i) & 1;
    }
    case DataType::INT8: {
      if (i >= CHAR_BIT) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const char value =
          GetValue<char>(*((Int8DataType *)input.value_.get()));
      if (i == (CHAR_BIT - 1))
        return value < 0;
      if (value >= 0)
        return (value >> i) & 1;
      // Negative values x in 2's complement have the same bits as 2^n + x.
      unsigned char as_positive =
          (((unsigned char)1) << (CHAR_BIT - 1)) + value;
      return (as_positive >> i) & 1;
    }
    case DataType::UINT8: {
      if (i >= CHAR_BIT) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const unsigned char value = GetValue<unsigned char>(
          *((Uint8DataType *)input.value_.get()));
      return (value >> i) & 1;
    }
    case DataType::INT16: {
      if (i >= CHAR_BIT * sizeof(short)) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const short value =
          GetValue<short>(*((Int16DataType *)input.value_.get()));
      if (i == (CHAR_BIT * sizeof(short) - 1))
        return value < 0;
      if (value >= 0)
        return (value >> i) & 1;
      // Negative values x in 2's complement have the same bits as 2^n + x.
      unsigned short as_positive =
          (((unsigned short)1) << (sizeof(short) * CHAR_BIT - 1)) +
          value;
      return (as_positive >> i) & 1;
    }
    case DataType::UINT16: {
      if (i >= CHAR_BIT * sizeof(unsigned short)) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const unsigned short value = GetValue<unsigned short>(
          *((Uint16DataType *)input.value_.get()));
      return (value >> i) & 1;
    }
    case DataType::INT32: {
      if (i >= CHAR_BIT * sizeof(int32_t)) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const int32_t value =
          GetValue<int32_t>(*((Int32DataType *)input.value_.get()));
      if (i == (CHAR_BIT * sizeof(int32_t) - 1))
        return value < 0;
      if (value >= 0)
        return (value >> i) & 1;
      // Negative values x in 2's complement have the same bits as 2^n + x.
      uint32_t as_positive =
          (((uint32_t)1) << (sizeof(uint32_t) * CHAR_BIT - 1)) + value;
      return (as_positive >> i) & 1;
    }
    case DataType::UINT32: {
      if (i >= CHAR_BIT * sizeof(uint32_t)) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const uint32_t value =
          GetValue<uint32_t>(*((Uint32DataType *)input.value_.get()));
      return (value >> i) & 1;
    }
    case DataType::INT64: {
      if (i >= CHAR_BIT * sizeof(int64_t)) {
        log_fatal(
            "Bit index %s too high for DataType %s",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const int64_t value =
          GetValue<int64_t>(*((Int64DataType *)input.value_.get()));
      if (i == (CHAR_BIT * sizeof(int64_t) - 1))
        return value < 0;
      if (value >= 0)
        return (value >> i) & 1;
      // Negative values x in 2's complement have the same bits as 2^n + x.
      uint64_t as_positive =
          (((uint64_t)1) << (sizeof(uint64_t) * CHAR_BIT - 1)) + value;
      return (as_positive >> i) & 1;
    }
    case DataType::UINT64: {
      if (i >= CHAR_BIT * sizeof(uint64_t)) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const uint64_t value =
          GetValue<uint64_t>(*((Uint64DataType *)input.value_.get()));
      return (value >> i) & 1;
    }
    case DataType::SLICE: {
      if (i >= CHAR_BIT * sizeof(slice)) {
        log_fatal(
            "Bit index %s too high for DataType %s.",
            Itoa(i).c_str(),
            GetDataTypeString(input.type_).c_str());
      }
      const slice value =
          GetValue<slice>(*((SliceDataType *)input.value_.get()));
      return (value >> i) & 1;
    }
    case DataType::DOUBLE: {
      // TODO(PHB): Determine how to cast a double as a binary string.
      log_fatal(
          "Unsupported DataType: %s",
          Itoa(static_cast<int>(input.type_)).c_str());
    }
    case DataType::VECTOR: {
      const vector<GenericValue> & input_cast =
          ((const VectorDataType *)input.value_.get())->value_;
      uint64_t current_bit = 0;
      for (size_t j = 0; j < input_cast.size(); ++j) {
        const GenericValue & input_j = input_cast[j];
        const uint64_t input_j_bits = GetValueNumBits(input_j);
        current_bit += input_j_bits;
        if (i >= current_bit)
          continue;
        current_bit -= input_j_bits;
        const uint64_t bit_to_select = i - current_bit;
        return GetBit(bit_to_select, input_j);
      }
      return false;
    }
    default:
      log_fatal(
          "Unsupported DataType: %s",
          Itoa(static_cast<int>(input.type_)).c_str());
  }

  // Code should never reach here.
  return false;
}

vector<unsigned char>
GetTwosComplementByteString(const GenericValue & input) {
  switch (input.type_) {
    case DataType::STRING8: {
      const string & value =
          GetValue<string>(*((StringDataType *)input.value_.get()));
      const size_t length = value.length();
      if (length > 8)
        log_fatal("length too big");
      vector<unsigned char> to_return(8, 0);
      const size_t offset = 8 - length;
      for (size_t i = 0; i < length; ++i) {
        to_return[offset + i] = value.at(i);
      }
      return to_return;
    }
    case DataType::STRING16: {
      const string & value =
          GetValue<string>(*((StringDataType *)input.value_.get()));
      const size_t length = value.length();
      if (length > 16)
        log_fatal("length too big");
      vector<unsigned char> to_return(16, 0);
      const size_t offset = 16 - length;
      for (size_t i = 0; i < length; ++i) {
        to_return[offset + i] = value.at(i);
      }
      return to_return;
    }
    case DataType::STRING24: {
      const string & value =
          GetValue<string>(*((StringDataType *)input.value_.get()));
      const size_t length = value.length();
      if (length > 24)
        log_fatal("length too big");
      vector<unsigned char> to_return(24, 0);
      const size_t offset = 24 - length;
      for (size_t i = 0; i < length; ++i) {
        to_return[offset + i] = value.at(i);
      }
      return to_return;
    }
    case DataType::STRING32: {
      const string & value =
          GetValue<string>(*((StringDataType *)input.value_.get()));
      const size_t length = value.length();
      if (length > 32)
        log_fatal("length too big");
      vector<unsigned char> to_return(32, 0);
      const size_t offset = 32 - length;
      for (size_t i = 0; i < length; ++i) {
        to_return[offset + i] = value.at(i);
      }
      return to_return;
    }
    case DataType::STRING64: {
      const string & value =
          GetValue<string>(*((StringDataType *)input.value_.get()));
      const size_t length = value.length();
      if (length > 64)
        log_fatal("length too big");
      vector<unsigned char> to_return(64, 0);
      const size_t offset = 64 - length;
      for (size_t i = 0; i < length; ++i) {
        to_return[offset + i] = value.at(i);
      }
      return to_return;
    }
    case DataType::STRING128: {
      const string & value =
          GetValue<string>(*((StringDataType *)input.value_.get()));
      const size_t length = value.length();
      if (length > 128)
        log_fatal("length too big");
      vector<unsigned char> to_return(128, 0);
      const size_t offset = 128 - length;
      for (size_t i = 0; i < length; ++i) {
        to_return[offset + i] = value.at(i);
      }
      return to_return;
    }
    case DataType::BOOL: {
      const bool value =
          GetValue<bool>(*((BoolDataType *)input.value_.get()));
      vector<unsigned char> to_return(1, (value ? 1 : 0));
      return to_return;
    }
    case DataType::INT2: {
      vector<unsigned char> to_return(1, 0);
      const char value =
          GetValue<char>(*((Int2DataType *)input.value_.get()));
      if (value == -2) {
        to_return[0] |= (unsigned char)1 << 1;
      } else if (value == -1) {
        to_return[0] |= (unsigned char)1;
        to_return[0] |= (unsigned char)1 << 1;
        // Nothing to do for int2_ == 0 case.
      } else if (value == 1) {
        to_return[0] |= (unsigned char)1;
      } else if (value != 0) {
        log_fatal("Invalid value for INT2: %s", Itoa(value).c_str());
      }
      return to_return;
    }
    case DataType::UINT2: {
      vector<unsigned char> to_return;
      const unsigned char value = GetValue<unsigned char>(
          *((Uint2DataType *)input.value_.get()));
      if (!ValueToByteString<unsigned char>(value, &to_return)) {
        log_fatal("length too big");
      }
      return to_return;
    }
    case DataType::INT4: {
      vector<unsigned char> to_return(1, 0);
      const char value =
          GetValue<char>(*((Int4DataType *)input.value_.get()));
      if (value == -8) {
        to_return[0] |= (unsigned char)1 << 3;
      } else if (value == -7) {
        to_return[0] |= (unsigned char)1;
        to_return[0] |= (unsigned char)1 << 3;
      } else if (value == -6) {
        to_return[0] |= (unsigned char)1 << 1;
        to_return[0] |= (unsigned char)1 << 3;
      } else if (value == -5) {
        to_return[0] |= (unsigned char)1;
        to_return[0] |= (unsigned char)1 << 1;
        to_return[0] |= (unsigned char)1 << 3;
      } else if (value == -4) {
        to_return[0] |= (unsigned char)1 << 2;
        to_return[0] |= (unsigned char)1 << 3;
      } else if (value == -3) {
        to_return[0] |= (unsigned char)1;
        to_return[0] |= (unsigned char)1 << 2;
        to_return[0] |= (unsigned char)1 << 3;
      } else if (value == -2) {
        to_return[0] |= (unsigned char)1 << 1;
        to_return[0] |= (unsigned char)1 << 2;
        to_return[0] |= (unsigned char)1 << 3;
      } else if (value == -1) {
        to_return[0] |= (unsigned char)1;
        to_return[0] |= (unsigned char)1 << 1;
        to_return[0] |= (unsigned char)1 << 2;
        to_return[0] |= (unsigned char)1 << 3;
        // Nothing to do for int4_ == 0 case.
      } else if (value == 1) {
        to_return[0] |= (unsigned char)1;
      } else if (value == 2) {
        to_return[0] |= (unsigned char)1 << 1;
      } else if (value == 3) {
        to_return[0] |= (unsigned char)1;
        to_return[0] |= (unsigned char)1 << 1;
      } else if (value == 4) {
        to_return[0] |= (unsigned char)1 << 2;
      } else if (value == 5) {
        to_return[0] |= (unsigned char)1;
        to_return[0] |= (unsigned char)1 << 2;
      } else if (value == 6) {
        to_return[0] |= (unsigned char)1 << 1;
        to_return[0] |= (unsigned char)1 << 2;
      } else if (value == 7) {
        to_return[0] |= (unsigned char)1;
        to_return[0] |= (unsigned char)1 << 1;
        to_return[0] |= (unsigned char)1 << 2;
      } else if (value != 0) {
        log_fatal("Invalid value for INT4: %s", Itoa(value).c_str());
      }
      return to_return;
    }
    case DataType::UINT4: {
      vector<unsigned char> to_return;
      const unsigned char value = GetValue<unsigned char>(
          *((Uint4DataType *)input.value_.get()));
      if (!ValueToByteString<unsigned char>(value, &to_return)) {
        log_fatal("length too big");
      }
      return to_return;
    }
    case DataType::INT8: {
      vector<unsigned char> to_return;
      const char value =
          GetValue<char>(*((Int8DataType *)input.value_.get()));
      const bool is_negative = value < 0;
      const unsigned char trailing_bits_value =
          is_negative ? (value - SCHAR_MIN) : value;
      if (!ValueToByteString<char>(trailing_bits_value, &to_return)) {
        log_fatal("length too big");
      }
      if (is_negative) {
        to_return[0] |= (unsigned char)1 << (CHAR_BIT - 1);
      }
      return to_return;
    }
    case DataType::UINT8: {
      vector<unsigned char> to_return;
      const unsigned char value = GetValue<unsigned char>(
          *((Uint8DataType *)input.value_.get()));
      if (!ValueToByteString<unsigned char>(value, &to_return)) {
        log_fatal("length too big");
      }
      return to_return;
    }
    case DataType::INT16: {
      vector<unsigned char> to_return;
      const short value =
          GetValue<short>(*((Int16DataType *)input.value_.get()));
      const bool is_negative = value < 0;
      const unsigned short trailing_bits_value =
          is_negative ? (value - SHRT_MIN) : value;
      if (!ValueToByteString<short>(trailing_bits_value, &to_return)) {
        log_fatal("length too big");
      }
      if (is_negative) {
        to_return[0] |= (unsigned short)1 << (CHAR_BIT - 1);
      }
      return to_return;
    }
    case DataType::UINT16: {
      vector<unsigned char> to_return;
      const unsigned short value = GetValue<unsigned short>(
          *((Uint16DataType *)input.value_.get()));
      if (!ValueToByteString<unsigned short>(value, &to_return)) {
        log_fatal("length too big");
      }
      return to_return;
    }
    case DataType::INT32: {
      vector<unsigned char> to_return;
      const int32_t value =
          GetValue<int32_t>(*((Int32DataType *)input.value_.get()));
      const bool is_negative = value < 0;
      const uint32_t trailing_bits_value =
          is_negative ? (value - LONG_MIN) : value;
      if (!ValueToByteString<int32_t>(
              trailing_bits_value, &to_return)) {
        log_fatal("length too big");
      }
      if (is_negative) {
        to_return[0] |= (uint32_t)1 << (CHAR_BIT - 1);
      }
      return to_return;
    }
    case DataType::UINT32: {
      vector<unsigned char> to_return;
      const uint32_t value =
          GetValue<uint32_t>(*((Uint32DataType *)input.value_.get()));
      if (!ValueToByteString<uint32_t>(value, &to_return)) {
        log_fatal("length too big");
      }
      return to_return;
    }
    case DataType::INT64: {
      vector<unsigned char> to_return;
      const int64_t value =
          GetValue<int64_t>(*((Int64DataType *)input.value_.get()));
      const bool is_negative = value < 0;
      const uint64_t trailing_bits_value =
          is_negative ? (value - LLONG_MIN) : value;
      if (!ValueToByteString<int64_t>(
              trailing_bits_value, &to_return)) {
        log_fatal("length too big");
      }
      if (is_negative) {
        to_return[0] |= (uint64_t)1 << (CHAR_BIT - 1);
      }
      return to_return;
    }
    case DataType::UINT64: {
      vector<unsigned char> to_return;
      const uint64_t value =
          GetValue<uint64_t>(*((Uint64DataType *)input.value_.get()));
      if (!ValueToByteString<uint64_t>(value, &to_return)) {
        log_fatal("length too big");
      }
      return to_return;
    }
    case DataType::SLICE: {
      vector<unsigned char> to_return;
      const slice value =
          GetValue<slice>(*((SliceDataType *)input.value_.get()));
      if (!ValueToByteString<slice>(value, &to_return)) {
        log_fatal("length too big");
      }
      return to_return;
    }
    case DataType::DOUBLE: {
      // TODO(PHB): Determine the best way to convert a double to a byte-string;
      // (just using the system-dependent representation, e.g. by how the
      // system stores doubles in memory, is probably not appropriate, since
      // the assumption is that this function is returning a system-independent
      // byte-string representation; but perhaps this is the best we can do,
      // and we should just make sure all code paths understand that the byte-
      // string returned by the present function for when the input
      // GenericValue has type double is system-dependent).
      log_fatal("GetTwosComplementByteString() for double is currently "
                "unsupported.");
    }
    case DataType::VECTOR: {
      // NOTE: Information for how many elements are in the vector, and the
      // DataType of each, will be lost here, as all values are concatenated
      // into a single char array (which doesn't encode these pieces of
      // information as e.g. metadata in the first several bytes).
      // Thus, caller must be able to retrieve this information via other calls
      // on the original GenericValue, if necessary.
      vector<unsigned char> to_return(GetValueNumBytes(input));
      const vector<GenericValue> & input_cast =
          ((const VectorDataType *)input.value_.get())->value_;
      uint64_t byte_index = 0;
      for (size_t i = 0; i < input_cast.size(); ++i) {
        const GenericValue & input_i = input_cast[i];
        vector<unsigned char> temp =
            GetTwosComplementByteString(input_i);
        for (size_t j = 0; j < temp.size(); ++j) {
          to_return[byte_index] = temp[j];
          ++byte_index;
        }
      }
      return to_return;
    }
    default:
      log_fatal(
          "Unsupported DataType: %s",
          Itoa(static_cast<int>(input.type_)).c_str());
  }

  // Code should never reach here.
  return vector<unsigned char>();
}

uint64_t GetValueNumBits(const DataType type) {
  switch (type) {
    case DataType::UNKNOWN: {
      return 0;
    }
    case DataType::STRING8: {
      return 8 * CHAR_BIT;
    }
    case DataType::STRING16: {
      return 16 * CHAR_BIT;
    }
    case DataType::STRING24: {
      return 24 * CHAR_BIT;
    }
    case DataType::STRING32: {
      return 32 * CHAR_BIT;
    }
    case DataType::STRING64: {
      return 64 * CHAR_BIT;
    }
    case DataType::STRING128: {
      return 128 * CHAR_BIT;
    }
    case DataType::BOOL: {
      return 1;
    }
    case DataType::INT2: {
      return 2;
    }
    case DataType::UINT2: {
      return 2;
    }
    case DataType::INT4: {
      return 4;
    }
    case DataType::UINT4: {
      return 4;
    }
    case DataType::INT8: {
      return sizeof(char) * CHAR_BIT;
    }
    case DataType::UINT8: {
      return sizeof(unsigned char) * CHAR_BIT;
    }
    case DataType::INT16: {
      return sizeof(short) * CHAR_BIT;
    }
    case DataType::UINT16: {
      return sizeof(unsigned short) * CHAR_BIT;
    }
    case DataType::INT32: {
      return sizeof(int32_t) * CHAR_BIT;
    }
    case DataType::UINT32: {
      return sizeof(uint32_t) * CHAR_BIT;
    }
    case DataType::INT64: {
      return sizeof(int64_t) * CHAR_BIT;
    }
    case DataType::UINT64: {
      return sizeof(uint64_t) * CHAR_BIT;
    }
    case DataType::SLICE: {
      return sizeof(slice) * CHAR_BIT;
    }
    case DataType::DOUBLE: {
      return sizeof(double) * CHAR_BIT;
    }
    case DataType::VECTOR: {
      log_fatal("Cannot determine number of bits in DataType::VECTOR.");
    }
    default:
      log_fatal(
          "Unsupported DataType: %s",
          Itoa(static_cast<int>(type)).c_str());
  }

  // Code should never reach here.
  return 0;
}

uint64_t GetValueNumBits(const GenericValue & input) {
  if (input.type_ == DataType::VECTOR) {
    const vector<GenericValue> & input_cast =
        ((const VectorDataType *)input.value_.get())->value_;
    uint64_t to_return = 0;
    for (size_t i = 0; i < input_cast.size(); ++i) {
      to_return += GetValueNumBits(input_cast[i]);
    }
    return to_return;
  }

  return GetValueNumBits(input.type_);
}

uint64_t GetValueNumBytes(const DataType type) {
  switch (type) {
    case DataType::UNKNOWN: {
      return 0;
    }
    case DataType::STRING8: {
      return 8;
    }
    case DataType::STRING16: {
      return 16;
    }
    case DataType::STRING24: {
      return 24;
    }
    case DataType::STRING32: {
      return 32;
    }
    case DataType::STRING64: {
      return 64;
    }
    case DataType::STRING128: {
      return 128;
    }
    case DataType::BOOL: {
      return 1;
    }
    case DataType::INT2: {
      return sizeof(char);
    }
    case DataType::UINT2: {
      return sizeof(unsigned char);
    }
    case DataType::INT4: {
      return sizeof(char);
    }
    case DataType::UINT4: {
      return sizeof(unsigned char);
    }
    case DataType::INT8: {
      return sizeof(char);
    }
    case DataType::UINT8: {
      return sizeof(unsigned char);
    }
    case DataType::INT16: {
      return sizeof(short);
    }
    case DataType::UINT16: {
      return sizeof(unsigned short);
    }
    case DataType::INT32: {
      return sizeof(int32_t);
    }
    case DataType::UINT32: {
      return sizeof(uint32_t);
    }
    case DataType::INT64: {
      return sizeof(int64_t);
    }
    case DataType::UINT64: {
      return sizeof(uint64_t);
    }
    case DataType::SLICE: {
      return sizeof(slice);
    }
    case DataType::DOUBLE: {
      return sizeof(double);
    }
    case DataType::VECTOR: {
      log_fatal("Cannot determine number of bits in DataType::VECTOR.");
    }
    default:
      log_fatal(
          "Unsupported DataType: %s",
          Itoa(static_cast<int>(type)).c_str());
  }

  // Code should never reach here.
  return 0;
}

uint64_t GetValueNumBytes(const GenericValue & input) {
  if (input.type_ == DataType::VECTOR) {
    const vector<GenericValue> & input_cast =
        ((const VectorDataType *)input.value_.get())->value_;
    uint64_t to_return = 0;
    for (size_t i = 0; i < input_cast.size(); ++i) {
      to_return += GetValueNumBytes(input_cast[i]);
    }
    return to_return;
  }

  return GetValueNumBytes(input.type_);
}

bool GetIntegerDataType(
    const bool is_signed, const int num_bits, DataType * type) {
  *type = DataType::UNKNOWN;
  if (num_bits == 1) {
    *type = DataType::BOOL;
  } else if (num_bits == 2) {
    *type = is_signed ? DataType::INT2 : DataType::UINT2;
  } else if (num_bits <= 4) {
    *type = is_signed ? DataType::INT4 : DataType::UINT4;
  } else if (num_bits <= 8) {
    *type = is_signed ? DataType::INT8 : DataType::UINT8;
  } else if (num_bits <= 16) {
    *type = is_signed ? DataType::INT16 : DataType::UINT16;
  } else if (num_bits <= 32) {
    *type = is_signed ? DataType::INT32 : DataType::UINT32;
  } else if (num_bits <= 64) {
    *type = is_signed ? DataType::INT64 : DataType::UINT64;
  } else {
    log_error("Too many bits.");
    return false;
  }

  return true;
}

bool ParseIfInteger(const string & input, GenericValue * output) {
  if (HasPrefixString(input, "-"))
    return ParseIfInteger(true, input.substr(1), output);
  return ParseIfInteger(false, input, output);
}

bool ParseIfInteger(
    const bool is_negative,
    const string & input,
    GenericValue * output) {
  // First, check if input has suffix "_DATATYPE" for one of the numeric DataTypes.
  string input_no_type = input;
  DataType type = DataType::UNKNOWN;
  if (HasSuffixString(input, "_BIT")) {
    // Unsigned DataTypes should not be negative.
    if (is_negative)
      return false;
    type = DataType::BOOL;
    input_no_type = StripSuffixString(input, "_BIT");
  } else if (HasSuffixString(input, "_BOOL")) {
    // Unsigned DataTypes should not be negative.
    if (is_negative)
      return false;
    type = DataType::BOOL;
    input_no_type = StripSuffixString(input, "_BOOL");
  } else if (HasSuffixString(input, "_INT2")) {
    type = DataType::INT2;
    input_no_type = StripSuffixString(input, "_INT2");
  } else if (HasSuffixString(input, "_UINT2")) {
    // Unsigned DataTypes should not be negative.
    if (is_negative)
      return false;
    type = DataType::UINT2;
    input_no_type = StripSuffixString(input, "_UINT2");
  } else if (HasSuffixString(input, "_INT4")) {
    type = DataType::INT4;
    input_no_type = StripSuffixString(input, "_INT4");
  } else if (HasSuffixString(input, "_UINT4")) {
    // Unsigned DataTypes should not be negative.
    if (is_negative)
      return false;
    type = DataType::UINT4;
    input_no_type = StripSuffixString(input, "_UINT4");
  } else if (HasSuffixString(input, "_INT8")) {
    type = DataType::INT8;
    input_no_type = StripSuffixString(input, "_INT8");
  } else if (HasSuffixString(input, "_UINT8")) {
    // Unsigned DataTypes should not be negative.
    if (is_negative)
      return false;
    type = DataType::UINT8;
    input_no_type = StripSuffixString(input, "_UINT8");
  } else if (HasSuffixString(input, "_INT16")) {
    type = DataType::INT16;
    input_no_type = StripSuffixString(input, "_INT16");
  } else if (HasSuffixString(input, "_UINT16")) {
    // Unsigned DataTypes should not be negative.
    if (is_negative)
      return false;
    type = DataType::UINT16;
    input_no_type = StripSuffixString(input, "_UINT16");
  } else if (HasSuffixString(input, "_INT32")) {
    type = DataType::INT32;
    input_no_type = StripSuffixString(input, "_INT32");
  } else if (HasSuffixString(input, "_UINT32")) {
    // Unsigned DataTypes should not be negative.
    if (is_negative)
      return false;
    type = DataType::UINT32;
    input_no_type = StripSuffixString(input, "_UINT32");
  } else if (HasSuffixString(input, "_INT64")) {
    type = DataType::INT64;
    input_no_type = StripSuffixString(input, "_INT64");
  } else if (HasSuffixString(input, "_UINT64")) {
    // Unsigned DataTypes should not be negative.
    if (is_negative)
      return false;
    type = DataType::UINT64;
    input_no_type = StripSuffixString(input, "_UINT64");
  } else if (HasSuffixString(input, "_SLICE")) {
    // Unsigned DataTypes should not be negative.
    if (is_negative)
      return false;
    type = DataType::SLICE;
    input_no_type = StripSuffixString(input, "_SLICE");
  }

  if (!IsAllDigits(input_no_type))
    return false;

  // Make sure there aren't so many digits that they don't fit.
  uint64_t value;
  if (!Stoi(input_no_type, &value) || value < 0)
    return false;

  // Make sure the value is small enough to fit in the largest
  // [un]signed integer DataType: [U]INT64.
  if ((is_negative && value > LLONG_MAX) || value > ULLONG_MAX) {
    return false;
  }

  // If DataType was specified, try to cast 'value' as that type.
  if (type != DataType::UNKNOWN) {
    if (output != nullptr)
      *output = GenericValue(type);
    if (type == DataType::BOOL) {
      if (value != 0 && value != 1)
        return false;
      if (output == nullptr)
        return true;
      output->value_.reset(new BoolDataType(value != 0));
    } else if (type == DataType::SLICE) {
      if (output == nullptr)
        return true;
      output->value_.reset(new SliceDataType((slice)value));
    } else if (type == DataType::INT2) {
      if (value > 1)
        return false;
      if (output == nullptr)
        return true;
      output->value_.reset(new Int2DataType(value == 0 ? 0 : -1));
    } else if (type == DataType::UINT2) {
      if (value > 3)
        return false;
      if (output == nullptr)
        return true;
      output->value_.reset(new Uint2DataType((unsigned char)value));
    } else if (type == DataType::INT4) {
      if (value > 7)
        return false;
      if (output == nullptr)
        return true;
      if (is_negative) {
        output->value_.reset(new Int4DataType(((char)value) * -1));
      } else {
        output->value_.reset(new Int4DataType((char)value));
      }
    } else if (type == DataType::UINT4) {
      if (value > 15)
        return false;
      if (output == nullptr)
        return true;
      output->value_.reset(new Uint4DataType((unsigned char)value));
    } else if (type == DataType::INT8) {
      if (value > SCHAR_MAX)
        return false;
      if (output == nullptr)
        return true;
      if (is_negative) {
        output->value_.reset(
            new Int8DataType(((char)value) * (char)-1));
      } else {
        output->value_.reset(new Int8DataType((char)value));
      }
    } else if (type == DataType::UINT8) {
      if (value > UCHAR_MAX)
        return false;
      if (output == nullptr)
        return true;
      output->value_.reset(new Uint8DataType((unsigned char)value));
    } else if (type == DataType::INT16) {
      if (value > SHRT_MAX)
        return false;
      if (output == nullptr)
        return true;
      if (is_negative) {
        output->value_.reset(
            new Int16DataType(((short)value) * (short)-1));
      } else {
        output->value_.reset(new Int16DataType((short)value));
      }
    } else if (type == DataType::UINT16) {
      if (value > USHRT_MAX)
        return false;
      if (output == nullptr)
        return true;
      output->value_.reset(new Uint16DataType((unsigned short)value));
    } else if (type == DataType::INT32) {
      if (value > LONG_MAX)
        return false;
      if (output == nullptr)
        return true;
      if (is_negative) {
        output->value_.reset(
            new Int32DataType(((int32_t)value) * (int32_t)-1));
      } else {
        output->value_.reset(new Int32DataType((int32_t)value));
      }
    } else if (type == DataType::UINT32) {
      if (value > ULONG_MAX)
        return false;
      if (output == nullptr)
        return true;
      output->value_.reset(new Uint32DataType((uint32_t)value));
    } else if (type == DataType::INT64) {
      if (value > LLONG_MAX)
        return false;
      if (output == nullptr)
        return true;
      if (is_negative) {
        output->value_.reset(
            new Int64DataType(((int64_t)value) * (int64_t)-1));
      } else {
        output->value_.reset(new Int64DataType((int64_t)value));
      }
    } else if (type == DataType::UINT64) {
      if (value > ULLONG_MAX)
        return false;
      if (output == nullptr)
        return true;
      output->value_.reset(new Uint64DataType(value));
    } else {
      return false;
    }

    return true;
  }

  // Nothing more to do if not using the API that populates output.
  if (output == nullptr)
    return true;

  // Only reach here if DataType was not specified (as a suffix).
  // Cast value into the smallest DataType it can fit into.
  if (is_negative) {
    // Assign DataType to be the smallest (signed) DataType that can
    // store 'value'.
    if (value == 0 || value == 1) {
      *output = GenericValue(DataType::INT2);
      output->value_.reset(new Int2DataType(value == 0 ? 0 : -1));
    } else if (value <= 7) {
      *output = GenericValue(DataType::INT4);
      output->value_.reset(new Int2DataType((char)value * -1));
    } else if (value <= SCHAR_MAX) {
      char val = (char)value * -1;
      *output = GenericValue(val);
    } else if (value <= SHRT_MAX) {
      short val = (short)value * -1;
      *output = GenericValue(val);
    } else if (value <= LONG_MAX) {
      int32_t val = (int32_t)value * -1;
      *output = GenericValue(val);
    } else {
      int64_t val = (int64_t)value * -1;
      *output = GenericValue(val);
    }
  } else {
    if (value == 0 || value == 1) {
      *output = GenericValue(DataType::BOOL);
      output->value_.reset(new BoolDataType(value == 1));
    } else if (value == 2 || value == 3) {
      *output = GenericValue(DataType::UINT2);
      output->value_.reset(new Uint2DataType((unsigned char)value));
    } else if (value <= 15) {
      *output = GenericValue(DataType::UINT4);
      output->value_.reset(new Uint4DataType((unsigned char)value));
    } else if (value <= UCHAR_MAX) {
      unsigned char val = (unsigned char)value;
      *output = GenericValue(val);
    } else if (value <= USHRT_MAX) {
      unsigned short val = (unsigned short)value;
      *output = GenericValue(val);
    } else if (value <= ULONG_MAX) {
      uint32_t val = (uint32_t)value;
      *output = GenericValue(val);
    } else {
      uint64_t val = (uint64_t)value;
      *output = GenericValue(val);
    }
  }

  return true;
}

bool ParseIfDouble(const string & input, GenericValue * output) {
  if (HasPrefixString(input, "-"))
    return ParseIfDouble(true, input.substr(1), output);
  return ParseIfDouble(false, input, output);
}

// Same as above, but allows for more numeric representations (representing
// a double), e.g. scientific notation and decimals.
bool ParseIfDouble(
    const bool is_negative,
    const string & input,
    GenericValue * output) {
  if (!is_negative && HasPrefixString(input, "-"))
    return false;

  // First, check if input has suffix "_DOUBLE" for one of the numeric DataTypes.
  string input_no_type = input;
  if (HasSuffixString(input, "_DOUBLE")) {
    input_no_type = StripSuffixString(input, "_DOUBLE");
  }

  if (!IsNumeric(input_no_type))
    return false;
  if (output == nullptr)
    return true;

  output->type_ = DataType::DOUBLE;
  double value;
  if (!Stod(input_no_type, &value))
    return false;
  output->value_.reset(new DoubleDataType(value));
  return true;
}

bool ParseGenericValue(
    const DataType type, const string & input, GenericValue * output) {
  if (input.empty() || output == nullptr)
    return false;
  output->type_ = type;
  switch (type) {
    case DataType::STRING8: {
      if (input.length() > 8)
        log_fatal("length too big.");
      output->value_.reset(new StringDataType(input));
      return true;
    }
    case DataType::STRING16: {
      if (input.length() > 16)
        log_fatal("length too big.");
      output->value_.reset(new StringDataType(input));
      return true;
    }
    case DataType::STRING24: {
      if (input.length() > 24)
        log_fatal("length too big.");
      output->value_.reset(new StringDataType(input));
      return true;
    }
    case DataType::STRING32: {
      if (input.length() > 32)
        log_fatal("length too big.");
      output->value_.reset(new StringDataType(input));
      return true;
    }
    case DataType::STRING64: {
      if (input.length() > 64)
        log_fatal("length too big.");
      output->value_.reset(new StringDataType(input));
      return true;
    }
    case DataType::STRING128: {
      if (input.length() > 128)
        log_fatal("length too big.");
      output->value_.reset(new StringDataType(input));
      return true;
    }
    case DataType::BOOL: {
      if (input == "0") {
        output->value_.reset(new BoolDataType(false));
        return true;
      }
      if (input == "1") {
        output->value_.reset(new BoolDataType(true));
        return true;
      }
      return false;
    }
    case DataType::INT2: {
      int value;
      if (!Stoi(input, &value))
        return false;
      if (value < -2 || value > 1)
        return false;
      output->value_.reset(new Int2DataType((char)value));
      return true;
    }
    case DataType::UINT2: {
      int value;
      if (!Stoi(input, &value))
        return false;
      if (value < 0 || value > 3)
        return false;
      output->value_.reset(new Uint2DataType((unsigned char)value));
      return true;
    }
    case DataType::INT4: {
      int value;
      if (!Stoi(input, &value))
        return false;
      if (value < -8 || value > 7)
        return false;
      output->value_.reset(new Int4DataType((char)value));
      return true;
    }
    case DataType::UINT4: {
      int value;
      if (!Stoi(input, &value))
        return false;
      if (value < 0 || value > 15)
        return false;
      output->value_.reset(new Uint4DataType((unsigned char)value));
      return true;
    }
    case DataType::INT8: {
      char value;
      if (!Stoi(input, &value))
        return false;
      output->value_.reset(new Int8DataType(value));
      return true;
    }
    case DataType::UINT8: {
      unsigned char value;
      if (!Stoi(input, &value))
        return false;
      output->value_.reset(new Uint8DataType(value));
      return true;
    }
    case DataType::INT16: {
      short value;
      if (!Stoi(input, &value))
        return false;
      output->value_.reset(new Int16DataType(value));
      return true;
    }
    case DataType::UINT16: {
      unsigned short value;
      if (!Stoi(input, &value))
        return false;
      output->value_.reset(new Uint16DataType(value));
      return true;
    }
    case DataType::INT32: {
      int32_t value;
      if (!Stoi(input, &value))
        return false;
      output->value_.reset(new Int32DataType(value));
      return true;
    }
    case DataType::UINT32: {
      uint32_t value;
      if (!Stoi(input, &value))
        return false;
      output->value_.reset(new Uint32DataType(value));
      return true;
    }
    case DataType::INT64: {
      int64_t value;
      if (!Stoi(input, &value))
        return false;
      output->value_.reset(new Int64DataType(value));
      return true;
    }
    case DataType::UINT64: {
      uint64_t value;
      if (!Stoi(input, &value))
        return false;
      output->value_.reset(new Uint64DataType(value));
      return true;
    }
    case DataType::SLICE: {
      slice value;
      if (!Stoi(input, &value))
        return false;
      output->value_.reset(new SliceDataType(value));
      return true;
    }
    case DataType::DOUBLE: {
      double value;
      if (!Stod(input, &value))
        return false;
      output->value_.reset(new DoubleDataType(value));
      return true;
    }
    case DataType::VECTOR: {
      // Because we needed the passed-in parameter 'type' to indicate VECTOR,
      // we don't know what the underlying DataType of each coordinate
      // (GenericValue) should be. We adopt the convention:
      //   - If a coordinate can be parsed as an integer
      //     (via ParseIfInteger()), then do so
      //   - Otherwise, if a coordinate can be parsed as an integer
      //     (via ParseIfInteger()), then do so
      //   - Otherwise, put as smallest string that can fit
      if_debug log_warn(
          "Constructing a Vector of GenericValues without knowing "
          "the underlying DataType of each value.");

      output->type_ = DataType::VECTOR;
      vector<string> parts;
      Split(input, ",", &parts);
      output->value_.reset(new VectorDataType());
      vector<GenericValue> & out_values =
          ((VectorDataType *)output->value_.get())->value_;
      out_values.resize(parts.size());
      for (size_t i = 0; i < parts.size(); ++i) {
        const string & part_i = parts[i];
        if (!ParseIfInteger(part_i, &(out_values[i])) &&
            !ParseIfDouble(part_i, &(out_values[i]))) {
          out_values[i].SetStringGenericValue(part_i);
        }
      }
      return true;
    }
    default:
      log_fatal(
          "Unsupported DataType: %s",
          Itoa(static_cast<int>(type)).c_str());
  }

  // Code should never reach here.
  return false;
}

bool ParseGenericValueFromTwosComplementString(
    const DataType type,
    const string & binary_string,
    GenericValue * output) {
  if (binary_string.empty() || output == nullptr) {
    log_error("Bad input.");
    return false;
  }
  output->type_ = type;
  switch (type) {
    case DataType::STRING8: {
      if (binary_string.length() % CHAR_BIT != 0) {
        log_error(
            "Unexpected binary string representation of a string: %s",
            binary_string.c_str());
        return false;
      }
      vector<char> string_as_chars;
      BinaryStringToCharVector(binary_string, &string_as_chars);
      output->value_.reset(new StringDataType(
          string(string_as_chars.begin(), string_as_chars.end())));
      return true;
    }
    case DataType::STRING16: {
      if (binary_string.length() % CHAR_BIT != 0) {
        log_error(
            "Unexpected binary string representation of a string: %s",
            binary_string.c_str());
        return false;
      }
      vector<char> string_as_chars;
      BinaryStringToCharVector(binary_string, &string_as_chars);
      output->value_.reset(new StringDataType(
          string(string_as_chars.begin(), string_as_chars.end())));
      return true;
    }
    case DataType::STRING24: {
      if (binary_string.length() % CHAR_BIT != 0) {
        log_error(
            "Unexpected binary string representation of a string: %s",
            binary_string.c_str());
        return false;
      }
      vector<char> string_as_chars;
      BinaryStringToCharVector(binary_string, &string_as_chars);
      output->value_.reset(new StringDataType(
          string(string_as_chars.begin(), string_as_chars.end())));
      return true;
    }
    case DataType::STRING32: {
      if (binary_string.length() % CHAR_BIT != 0) {
        log_error(
            "Unexpected binary string representation of a string: %s",
            binary_string.c_str());
        return false;
      }
      vector<char> string_as_chars;
      BinaryStringToCharVector(binary_string, &string_as_chars);
      output->value_.reset(new StringDataType(
          string(string_as_chars.begin(), string_as_chars.end())));
      return true;
    }
    case DataType::STRING64: {
      if (binary_string.length() % CHAR_BIT != 0) {
        log_error(
            "Unexpected binary string representation of a string: %s",
            binary_string.c_str());
        return false;
      }
      vector<char> string_as_chars;
      BinaryStringToCharVector(binary_string, &string_as_chars);
      output->value_.reset(new StringDataType(
          string(string_as_chars.begin(), string_as_chars.end())));
      return true;
    }
    case DataType::STRING128: {
      if (binary_string.length() % CHAR_BIT != 0) {
        log_error(
            "Unexpected binary string representation of a string: %s",
            binary_string.c_str());
        return false;
      }
      vector<char> string_as_chars;
      BinaryStringToCharVector(binary_string, &string_as_chars);
      output->value_.reset(new StringDataType(
          string(string_as_chars.begin(), string_as_chars.end())));
      return true;
    }
    case DataType::BOOL: {
      if (binary_string == "0" || binary_string == "00000000") {
        output->value_.reset(new BoolDataType(false));
        return true;
      }
      if (binary_string == "1" || binary_string == "00000001") {
        output->value_.reset(new BoolDataType(true));
        return true;
      }
      log_error(
          "Invalid bool representation: '%s'", binary_string.c_str());
      return false;
    }
    case DataType::INT2: {
      if (binary_string.length() != 2 && binary_string.length() != 8) {
        log_error(
            "Unexpected 2's complement representation of a INT2: %s",
            binary_string.c_str());
        return false;
      }
      if (binary_string == "00" || binary_string == "00000000") {
        output->value_.reset(new Int2DataType(0));
      } else if (binary_string == "01" || binary_string == "00000001") {
        output->value_.reset(new Int2DataType(1));
      } else if (binary_string == "10" || binary_string == "11111110") {
        output->value_.reset(new Int2DataType(-2));
      } else if (binary_string == "11" || binary_string == "11111111") {
        output->value_.reset(new Int2DataType(-1));
      } else {
        log_error(
            "Invalid INT2 representation: '%s'", binary_string.c_str());
        return false;
      }
      return true;
    }
    case DataType::UINT2: {
      if (binary_string.length() != 2 && binary_string.length() != 8) {
        log_error(
            "Unexpected binary string representation of a INT2: %s",
            binary_string.c_str());
        return false;
      }
      if (binary_string == "00" || binary_string == "00000000") {
        output->value_.reset(new Uint2DataType(0));
      } else if (binary_string == "01" || binary_string == "00000001") {
        output->value_.reset(new Uint2DataType(1));
      } else if (binary_string == "10" || binary_string == "00000010") {
        output->value_.reset(new Uint2DataType(2));
      } else if (binary_string == "11" || binary_string == "00000011") {
        output->value_.reset(new Uint2DataType(3));
      } else {
        log_error(
            "Invalid UINT2 representation: '%s'",
            binary_string.c_str());
        return false;
      }
      return true;
    }
    case DataType::INT4: {
      if (binary_string.length() != 4 && binary_string.length() != 8) {
        log_error(
            "Unexpected 2's complement representation of a INT4: %s",
            binary_string.c_str());
        return false;
      }
      const string suffix = binary_string.length() == 8 ?
          binary_string.substr(4) :
          binary_string;
      unsigned char trailing_bits_value;
      if (!BinaryStringToValue(
              suffix.substr(1), &trailing_bits_value)) {
        log_error("Invalid binary string: '%s'", binary_string.c_str());
        return false;
      }
      if (suffix.substr(0, 1) == "1") {
        output->value_.reset(
            new Int4DataType((char)trailing_bits_value - 8));
      } else {
        output->value_.reset(
            new Int4DataType((char)trailing_bits_value));
      }
      return true;
    }
    case DataType::UINT4: {
      if (binary_string.length() != 4 && binary_string.length() != 8) {
        log_error(
            "Unexpected binary string representation of a UINT4: %s",
            binary_string.c_str());
        return false;
      }
      const string suffix = binary_string.length() == 8 ?
          binary_string.substr(4) :
          binary_string;
      unsigned char value;
      if (!BinaryStringToValue(suffix, &value)) {
        log_error("Invalid binary string: '%s'", binary_string.c_str());
        return false;
      }
      output->value_.reset(new Uint4DataType(value));
      return true;
    }
    case DataType::INT8: {
      if (binary_string.length() != CHAR_BIT * sizeof(char)) {
        log_error(
            "Unexpected 2's complement representation of a INT8: %s",
            binary_string.c_str());
        return false;
      }
      char value;
      if (!TwosComplementStringToValue(binary_string, &value)) {
        log_error("Invalid binary string: '%s'", binary_string.c_str());
        return false;
      }
      output->value_.reset(new Int8DataType(value));
      return true;
    }
    case DataType::UINT8: {
      if (binary_string.length() != CHAR_BIT * sizeof(unsigned char)) {
        log_error(
            "Unexpected binary string representation of a UINT8: %s",
            binary_string.c_str());
        return false;
      }
      unsigned char value;
      if (!BinaryStringToValue(binary_string, &value)) {
        log_error("Invalid binary string: '%s'", binary_string.c_str());
        return false;
      }
      output->value_.reset(new Uint8DataType(value));
      return true;
    }
    case DataType::INT16: {
      if (binary_string.length() != CHAR_BIT * sizeof(short)) {
        log_error(
            "Unexpected 2's complement representation of a INT16: %s",
            binary_string.c_str());
        return false;
      }
      short value;
      if (!TwosComplementStringToValue(binary_string, &value)) {
        log_error("Invalid binary string: '%s'", binary_string.c_str());
        return false;
      }
      output->value_.reset(new Int16DataType(value));
      return true;
    }
    case DataType::UINT16: {
      if (binary_string.length() != CHAR_BIT * sizeof(unsigned short)) {
        log_error(
            "Unexpected binary string representation of a UINT16: %s",
            binary_string.c_str());
        return false;
      }
      unsigned short value;
      if (!BinaryStringToValue(binary_string, &value)) {
        log_error("Invalid binary string: '%s'", binary_string.c_str());
        return false;
      }
      output->value_.reset(new Uint16DataType(value));
      return true;
    }
    case DataType::INT32: {
      if (binary_string.length() != CHAR_BIT * sizeof(int32_t)) {
        log_error(
            "Unexpected 2's complement representation of a INT32: %s",
            binary_string.c_str());
        return false;
      }
      int32_t value;
      if (!TwosComplementStringToValue(binary_string, &value)) {
        log_error("Invalid binary string: '%s'", binary_string.c_str());
        return false;
      }
      output->value_.reset(new Int32DataType(value));
      return true;
    }
    case DataType::UINT32: {
      if (binary_string.length() != CHAR_BIT * sizeof(uint32_t)) {
        log_error(
            "Unexpected binary string representation of a UINT32: %s",
            binary_string.c_str());
        return false;
      }
      uint32_t value;
      if (!BinaryStringToValue(binary_string, &value)) {
        log_error("Invalid binary string: '%s'", binary_string.c_str());
        return false;
      }
      output->value_.reset(new Uint32DataType(value));
      return true;
    }
    case DataType::INT64: {
      if (binary_string.length() != CHAR_BIT * sizeof(int64_t)) {
        log_error(
            "Unexpected 2's complement representation of a INT64: %s",
            binary_string.c_str());
        return false;
      }
      int64_t value;
      if (!TwosComplementStringToValue(binary_string, &value)) {
        log_error("Invalid binary string: '%s'", binary_string.c_str());
        return false;
      }
      output->value_.reset(new Int64DataType(value));
      return true;
    }
    case DataType::UINT64: {
      if (binary_string.length() != CHAR_BIT * sizeof(uint64_t)) {
        log_error(
            "Unexpected binary string representation of a UINT64: %s",
            binary_string.c_str());
        return false;
      }
      uint64_t value;
      if (!BinaryStringToValue(binary_string, &value)) {
        log_error("Invalid binary string: '%s'", binary_string.c_str());
        return false;
      }
      output->value_.reset(new Uint64DataType(value));
      return true;
    }
    case DataType::SLICE: {
      if (binary_string.length() != CHAR_BIT * sizeof(slice)) {
        log_error(
            "Unexpected binary string representation of a SLICE: %s",
            binary_string.c_str());
        return false;
      }
      slice value;
      if (!BinaryStringToValue(binary_string, &value)) {
        log_error("Invalid binary string: '%s'", binary_string.c_str());
        return false;
      }
      output->value_.reset(new SliceDataType(value));
      return true;
    }
    case DataType::DOUBLE: {
      // TODO(PHB): Implement this.
      log_fatal(
          "Unsupported DataType: %s",
          Itoa(static_cast<int>(type)).c_str());
    }
    case DataType::VECTOR: {
      log_fatal("Cannot determine how to cast a string of bits into a "
                "GenericValue of type DataType::VECTOR.");
    }
    default:
      log_fatal(
          "Unsupported DataType: %s",
          Itoa(static_cast<int>(type)).c_str());
  }

  log_fatal("Code should never reach here.");
  return false;
}

bool ParseGenericValueFromTwosComplementString(
    const DataType type,
    const vector<unsigned char> & bytes,
    GenericValue * output) {
  string as_string;
  CharVectorToBinaryString(false, bytes, &as_string);
  return ParseGenericValueFromTwosComplementString(
      type, as_string, output);
}

bool ParseGenericValueFromTwosComplementString(
    const DataType type,
    const vector<char> & bytes,
    GenericValue * output) {
  string as_string;
  CharVectorToBinaryString(false, bytes, &as_string);
  return ParseGenericValueFromTwosComplementString(
      type, as_string, output);
}

string
GetGenericValueString(const GenericValue & input, const int precision) {
  switch (input.type_) {
    case DataType::UNKNOWN: {
      return "UNKNOWN";
    }
    case DataType::STRING8: {
      return GetValue<string>(*((StringDataType *)input.value_.get()));
    }
    case DataType::STRING16: {
      return GetValue<string>(*((StringDataType *)input.value_.get()));
    }
    case DataType::STRING24: {
      return GetValue<string>(*((StringDataType *)input.value_.get()));
    }
    case DataType::STRING32: {
      return GetValue<string>(*((StringDataType *)input.value_.get()));
    }
    case DataType::STRING64: {
      return GetValue<string>(*((StringDataType *)input.value_.get()));
    }
    case DataType::STRING128: {
      return GetValue<string>(*((StringDataType *)input.value_.get()));
    }
    case DataType::BOOL: {
      const bool value =
          GetValue<bool>(*((BoolDataType *)input.value_.get()));
      return value ? "1" : "0";
    }
    case DataType::INT2: {
      return Itoa(
          (int)GetValue<char>(*((Int2DataType *)input.value_.get())));
    }
    case DataType::UINT2: {
      return Itoa((unsigned int)GetValue<unsigned char>(
          *((Uint2DataType *)input.value_.get())));
    }
    case DataType::INT4: {
      return Itoa(
          (int)GetValue<char>(*((Int4DataType *)input.value_.get())));
    }
    case DataType::UINT4: {
      return Itoa((unsigned int)GetValue<unsigned char>(
          *((Uint4DataType *)input.value_.get())));
    }
    case DataType::INT8: {
      return Itoa(
          (int)GetValue<char>(*((Int8DataType *)input.value_.get())));
    }
    case DataType::UINT8: {
      return Itoa((unsigned int)GetValue<unsigned char>(
          *((Uint8DataType *)input.value_.get())));
    }
    case DataType::INT16: {
      return Itoa(
          GetValue<short>(*((Int16DataType *)input.value_.get())));
    }
    case DataType::UINT16: {
      return Itoa(GetValue<unsigned short>(
          *((Uint16DataType *)input.value_.get())));
    }
    case DataType::INT32: {
      return Itoa(
          GetValue<int32_t>(*((Int32DataType *)input.value_.get())));
    }
    case DataType::UINT32: {
      return Itoa(
          GetValue<uint32_t>(*((Uint32DataType *)input.value_.get())));
    }
    case DataType::INT64: {
      return Itoa(
          GetValue<int64_t>(*((Int64DataType *)input.value_.get())));
    }
    case DataType::UINT64: {
      return Itoa(
          GetValue<uint64_t>(*((Uint64DataType *)input.value_.get())));
    }
    case DataType::SLICE: {
      return Itoa(
          GetValue<slice>(*((SliceDataType *)input.value_.get())));
    }
    case DataType::DOUBLE: {
      return Itoa(
          GetValue<double>(*((DoubleDataType *)input.value_.get())),
          precision);
    }
    case DataType::VECTOR: {
      string to_return = "(";
      const vector<GenericValue> & input_cast =
          ((const VectorDataType *)input.value_.get())->value_;
      for (size_t i = 0; i < input_cast.size(); ++i) {
        to_return += GetGenericValueString(input_cast[i]);
        if (i < input_cast.size() - 1)
          to_return += ", ";
      }
      to_return += ")";
      return to_return;
    }
    default:
      log_fatal(
          "Unsupported DataType: %s",
          Itoa(static_cast<int>(input.type_)).c_str());
  }

  // Code should never reach here.
  return "";
}

string GetGenericValueString(const GenericValue & input) {
  return GetGenericValueString(input, -1);
}

CircuitOperation BoolOpToCircuitOp(const BooleanOperation op) {
  if (op == BooleanOperation::IDENTITY)
    return CircuitOperation::IDENTITY;
  if (op == BooleanOperation::NOT)
    return CircuitOperation::NOT;
  if (op == BooleanOperation::AND)
    return CircuitOperation::AND;
  if (op == BooleanOperation::NAND)
    return CircuitOperation::NAND;
  if (op == BooleanOperation::OR)
    return CircuitOperation::OR;
  if (op == BooleanOperation::NOR)
    return CircuitOperation::NOR;
  if (op == BooleanOperation::XOR)
    return CircuitOperation::XOR;
  if (op == BooleanOperation::EQ)
    return CircuitOperation::EQ;
  if (op == BooleanOperation::GT)
    return CircuitOperation::GT;
  if (op == BooleanOperation::GTE)
    return CircuitOperation::GTE;
  if (op == BooleanOperation::LT)
    return CircuitOperation::LT;
  if (op == BooleanOperation::LTE)
    return CircuitOperation::LTE;
  return CircuitOperation::UNKNOWN;
}

bool MergeValuesViaOperator(
    const OperationHolder & op,
    const GenericValue & one,
    const GenericValue & two,
    GenericValue * out) {
  if (op.type_ == OperationType::MATH) {
    return MergeValuesViaOperator(op.math_op_, one, two, out);
  }
  if (op.type_ == OperationType::BOOLEAN) {
    return MergeValuesViaOperator(op.gate_op_, one, two, out);
  }
  if (op.type_ == OperationType::COMPARISON) {
    return MergeValuesViaOperator(op.comparison_op_, one, two, out);
  }
  if (op.type_ == OperationType::ARITHMETIC) {
    return MergeValuesViaOperator(op.arithmetic_op_, one, two, out);
  }
  return false;
}

bool MergeValuesViaOperator(
    const MathOperation & op,
    const GenericValue & one,
    const GenericValue & two,
    GenericValue * out) {
  switch (op) {
    case MathOperation::IDENTITY:
      return MergeValuesViaOperator(
          ArithmeticOperation::IDENTITY, one, two, out);
    case MathOperation::ABS:
      return MergeValuesViaOperator(
          ArithmeticOperation::ABS, one, two, out);
    case MathOperation::FACTORIAL:
      return MergeValuesViaOperator(
          ArithmeticOperation::FACTORIAL, one, two, out);
    case MathOperation::SQRT:
      return MergeValuesViaOperator(
          ArithmeticOperation::SQRT, one, two, out);
    case MathOperation::ADD:
      return MergeValuesViaOperator(
          ArithmeticOperation::ADD, one, two, out);
    case MathOperation::SUB:
      return MergeValuesViaOperator(
          ArithmeticOperation::SUB, one, two, out);
    case MathOperation::MULT:
      return MergeValuesViaOperator(
          ArithmeticOperation::MULT, one, two, out);
    case MathOperation::DIV:
      return MergeValuesViaOperator(
          ArithmeticOperation::DIV, one, two, out);
    case MathOperation::POW:
      return MergeValuesViaOperator(
          ArithmeticOperation::POW, one, two, out);
    case MathOperation::EQ:
      return MergeValuesViaOperator(
          ComparisonOperation::COMP_EQ, one, two, out);
    case MathOperation::NEQ:
      return MergeValuesViaOperator(
          ComparisonOperation::COMP_NEQ, one, two, out);
    case MathOperation::GT:
      return MergeValuesViaOperator(
          ComparisonOperation::COMP_GT, one, two, out);
    case MathOperation::GTE:
      return MergeValuesViaOperator(
          ComparisonOperation::COMP_GTE, one, two, out);
    case MathOperation::LT:
      return MergeValuesViaOperator(
          ComparisonOperation::COMP_LT, one, two, out);
    case MathOperation::LTE:
      return MergeValuesViaOperator(
          ComparisonOperation::COMP_LTE, one, two, out);
    default:
      log_error("%s not yet implemented.", GetOpString(op).c_str());
      return false;
  }

  return true;
}

bool MergeValuesViaOperator(
    const BooleanOperation & op,
    const GenericValue & one,
    const GenericValue & two,
    GenericValue * out) {
  // Sanity-check usage (op should be set, and two-term op).
  if (out == nullptr || op == BooleanOperation::UNKNOWN ||
      op == BooleanOperation::IDENTITY || op == BooleanOperation::NOT) {
    return false;
  }

  // Sanity check type_ is consistent for the inputs: both numeric or
  // both string.
  if (IsStringDataType(one) != IsStringDataType(two))
    return false;
  // Boolean (bit-wise) Operators don't make sense for DOUBLEs.
  if (one.type_ == DataType::DOUBLE || two.type_ == DataType::DOUBLE)
    return false;

  vector<unsigned char> one_bytes = GetTwosComplementByteString(one);
  vector<unsigned char> two_bytes = GetTwosComplementByteString(two);
  vector<unsigned char> one_full, two_full;
  if (one_bytes.size() == two_bytes.size()) {
    one_full = one_bytes;
    two_full = two_bytes;
  } else if (one_bytes.size() > two_bytes.size()) {
    one_full = one_bytes;
    const char padding_char =
        (IsSignedDataType(two.type_) &&
         (two_bytes[0] & (((char)1) << (CHAR_BIT - 1)))) ?
        -1 :
        0;
    two_full.resize(one_bytes.size(), padding_char);
    const size_t offset = one_bytes.size() - two_bytes.size();
    for (size_t i = 0; i < two_bytes.size(); ++i) {
      two_full[offset + i] = two_bytes[i];
    }
  } else {
    two_full = two_bytes;
    const char padding_char =
        (IsSignedDataType(one.type_) &&
         (one_bytes[0] & (((char)1) << (CHAR_BIT - 1)))) ?
        -1 :
        0;
    one_full.resize(two_bytes.size(), padding_char);
    const size_t offset = two_bytes.size() - one_bytes.size();
    for (size_t i = 0; i < one_bytes.size(); ++i) {
      one_full[offset + i] = one_bytes[i];
    }
  }

  const size_t max_size = one_full.size();
  vector<unsigned char> result(max_size, 0);

  if (op == BooleanOperation::AND) {
    for (size_t i = 0; i < max_size; ++i) {
      result[i] = one_full[i] & two_full[i];
    }
  } else if (op == BooleanOperation::NAND) {
    for (size_t i = 0; i < max_size; ++i) {
      result[i] = ~(one_full[i] & two_full[i]);
    }
  } else if (op == BooleanOperation::OR) {
    for (size_t i = 0; i < max_size; ++i) {
      result[i] = one_full[i] | two_full[i];
    }
  } else if (op == BooleanOperation::NOR) {
    for (size_t i = 0; i < max_size; ++i) {
      result[i] = ~(one_full[i] | two_full[i]);
    }
  } else if (op == BooleanOperation::XOR) {
    for (size_t i = 0; i < max_size; ++i) {
      result[i] = one_full[i] ^ two_full[i];
    }
  } else if (op == BooleanOperation::EQ) {
    for (size_t i = 0; i < max_size; ++i) {
      result[i] = ~(one_full[i] ^ two_full[i]);
    }
  } else if (op == BooleanOperation::GT) {
    for (size_t i = 0; i < max_size; ++i) {
      result[i] = one_full[i] & (~two_full[i]);
    }
  } else if (op == BooleanOperation::GTE) {
    for (size_t i = 0; i < max_size; ++i) {
      result[i] = one_full[i] | (~two_full[i]);
    }
  } else if (op == BooleanOperation::LT) {
    for (size_t i = 0; i < max_size; ++i) {
      result[i] = (~one_full[i]) & two_full[i];
    }
  } else if (op == BooleanOperation::LTE) {
    for (size_t i = 0; i < max_size; ++i) {
      result[i] = (~one_full[i]) | two_full[i];
    }
  } else {
    log_error("Unexpected op: %s", GetOpString(op).c_str());
    return false;
  }

  if (IsStringDataType(one)) {
    out->SetStringGenericValue(string(result.begin(), result.end()));
    // We make the convention that applying a BooleanOperator to Integer
    // types that have different sign-types (unsigned vs. signed) should
    // result in a DataType that is signed.
  } else if (
      IsSignedDataType(one.type_) || IsSignedDataType(two.type_)) {
    if (result.size() == sizeof(int64_t)) {
      out->type_ = DataType::INT64;
      const int64_t value = ByteStringToValue<int64_t>(result);
      out->value_.reset(new Int64DataType(value));
    } else if (result.size() == sizeof(int32_t)) {
      out->type_ = DataType::INT32;
      const int32_t value = ByteStringToValue<int32_t>(result);
      out->value_.reset(new Int32DataType(value));
    } else if (result.size() == sizeof(short)) {
      out->type_ = DataType::INT16;
      const short value = ByteStringToValue<short>(result);
      out->value_.reset(new Int16DataType(value));
    } else if (result.size() == sizeof(char)) {
      const size_t max_length =
          max(GetValueNumBits(one), GetValueNumBits(two));
      if (max_length == 8) {
        out->type_ = DataType::INT8;
        out->value_.reset(new Int8DataType(result[0]));
      } else if (max_length == 4) {
        out->type_ = DataType::INT4;
        out->value_.reset(new Int4DataType(result[0]));
      } else if (max_length == 2) {
        out->type_ = DataType::INT2;
        out->value_.reset(new Int2DataType(result[0]));
      } else {
        log_error(
            "Unexpected max_length: %s", Itoa(max_length).c_str());
        return false;
      }
    } else {
      log_error(
          "Unexpected result size: %s", Itoa(result.size()).c_str());
      return false;
    }
  } else {
    if (result.size() == sizeof(uint64_t)) {
      out->type_ = DataType::UINT64;
      const uint64_t value = ByteStringToValue<uint64_t>(result);
      out->value_.reset(new Uint64DataType(value));
    } else if (result.size() == sizeof(uint32_t)) {
      out->type_ = DataType::UINT32;
      const uint32_t value = ByteStringToValue<uint32_t>(result);
      out->value_.reset(new Uint32DataType(value));
    } else if (result.size() == sizeof(unsigned short)) {
      out->type_ = DataType::UINT16;
      const unsigned short value =
          ByteStringToValue<unsigned short>(result);
      out->value_.reset(new Uint16DataType(value));
    } else if (result.size() == sizeof(unsigned char)) {
      const size_t max_length =
          max(GetValueNumBits(one), GetValueNumBits(two));
      if (max_length == 8) {
        out->type_ = DataType::UINT8;
        out->value_.reset(new Uint8DataType(result[0]));
      } else if (max_length == 4) {
        out->type_ = DataType::UINT4;
        out->value_.reset(new Uint4DataType(result[0]));
      } else if (max_length == 2) {
        out->type_ = DataType::UINT2;
        out->value_.reset(new Uint2DataType(result[0]));
      } else if (max_length == 1) {
        out->type_ = DataType::BOOL;
        out->value_.reset(
            new BoolDataType(result[0] == (unsigned char)1));
      } else {
        log_error(
            "Unexpected max_length: %s", Itoa(max_length).c_str());
        return false;
      }
    } else {
      log_error(
          "Unexpected result size: %s", Itoa(result.size()).c_str());
      return false;
    }
  }

  return true;
}

bool MergeValuesViaOperator(
    const ComparisonOperation & op,
    const GenericValue & one,
    const GenericValue & two,
    GenericValue * out) {
  // Sanity check inputs.
  if (out == nullptr || op == ComparisonOperation::UNKNOWN)
    return false;
  if (IsStringDataType(one) != IsStringDataType(two))
    return false;
  if ((one.type_ == DataType::DOUBLE) !=
      (two.type_ == DataType::DOUBLE))
    return false;

  out->type_ = DataType::BOOL;

  if (IsStringDataType(one)) {
    const string value_one = GetGenericValueString(one);
    const string value_two = GetGenericValueString(one);
    if (op == ComparisonOperation::COMP_EQ) {
      out->value_.reset(new BoolDataType(value_one == value_two));
    } else if (op == ComparisonOperation::COMP_NEQ) {
      out->value_.reset(new BoolDataType(value_one != value_two));
    } else if (op == ComparisonOperation::COMP_GT) {
      out->value_.reset(new BoolDataType(value_one > value_two));
    } else if (op == ComparisonOperation::COMP_GTE) {
      out->value_.reset(new BoolDataType(value_one >= value_two));
    } else if (op == ComparisonOperation::COMP_LT) {
      out->value_.reset(new BoolDataType(value_one < value_two));
    } else if (op == ComparisonOperation::COMP_LTE) {
      out->value_.reset(new BoolDataType(value_one <= value_two));
    } else {
      log_error("Unexpected op: %s", GetOpString(op).c_str());
      return false;
    }
  } else if (one.type_ == DataType::DOUBLE) {
    const double & value_one =
        GetValue<double>(*((DoubleDataType *)one.value_.get()));
    const double & value_two =
        GetValue<double>(*((DoubleDataType *)two.value_.get()));
    if (op == ComparisonOperation::COMP_EQ) {
      out->value_.reset(new BoolDataType(value_one == value_two));
    } else if (op == ComparisonOperation::COMP_NEQ) {
      out->value_.reset(new BoolDataType(value_one != value_two));
    } else if (op == ComparisonOperation::COMP_GT) {
      out->value_.reset(new BoolDataType(value_one > value_two));
    } else if (op == ComparisonOperation::COMP_GTE) {
      out->value_.reset(new BoolDataType(value_one >= value_two));
    } else if (op == ComparisonOperation::COMP_LT) {
      out->value_.reset(new BoolDataType(value_one < value_two));
    } else if (op == ComparisonOperation::COMP_LTE) {
      out->value_.reset(new BoolDataType(value_one <= value_two));
    } else {
      log_error("Unexpected op: %s", GetOpString(op).c_str());
      return false;
    }
  } else {
    if (one.type_ == DataType::UINT64 || one.type_ == DataType::SLICE ||
        two.type_ == DataType::UINT64 || two.type_ == DataType::SLICE) {
      if (one.type_ == DataType::UINT64 ||
          one.type_ == DataType::SLICE) {
        if (two.type_ == DataType::UINT64 ||
            two.type_ == DataType::SLICE) {
          uint64_t value_one, value_two;
          if (!GetUnsignedIntegerValue(one, &value_one) ||
              !GetUnsignedIntegerValue(two, &value_two)) {
            return false;
          }
          if (op == ComparisonOperation::COMP_EQ) {
            out->value_.reset(new BoolDataType(value_one == value_two));
          } else if (op == ComparisonOperation::COMP_NEQ) {
            out->value_.reset(new BoolDataType(value_one != value_two));
          } else if (op == ComparisonOperation::COMP_GT) {
            out->value_.reset(new BoolDataType(value_one > value_two));
          } else if (op == ComparisonOperation::COMP_GTE) {
            out->value_.reset(new BoolDataType(value_one >= value_two));
          } else if (op == ComparisonOperation::COMP_LT) {
            out->value_.reset(new BoolDataType(value_one < value_two));
          } else if (op == ComparisonOperation::COMP_LTE) {
            out->value_.reset(new BoolDataType(value_one <= value_two));
          } else {
            log_error("Unexpected op: %s", GetOpString(op).c_str());
            return false;
          }
        } else {
          int64_t value_two_temp;
          uint64_t value_one;
          if (!GetSignedIntegerValue(two, &value_two_temp) ||
              !GetUnsignedIntegerValue(one, &value_one)) {
            return false;
          }
          if (value_two_temp < 0) {
            if (op == ComparisonOperation::COMP_EQ) {
              out->value_.reset(new BoolDataType(false));
            } else if (op == ComparisonOperation::COMP_NEQ) {
              out->value_.reset(new BoolDataType(true));
            } else if (op == ComparisonOperation::COMP_GT) {
              out->value_.reset(new BoolDataType(true));
            } else if (op == ComparisonOperation::COMP_GTE) {
              out->value_.reset(new BoolDataType(true));
            } else if (op == ComparisonOperation::COMP_LT) {
              out->value_.reset(new BoolDataType(false));
            } else if (op == ComparisonOperation::COMP_LTE) {
              out->value_.reset(new BoolDataType(false));
            } else {
              log_error("Unexpected op: %s", GetOpString(op).c_str());
              return false;
            }
          } else {
            uint64_t value_two = (uint64_t)value_two_temp;
            if (op == ComparisonOperation::COMP_EQ) {
              out->value_.reset(
                  new BoolDataType(value_one == value_two));
            } else if (op == ComparisonOperation::COMP_NEQ) {
              out->value_.reset(
                  new BoolDataType(value_one != value_two));
            } else if (op == ComparisonOperation::COMP_GT) {
              out->value_.reset(
                  new BoolDataType(value_one > value_two));
            } else if (op == ComparisonOperation::COMP_GTE) {
              out->value_.reset(
                  new BoolDataType(value_one >= value_two));
            } else if (op == ComparisonOperation::COMP_LT) {
              out->value_.reset(
                  new BoolDataType(value_one < value_two));
            } else if (op == ComparisonOperation::COMP_LTE) {
              out->value_.reset(
                  new BoolDataType(value_one <= value_two));
            } else {
              log_error("Unexpected op: %s", GetOpString(op).c_str());
              return false;
            }
          }
        }
      } else {
        int64_t value_one_temp;
        uint64_t value_two;
        if (!GetSignedIntegerValue(one, &value_one_temp) ||
            !GetUnsignedIntegerValue(two, &value_two)) {
          return false;
        }
        if (value_one_temp < 0) {
          if (op == ComparisonOperation::COMP_EQ) {
            out->value_.reset(new BoolDataType(false));
          } else if (op == ComparisonOperation::COMP_NEQ) {
            out->value_.reset(new BoolDataType(true));
          } else if (op == ComparisonOperation::COMP_GT) {
            out->value_.reset(new BoolDataType(false));
          } else if (op == ComparisonOperation::COMP_GTE) {
            out->value_.reset(new BoolDataType(false));
          } else if (op == ComparisonOperation::COMP_LT) {
            out->value_.reset(new BoolDataType(true));
          } else if (op == ComparisonOperation::COMP_LTE) {
            out->value_.reset(new BoolDataType(true));
          } else {
            log_error("Unexpected op: %s", GetOpString(op).c_str());
            return false;
          }
        } else {
          uint64_t value_one = (uint64_t)value_one_temp;
          if (op == ComparisonOperation::COMP_EQ) {
            out->value_.reset(new BoolDataType(value_one == value_two));
          } else if (op == ComparisonOperation::COMP_NEQ) {
            out->value_.reset(new BoolDataType(value_one != value_two));
          } else if (op == ComparisonOperation::COMP_GT) {
            out->value_.reset(new BoolDataType(value_one > value_two));
          } else if (op == ComparisonOperation::COMP_GTE) {
            out->value_.reset(new BoolDataType(value_one >= value_two));
          } else if (op == ComparisonOperation::COMP_LT) {
            out->value_.reset(new BoolDataType(value_one < value_two));
          } else if (op == ComparisonOperation::COMP_LTE) {
            out->value_.reset(new BoolDataType(value_one <= value_two));
          } else {
            log_error("Unexpected op: %s", GetOpString(op).c_str());
            return false;
          }
        }
      }
    } else {
      // Since neither is unsigned 64 bit type, we can cast as int64_t
      // with no risk of improper cast (e.g. a large positive value being
      // interpretted as a negative).
      int64_t value_one, value_two;
      if (!GetSignedIntegerValue(one, &value_one) ||
          !GetSignedIntegerValue(two, &value_two)) {
        return false;
      }

      if (op == ComparisonOperation::COMP_EQ) {
        out->value_.reset(new BoolDataType(value_one == value_two));
      } else if (op == ComparisonOperation::COMP_NEQ) {
        out->value_.reset(new BoolDataType(value_one != value_two));
      } else if (op == ComparisonOperation::COMP_GT) {
        out->value_.reset(new BoolDataType(value_one > value_two));
      } else if (op == ComparisonOperation::COMP_GTE) {
        out->value_.reset(new BoolDataType(value_one >= value_two));
      } else if (op == ComparisonOperation::COMP_LT) {
        out->value_.reset(new BoolDataType(value_one < value_two));
      } else if (op == ComparisonOperation::COMP_LTE) {
        out->value_.reset(new BoolDataType(value_one <= value_two));
      } else {
        log_error("Unexpected op: %s", GetOpString(op).c_str());
        return false;
      }
    }
  }

  return true;
}

// TODO(PHB): This function may be called over-and-over again for circuits,
// and so minor efficiency/optimizations may make a difference. Consider
// cleaning-up below function so that it doesn't make extra checks and that
// the code flow is optimized for the most likely use-cases.
bool MergeValuesViaOperator(
    const ArithmeticOperation & op,
    const GenericValue & one,
    const GenericValue & two,
    GenericValue * out) {
  // Sanity-check usage (op should be set, and two-term op).
  if (out == nullptr || op == ArithmeticOperation::UNKNOWN ||
      op == ArithmeticOperation::ABS ||
      op == ArithmeticOperation::FLIP_SIGN ||
      op == ArithmeticOperation::FACTORIAL ||
      op == ArithmeticOperation::SQRT) {
    return false;
  }

  // Sanity check type_ is not string (arithmetic operations don't make sense
  // on strings).
  if (IsStringDataType(one) || IsStringDataType(two))
    return false;

  // Handle VECTOR type specially.
  if (one.type_ == DataType::VECTOR || two.type_ == DataType::VECTOR) {
    out->type_ = DataType::VECTOR;
    if (op == ArithmeticOperation::INNER_PRODUCT) {
      const vector<GenericValue> & one_cast =
          ((const VectorDataType *)one.value_.get())->value_;
      const vector<GenericValue> & two_cast =
          ((const VectorDataType *)two.value_.get())->value_;
      const size_t n = one_cast.size();
      if (n != two_cast.size())
        return false;
      GenericValue running_sum;
      for (size_t i = 0; i < n; ++i) {
        const GenericValue & one_i = one_cast[i];
        const GenericValue & two_i = two_cast[i];
        GenericValue product_i;
        if (!MergeValuesViaOperator(
                ArithmeticOperation::MULT,
                one_i,
                two_i,
                n == 1 ? out : &product_i)) {
          return false;
        }
        if (i == 0) {
          if (n == 1)
            return true;
          running_sum = product_i;
        } else {
          GenericValue temp;
          if (!MergeValuesViaOperator(
                  ArithmeticOperation::ADD,
                  running_sum,
                  product_i,
                  &temp)) {
            return false;
          }
          if (i != n - 1) {
            running_sum = temp;
          } else {
            *out = temp;
          }
        }
      }
    } else if (op == ArithmeticOperation::VEC) {
      out->value_.reset(new VectorDataType());
      vector<GenericValue> & out_values =
          ((VectorDataType *)out->value_.get())->value_;
      if (one.type_ == DataType::VECTOR) {
        const vector<GenericValue> & one_cast =
            ((const VectorDataType *)one.value_.get())->value_;
        out_values.resize(
            one_cast.size() +
            1); // Reserve extra slot for right vector.
        for (int i = 0; i < one_cast.size(); ++i) {
          out_values[i] = one_cast[i];
        }
      } else {
        out_values.resize(2);
        out_values[0] = one;
      }
      const int num_left_coordinates = out_values.size() - 1;
      if (two.type_ == DataType::VECTOR) {
        const vector<GenericValue> & two_cast =
            ((const VectorDataType *)two.value_.get())->value_;
        out_values.resize(num_left_coordinates + two_cast.size());
        for (int i = 0; i < two_cast.size(); ++i) {
          out_values[num_left_coordinates + i] = two_cast[i];
        }
      } else {
        out_values.resize(num_left_coordinates + 1);
        out_values[num_left_coordinates] = two;
      }
    } else if (
        op == ArithmeticOperation::ARGMIN ||
        op == ArithmeticOperation::ARGMAX ||
        op == ArithmeticOperation::ARGMIN_INTERNAL ||
        op == ArithmeticOperation::ARGMAX_INTERNAL) {
      const bool is_min = op == ArithmeticOperation::ARGMIN ||
          op == ArithmeticOperation::ARGMIN_INTERNAL;
      // Each of these operators can have one or both of the subterms be
      // a vector or a single value. Handle each case separately.
      if (one.type_ == DataType::VECTOR &&
          two.type_ == DataType::VECTOR) {
        const vector<GenericValue> & one_cast =
            ((const VectorDataType *)one.value_.get())->value_;
        const vector<GenericValue> & two_cast =
            ((const VectorDataType *)two.value_.get())->value_;
        // By convention, the minimum value of the left/right arguments are
        // stored in the *first* coordinate of each vector; and then the
        // remaining coordinates of the vector are the characteristic
        // vector (of the location/position/index of the min).
        if (one_cast.size() < 2 || two_cast.size() < 2)
          return false;
        GenericValue argmin;
        if (!MergeValuesViaOperator(
                (is_min ? ArithmeticOperation::ARGMIN_INTERNAL :
                          ArithmeticOperation::ARGMAX_INTERNAL),
                one_cast[0],
                two_cast[0],
                &argmin)) {
          return false;
        }
        const vector<GenericValue> & argmin_cast =
            ((const VectorDataType *)argmin.value_.get())->value_;
        if (argmin_cast.size() != 3)
          return false;
        const bool first_is_min = GetValue<bool>(
            *((BoolDataType *)argmin_cast[1].value_.get()));
        out->value_.reset(new VectorDataType());
        vector<GenericValue> & out_values =
            ((VectorDataType *)out->value_.get())->value_;
        const int toggle =
            (op == ArithmeticOperation::ARGMIN_INTERNAL ||
             op == ArithmeticOperation::ARGMAX_INTERNAL) ?
            1 :
            0;
        out_values.resize(
            one_cast.size() + two_cast.size() - 2 + toggle);
        if (toggle == 1)
          out_values[0] = argmin_cast[0];
        int current_index = toggle;
        const GenericValue zero = GenericValue(false);
        for (int i = 0; i < one_cast.size() - 1; ++i) {
          if (first_is_min) {
            out_values[current_index] = one_cast[1 + i];
          } else {
            out_values[current_index] = zero;
          }
          ++current_index;
        }
        for (int i = 0; i < two_cast.size() - 1; ++i) {
          if (!first_is_min) {
            out_values[current_index] = two_cast[1 + i];
          } else {
            out_values[current_index] = zero;
          }
          ++current_index;
        }
      } else if (one.type_ == DataType::VECTOR) {
        const vector<GenericValue> & one_cast =
            ((const VectorDataType *)one.value_.get())->value_;
        // By convention, the minimum value of the left/right arguments are
        // stored in the *first* coordinate of each vector; and then the
        // remaining coordinates of the vector are the characteristic
        // vector (of the location/position/index of the min).
        if (one_cast.size() < 2)
          return false;
        GenericValue argmin;
        if (!MergeValuesViaOperator(
                (is_min ? ArithmeticOperation::ARGMIN_INTERNAL :
                          ArithmeticOperation::ARGMAX_INTERNAL),
                one_cast[0],
                two,
                &argmin)) {
          return false;
        }
        const vector<GenericValue> & argmin_cast =
            ((const VectorDataType *)argmin.value_.get())->value_;
        if (argmin_cast.size() != 3)
          return false;
        const bool first_is_min = GetValue<bool>(
            *((BoolDataType *)argmin_cast[1].value_.get()));
        out->value_.reset(new VectorDataType());
        vector<GenericValue> & out_values =
            ((VectorDataType *)out->value_.get())->value_;
        const int toggle =
            (op == ArithmeticOperation::ARGMIN_INTERNAL ||
             op == ArithmeticOperation::ARGMAX_INTERNAL) ?
            1 :
            0;
        out_values.resize(one_cast.size() + toggle);
        if (toggle == 1)
          out_values[0] = argmin_cast[0];
        int current_index = toggle;
        const GenericValue zero = GenericValue(false);
        for (int i = 0; i < one_cast.size() - 1; ++i) {
          if (first_is_min) {
            out_values[current_index] = one_cast[1 + i];
          } else {
            out_values[current_index] = zero;
          }
          ++current_index;
        }
        if (!first_is_min) {
          out_values[current_index] = GenericValue(true);
        } else {
          out_values[current_index] = zero;
        }
      } else {
        const vector<GenericValue> & two_cast =
            ((const VectorDataType *)two.value_.get())->value_;
        // By convention, the minimum value of the left/right arguments are
        // stored in the *first* coordinate of each vector; and then the
        // remaining coordinates of the vector are the characteristic
        // vector (of the location/position/index of the min).
        if (two_cast.size() < 2)
          return false;
        GenericValue argmin;
        if (!MergeValuesViaOperator(
                (is_min ? ArithmeticOperation::ARGMIN_INTERNAL :
                          ArithmeticOperation::ARGMAX_INTERNAL),
                one,
                two_cast[0],
                &argmin)) {
          return false;
        }
        const vector<GenericValue> & argmin_cast =
            ((const VectorDataType *)argmin.value_.get())->value_;
        if (argmin_cast.size() != 3)
          return false;
        const bool first_is_min = GetValue<bool>(
            *((BoolDataType *)argmin_cast[1].value_.get()));
        out->value_.reset(new VectorDataType());
        vector<GenericValue> & out_values =
            ((VectorDataType *)out->value_.get())->value_;
        const int toggle =
            (op == ArithmeticOperation::ARGMIN_INTERNAL ||
             op == ArithmeticOperation::ARGMAX_INTERNAL) ?
            1 :
            0;
        out_values.resize(two_cast.size() + toggle);
        if (toggle == 1)
          out_values[0] = argmin_cast[0];
        int current_index = toggle;
        const GenericValue zero = GenericValue(false);
        if (first_is_min) {
          out_values[current_index] = GenericValue(true);
        } else {
          out_values[current_index] = zero;
        }
        ++current_index;
        for (int i = 0; i < two_cast.size() - 1; ++i) {
          if (!first_is_min) {
            out_values[current_index] = two_cast[1 + i];
          } else {
            out_values[current_index] = zero;
          }
          ++current_index;
        }
      }
    } else {
      log_error("Unexpected op: %s", GetOpString(op).c_str());
      return false;
    }
    return true;
  }

  // Some operations will be handled identically, regardless of the
  // underlying DataTypes of 'one' and 'two'. Handle those first
  // (since after this, the code goes case-by-case based on the
  // DataTypes of 'one' and 'two').
  if (op == ArithmeticOperation::ARGMIN ||
      op == ArithmeticOperation::ARGMAX ||
      op == ArithmeticOperation::VEC ||
      op == ArithmeticOperation::ARGMIN_INTERNAL ||
      op == ArithmeticOperation::ARGMAX_INTERNAL) {
    out->type_ = DataType::VECTOR;
    if (op == ArithmeticOperation::VEC) {
      out->value_.reset(new VectorDataType());
      vector<GenericValue> & out_values =
          ((VectorDataType *)out->value_.get())->value_;
      out_values.resize(2);
      out_values[0] = one;
      out_values[1] = two;
    } else if (
        op == ArithmeticOperation::ARGMIN ||
        op == ArithmeticOperation::ARGMAX ||
        op == ArithmeticOperation::ARGMIN_INTERNAL ||
        op == ArithmeticOperation::ARGMAX_INTERNAL) {
      const bool is_min = op == ArithmeticOperation::ARGMIN ||
          op == ArithmeticOperation::ARGMIN_INTERNAL;
      GenericValue comparison;
      if (!MergeValuesViaOperator(
              is_min ? ComparisonOperation::COMP_LT :
                       ComparisonOperation::COMP_GT,
              one,
              two,
              &comparison)) {
        return false;
      }
      const bool first_is_min =
          GetValue<bool>(*((BoolDataType *)comparison.value_.get()));
      out->value_.reset(new VectorDataType());
      vector<GenericValue> & out_values =
          ((VectorDataType *)out->value_.get())->value_;
      const int toggle = (op == ArithmeticOperation::ARGMIN_INTERNAL ||
                          op == ArithmeticOperation::ARGMAX_INTERNAL) ?
          1 :
          0;
      out_values.resize(2 + toggle);
      if (toggle == 1)
        out_values[0] = first_is_min ? one : two;
      int current_index = toggle;
      out_values[toggle] = GenericValue(first_is_min);
      out_values[1 + toggle] = GenericValue(!first_is_min);
    }
    return true;
  }

  // The fact that code reached here means we are not a in special case
  // (e.g. vector-valued arguments); performing the indicated operation
  // will depend on the underlying DataTypes of 'one' and 'two'.
  // Proceed case-by-case based on their DataTypes.

  // First, handle DOUBLE type specially.
  if (one.type_ == DataType::DOUBLE || two.type_ == DataType::DOUBLE) {
    if (!IsNumericDataType(one.type_) || !IsNumericDataType(two.type_))
      return false;
    out->type_ = DataType::DOUBLE;

    // Parse value_one as a double.
    double value_one_holder;
    if (one.type_ == DataType::DOUBLE) {
      value_one_holder =
          GetValue<double>(*((DoubleDataType *)one.value_.get()));
    } else if (IsSignedDataType(one.type_)) {
      int64_t temp_value_one;
      if (!GetSignedIntegerValue(one, &temp_value_one))
        return false;
      value_one_holder = temp_value_one;
    } else {
      uint64_t temp_value_one;
      if (!GetUnsignedIntegerValue(one, &temp_value_one))
        return false;
      value_one_holder = temp_value_one;
    }
    const double * value_one = &value_one_holder;

    // Parse value_two as a double.
    double value_two_holder;
    if (two.type_ == DataType::DOUBLE) {
      value_two_holder =
          GetValue<double>(*((DoubleDataType *)two.value_.get()));
    } else if (IsSignedDataType(two.type_)) {
      int64_t temp_value_two;
      if (!GetSignedIntegerValue(two, &temp_value_two))
        return false;
      value_two_holder = temp_value_two;
    } else {
      uint64_t temp_value_two;
      if (!GetUnsignedIntegerValue(two, &temp_value_two))
        return false;
    }
    const double * value_two = &value_two_holder;

    // Apply operation.
    if (op == ArithmeticOperation::ADD) {
      out->value_.reset(new DoubleDataType(*value_one + *value_two));
    } else if (op == ArithmeticOperation::SUB) {
      out->value_.reset(new DoubleDataType(*value_one - *value_two));
    } else if (
        op == ArithmeticOperation::MULT ||
        op == ArithmeticOperation::INNER_PRODUCT) {
      out->value_.reset(new DoubleDataType(*value_one * *value_two));
    } else if (op == ArithmeticOperation::DIV) {
      if (*value_two == 0.0) {
        log_error("Unable to divide by 0");
        return false;
      }
      out->value_.reset(new DoubleDataType(*value_one / *value_two));
    } else if (op == ArithmeticOperation::MIN) {
      out->value_.reset(
          new DoubleDataType(min(*value_one, *value_two)));
    } else if (op == ArithmeticOperation::MAX) {
      out->value_.reset(
          new DoubleDataType(max(*value_one, *value_two)));
    } else if (op == ArithmeticOperation::POW) {
      if (*value_one < 0.0 && ceil(*value_two) != *value_two) {
        log_error(
            "Unable to raise a negative base (%s) to a non-integer "
            "exponent (%s)",
            Itoa(*value_one).c_str(),
            Itoa(*value_two).c_str());
        return false;
      }
      if (*value_one == 0.0 && *value_two <= 0.0) {
        log_error("Unable to evaluate 0^0 or 0^negative.");
        return false;
      }
      out->value_.reset(
          new DoubleDataType(pow(*value_one, *value_two)));
    } else {
      log_error("Unexpected op: %s", GetOpString(op).c_str());
      return false;
    }
    return true;
  }

  // The fact that we reached here means we have (non-double) numeric types.
  // Handle this case as follows (in order to obey convention rule (II)):
  //   - Partition cases based on number of output bits
  //   - Within this, partition based on sign vs. unsigned output type
  if (one.type_ == DataType::UINT64 || one.type_ == DataType::SLICE ||
      two.type_ == DataType::UINT64 || two.type_ == DataType::SLICE ||
      one.type_ == DataType::INT64 || two.type_ == DataType::INT64) {
    if (!IsSignedDataType(two.type_) && !IsSignedDataType(two.type_)) {
      uint64_t value_one, value_two;
      if (!GetUnsignedIntegerValue(one, &value_one) ||
          !GetUnsignedIntegerValue(two, &value_two)) {
        return false;
      }
      out->type_ = DataType::UINT64;
      if (op == ArithmeticOperation::ADD) {
        out->value_.reset(new Uint64DataType(value_one + value_two));
      } else if (op == ArithmeticOperation::SUB) {
        out->value_.reset(new Uint64DataType(value_one - value_two));
      } else if (
          op == ArithmeticOperation::MULT ||
          op == ArithmeticOperation::INNER_PRODUCT) {
        out->value_.reset(new Uint64DataType(value_one * value_two));
      } else if (op == ArithmeticOperation::DIV) {
        if (value_two == 0) {
          log_error("Unable to divide by 0");
          return false;
        }
        out->value_.reset(new Uint64DataType(value_one / value_two));
      } else if (op == ArithmeticOperation::MIN) {
        out->value_.reset(
            new Uint64DataType(min(value_one, value_two)));
      } else if (op == ArithmeticOperation::MAX) {
        out->value_.reset(
            new Uint64DataType(max(value_one, value_two)));
      } else if (op == ArithmeticOperation::POW) {
        if (value_two == 0 && value_one == 0) {
          log_error("Unable to evaluate 0^0");
          return false;
        }
        out->value_.reset(
            new Uint64DataType(pow(value_one, value_two)));
      } else {
        log_error("Unexpected op: %s", GetOpString(op).c_str());
        return false;
      }
      return true;
    } else {
      int64_t value_one, value_two;
      uint64_t value_temp;
      if (IsSignedDataType(two.type_)) {
        if (!GetSignedIntegerValue(two, &value_two) ||
            !GetUnsignedIntegerValue(one, &value_temp)) {
          return false;
        }
        // Cast value_one as INT64 (even if it overflows and becomes negative,
        // this is what output convention rule II says we should do).
        value_one = (int64_t)value_temp;
      } else {
        if (!GetSignedIntegerValue(one, &value_one) ||
            !GetUnsignedIntegerValue(two, &value_temp)) {
          return false;
        }
        // Cast value_two as INT64 (even if it overflows and becomes negative,
        // this is what output convention rule II says we should do).
        value_two = (int64_t)value_temp;
      }
      out->type_ = DataType::INT64;
      if (op == ArithmeticOperation::ADD) {
        out->value_.reset(new Int64DataType(value_one + value_two));
      } else if (op == ArithmeticOperation::SUB) {
        out->value_.reset(new Int64DataType(value_one - value_two));
      } else if (
          op == ArithmeticOperation::MULT ||
          op == ArithmeticOperation::INNER_PRODUCT) {
        out->value_.reset(new Int64DataType(value_one * value_two));
      } else if (op == ArithmeticOperation::DIV) {
        if (value_two == 0) {
          log_error("Unable to divide by 0");
          return false;
        }
        out->value_.reset(new Int64DataType(value_one / value_two));
      } else if (op == ArithmeticOperation::MIN) {
        out->value_.reset(new Int64DataType(min(value_one, value_two)));
      } else if (op == ArithmeticOperation::MAX) {
        out->value_.reset(new Int64DataType(max(value_one, value_two)));
      } else if (op == ArithmeticOperation::POW) {
        if (value_two == 0 && value_one == 0) {
          log_error("Unable to evaluate 0^0");
          return false;
        }
        out->value_.reset(new Int64DataType(pow(value_one, value_two)));
      } else {
        log_error("Unexpected op: %s", GetOpString(op).c_str());
        return false;
      }
      return true;
    }
  } else if (
      one.type_ == DataType::UINT32 || one.type_ == DataType::INT32 ||
      two.type_ == DataType::UINT32 || two.type_ == DataType::INT32) {
    if (!IsSignedDataType(two.type_) && !IsSignedDataType(two.type_)) {
      uint32_t value_one, value_two;
      if (!GetIntegerValue<uint32_t>(one, &value_one) ||
          !GetIntegerValue<uint32_t>(two, &value_two)) {
        return false;
      }
      out->type_ = DataType::UINT32;
      if (op == ArithmeticOperation::ADD) {
        out->value_.reset(new Uint32DataType(value_one + value_two));
      } else if (op == ArithmeticOperation::SUB) {
        out->value_.reset(new Uint32DataType(value_one - value_two));
      } else if (
          op == ArithmeticOperation::MULT ||
          op == ArithmeticOperation::INNER_PRODUCT) {
        out->value_.reset(new Uint32DataType(value_one * value_two));
      } else if (op == ArithmeticOperation::DIV) {
        if (value_two == 0) {
          log_error("Unable to divide by 0");
          return false;
        }
        out->value_.reset(new Uint32DataType(value_one / value_two));
      } else if (op == ArithmeticOperation::MIN) {
        out->value_.reset(
            new Uint32DataType(min(value_one, value_two)));
      } else if (op == ArithmeticOperation::MAX) {
        out->value_.reset(
            new Uint32DataType(max(value_one, value_two)));
      } else if (op == ArithmeticOperation::POW) {
        if (value_two == 0 && value_one == 0) {
          log_error("Unable to evaluate 0^0");
          return false;
        }
        out->value_.reset(
            new Uint32DataType(pow(value_one, value_two)));
      } else {
        log_error("Unexpected op: %s", GetOpString(op).c_str());
        return false;
      }
      return true;
    } else {
      int32_t value_one, value_two;
      if (!GetIntegerValue<int32_t>(one, &value_one) ||
          !GetIntegerValue<int32_t>(two, &value_two)) {
        return false;
      }
      out->type_ = DataType::INT32;
      if (op == ArithmeticOperation::ADD) {
        out->value_.reset(new Int32DataType(value_one + value_two));
      } else if (op == ArithmeticOperation::SUB) {
        out->value_.reset(new Int32DataType(value_one - value_two));
      } else if (
          op == ArithmeticOperation::MULT ||
          op == ArithmeticOperation::INNER_PRODUCT) {
        out->value_.reset(new Int32DataType(value_one * value_two));
      } else if (op == ArithmeticOperation::DIV) {
        if (value_two == 0) {
          log_error("Unable to divide by 0");
          return false;
        }
        out->value_.reset(new Int32DataType(value_one / value_two));
      } else if (op == ArithmeticOperation::MIN) {
        out->value_.reset(new Int32DataType(min(value_one, value_two)));
      } else if (op == ArithmeticOperation::MAX) {
        out->value_.reset(new Int32DataType(max(value_one, value_two)));
      } else if (op == ArithmeticOperation::POW) {
        if (value_two == 0 && value_one == 0) {
          log_error("Unable to evaluate 0^0");
          return false;
        }
        out->value_.reset(new Int32DataType(pow(value_one, value_two)));
      } else {
        log_error("Unexpected op: %s", GetOpString(op).c_str());
        return false;
      }
      return true;
    }
  } else if (
      one.type_ == DataType::UINT16 || one.type_ == DataType::INT16 ||
      two.type_ == DataType::UINT16 || two.type_ == DataType::INT16) {
    if (!IsSignedDataType(two.type_) && !IsSignedDataType(two.type_)) {
      unsigned short value_one, value_two;
      if (!GetIntegerValue<unsigned short>(one, &value_one) ||
          !GetIntegerValue<unsigned short>(two, &value_two)) {
        return false;
      }
      out->type_ = DataType::UINT16;
      if (op == ArithmeticOperation::ADD) {
        out->value_.reset(new Uint16DataType(value_one + value_two));
      } else if (op == ArithmeticOperation::SUB) {
        out->value_.reset(new Uint16DataType(value_one - value_two));
      } else if (
          op == ArithmeticOperation::MULT ||
          op == ArithmeticOperation::INNER_PRODUCT) {
        out->value_.reset(new Uint16DataType(value_one * value_two));
      } else if (op == ArithmeticOperation::DIV) {
        if (value_two == 0) {
          log_error("Unable to divide by 0");
          return false;
        }
        out->value_.reset(new Uint16DataType(value_one / value_two));
      } else if (op == ArithmeticOperation::MIN) {
        out->value_.reset(
            new Uint16DataType(min(value_one, value_two)));
      } else if (op == ArithmeticOperation::MAX) {
        out->value_.reset(
            new Uint16DataType(max(value_one, value_two)));
      } else if (op == ArithmeticOperation::POW) {
        if (value_two == 0 && value_one == 0) {
          log_error("Unable to evaluate 0^0");
          return false;
        }
        out->value_.reset(
            new Uint16DataType(pow(value_one, value_two)));
      } else {
        log_error("Unexpected op: %s", GetOpString(op).c_str());
        return false;
      }
      return true;
    } else {
      short value_one, value_two;
      if (!GetIntegerValue<short>(one, &value_one) ||
          !GetIntegerValue<short>(two, &value_two)) {
        return false;
      }
      out->type_ = DataType::INT16;
      if (op == ArithmeticOperation::ADD) {
        out->value_.reset(new Int16DataType(value_one + value_two));
      } else if (op == ArithmeticOperation::SUB) {
        out->value_.reset(new Int16DataType(value_one - value_two));
      } else if (
          op == ArithmeticOperation::MULT ||
          op == ArithmeticOperation::INNER_PRODUCT) {
        out->value_.reset(new Int16DataType(value_one * value_two));
      } else if (op == ArithmeticOperation::DIV) {
        if (value_two == 0) {
          log_error("Unable to divide by 0");
          return false;
        }
        out->value_.reset(new Int16DataType(value_one / value_two));
      } else if (op == ArithmeticOperation::MIN) {
        out->value_.reset(new Int16DataType(min(value_one, value_two)));
      } else if (op == ArithmeticOperation::MAX) {
        out->value_.reset(new Int16DataType(max(value_one, value_two)));
      } else if (op == ArithmeticOperation::POW) {
        if (value_two == 0 && value_one == 0) {
          log_error("Unable to evaluate 0^0");
          return false;
        }
        out->value_.reset(new Int16DataType(pow(value_one, value_two)));
      } else {
        log_error("Unexpected op: %s", GetOpString(op).c_str());
        return false;
      }
      return true;
    }
  } else if (
      one.type_ == DataType::UINT8 || one.type_ == DataType::INT8 ||
      two.type_ == DataType::UINT8 || two.type_ == DataType::INT8) {
    if (!IsSignedDataType(two.type_) && !IsSignedDataType(two.type_)) {
      unsigned char value_one, value_two;
      if (!GetIntegerValue<unsigned char>(one, &value_one) ||
          !GetIntegerValue<unsigned char>(two, &value_two)) {
        return false;
      }
      out->type_ = DataType::UINT8;
      if (op == ArithmeticOperation::ADD) {
        out->value_.reset(new Uint8DataType(value_one + value_two));
      } else if (op == ArithmeticOperation::SUB) {
        out->value_.reset(new Uint8DataType(value_one - value_two));
      } else if (
          op == ArithmeticOperation::MULT ||
          op == ArithmeticOperation::INNER_PRODUCT) {
        out->value_.reset(new Uint8DataType(value_one * value_two));
      } else if (op == ArithmeticOperation::DIV) {
        if (value_two == 0) {
          log_error("Unable to divide by 0");
          return false;
        }
        out->value_.reset(new Uint8DataType(value_one / value_two));
      } else if (op == ArithmeticOperation::MIN) {
        out->value_.reset(new Uint8DataType(min(value_one, value_two)));
      } else if (op == ArithmeticOperation::MAX) {
        out->value_.reset(new Uint8DataType(max(value_one, value_two)));
      } else if (op == ArithmeticOperation::POW) {
        if (value_two == 0 && value_one == 0) {
          log_error("Unable to evaluate 0^0");
          return false;
        }
        out->value_.reset(new Uint8DataType(pow(value_one, value_two)));
      } else {
        log_error("Unexpected op: %s", GetOpString(op).c_str());
        return false;
      }
      return true;
    } else {
      char value_one, value_two;
      if (!GetIntegerValue<char>(one, &value_one) ||
          !GetIntegerValue<char>(two, &value_two)) {
        return false;
      }
      out->type_ = DataType::INT8;
      if (op == ArithmeticOperation::ADD) {
        out->value_.reset(new Int8DataType(value_one + value_two));
      } else if (op == ArithmeticOperation::SUB) {
        out->value_.reset(new Int8DataType(value_one - value_two));
      } else if (
          op == ArithmeticOperation::MULT ||
          op == ArithmeticOperation::INNER_PRODUCT) {
        out->value_.reset(new Int8DataType(value_one * value_two));
      } else if (op == ArithmeticOperation::DIV) {
        if (value_two == 0) {
          log_error("Unable to divide by 0");
          return false;
        }
        out->value_.reset(new Int8DataType(value_one / value_two));
      } else if (op == ArithmeticOperation::MIN) {
        out->value_.reset(new Int8DataType(min(value_one, value_two)));
      } else if (op == ArithmeticOperation::MAX) {
        out->value_.reset(new Int8DataType(max(value_one, value_two)));
      } else if (op == ArithmeticOperation::POW) {
        if (value_two == 0 && value_one == 0) {
          log_error("Unable to evaluate 0^0");
          return false;
        }
        out->value_.reset(new Int8DataType(pow(value_one, value_two)));
      } else {
        log_error("Unexpected op: %s", GetOpString(op).c_str());
        return false;
      }
      return true;
    }
  } else if (
      one.type_ == DataType::UINT4 || one.type_ == DataType::INT4 ||
      two.type_ == DataType::UINT4 || two.type_ == DataType::INT4) {
    if (!IsSignedDataType(two.type_) && !IsSignedDataType(two.type_)) {
      unsigned char value_one, value_two;
      if (!GetIntegerValue<unsigned char>(one, &value_one) ||
          !GetIntegerValue<unsigned char>(two, &value_two)) {
        return false;
      }
      out->type_ = DataType::UINT4;
      if (op == ArithmeticOperation::ADD) {
        const char temp = value_one + value_two;
        out->value_.reset(
            new Uint4DataType(temp > 15 ? temp - 16 : temp));
      } else if (op == ArithmeticOperation::SUB) {
        const char temp = value_one - value_two;
        out->value_.reset(
            new Uint4DataType(temp < 0 ? temp + 16 : temp));
      } else if (
          op == ArithmeticOperation::MULT ||
          op == ArithmeticOperation::INNER_PRODUCT) {
        char temp = value_one * value_two;
        while (temp < 0)
          temp += 16;
        while (temp > 15)
          temp -= 16;
        out->value_.reset(new Uint4DataType(temp));
      } else if (op == ArithmeticOperation::DIV) {
        if (value_two == 0) {
          log_error("Unable to divide by 0");
          return false;
        }
        out->value_.reset(new Uint4DataType(value_one / value_two));
      } else if (op == ArithmeticOperation::MIN) {
        out->value_.reset(new Uint4DataType(min(value_one, value_two)));
      } else if (op == ArithmeticOperation::MAX) {
        out->value_.reset(new Uint4DataType(max(value_one, value_two)));
      } else if (op == ArithmeticOperation::POW) {
        if (value_two == 0 && value_one == 0) {
          log_error("Unable to evaluate 0^0");
          return false;
        }
        char temp = pow(value_one, value_two);
        while (temp < 0)
          temp += 16;
        while (temp > 15)
          temp -= 16;
        out->value_.reset(new Uint4DataType(temp));
      } else {
        log_error("Unexpected op: %s", GetOpString(op).c_str());
        return false;
      }
      return true;
    } else {
      char value_one, value_two;
      if (!GetIntegerValue<char>(one, &value_one) ||
          !GetIntegerValue<char>(two, &value_two)) {
        return false;
      }
      out->type_ = DataType::INT4;
      if (op == ArithmeticOperation::ADD) {
        const char temp = value_one + value_two;
        out->value_.reset(new Int4DataType(
            temp < -8 ? temp + 16 : (temp > 7 ? temp - 16 : temp)));
      } else if (op == ArithmeticOperation::SUB) {
        const char temp = value_one - value_two;
        out->value_.reset(new Int4DataType(
            temp < -8 ? temp + 16 : (temp > 7 ? temp - 16 : temp)));
      } else if (
          op == ArithmeticOperation::MULT ||
          op == ArithmeticOperation::INNER_PRODUCT) {
        char temp = value_one * value_two;
        while (temp < -8)
          temp += 16;
        while (temp > 7)
          temp -= 16;
        out->value_.reset(new Int4DataType(temp));
      } else if (op == ArithmeticOperation::DIV) {
        if (value_two == 0) {
          log_error("Unable to divide by 0");
          return false;
        }
        out->value_.reset(new Int4DataType(value_one / value_two));
      } else if (op == ArithmeticOperation::MIN) {
        out->value_.reset(new Int4DataType(min(value_one, value_two)));
      } else if (op == ArithmeticOperation::MAX) {
        out->value_.reset(new Int4DataType(max(value_one, value_two)));
      } else if (op == ArithmeticOperation::POW) {
        if (value_two == 0 && value_one == 0) {
          log_error("Unable to evaluate 0^0");
          return false;
        }
        char temp = pow(value_one, value_two);
        while (temp < -8)
          temp += 16;
        while (temp > 7)
          temp -= 16;
        out->value_.reset(new Int4DataType(temp));
      } else {
        log_error("Unexpected op: %s", GetOpString(op).c_str());
        return false;
      }
      return true;
    }
  } else if (
      one.type_ == DataType::UINT2 || one.type_ == DataType::INT2 ||
      two.type_ == DataType::UINT2 || two.type_ == DataType::INT2) {
    if (!IsSignedDataType(two.type_) && !IsSignedDataType(two.type_)) {
      unsigned char value_one, value_two;
      if (!GetIntegerValue<unsigned char>(one, &value_one) ||
          !GetIntegerValue<unsigned char>(two, &value_two)) {
        return false;
      }
      out->type_ = DataType::UINT2;
      if (op == ArithmeticOperation::ADD) {
        const char temp = value_one + value_two;
        out->value_.reset(
            new Uint2DataType(temp > 3 ? temp - 4 : temp));
      } else if (op == ArithmeticOperation::SUB) {
        const char temp = value_one - value_two;
        out->value_.reset(
            new Uint2DataType(temp < 0 ? temp + 4 : temp));
      } else if (
          op == ArithmeticOperation::MULT ||
          op == ArithmeticOperation::INNER_PRODUCT) {
        char temp = value_one * value_two;
        while (temp < 0)
          temp += 4;
        while (temp > 3)
          temp -= 4;
        out->value_.reset(new Uint2DataType(temp));
      } else if (op == ArithmeticOperation::DIV) {
        if (value_two == 0) {
          log_error("Unable to divide by 0");
          return false;
        }
        out->value_.reset(new Uint2DataType(value_one / value_two));
      } else if (op == ArithmeticOperation::MIN) {
        out->value_.reset(new Uint2DataType(min(value_one, value_two)));
      } else if (op == ArithmeticOperation::MAX) {
        out->value_.reset(new Uint2DataType(max(value_one, value_two)));
      } else if (op == ArithmeticOperation::POW) {
        if (value_two == 0 && value_one == 0) {
          log_error("Unable to evaluate 0^0");
          return false;
        }
        char temp = pow(value_one, value_two);
        while (temp < 0)
          temp += 4;
        while (temp > 3)
          temp -= 4;
        out->value_.reset(new Int2DataType(temp));
      } else {
        log_error("Unexpected op: %s", GetOpString(op).c_str());
        return false;
      }
      return true;
    } else {
      char value_one, value_two;
      if (!GetIntegerValue<char>(one, &value_one) ||
          !GetIntegerValue<char>(two, &value_two)) {
        return false;
      }
      out->type_ = DataType::INT2;
      if (op == ArithmeticOperation::ADD) {
        const char temp = value_one + value_two;
        out->value_.reset(new Int2DataType(
            temp < -2 ? temp + 4 : (temp > 1 ? temp - 4 : temp)));
      } else if (op == ArithmeticOperation::SUB) {
        const char temp = value_one - value_two;
        out->value_.reset(new Int2DataType(
            temp < -2 ? temp + 4 : (temp > 1 ? temp - 4 : temp)));
      } else if (
          op == ArithmeticOperation::MULT ||
          op == ArithmeticOperation::INNER_PRODUCT) {
        char temp = value_one * value_two;
        while (temp < -2)
          temp += 4;
        while (temp > 1)
          temp -= 4;
        out->value_.reset(new Int2DataType(temp));
      } else if (op == ArithmeticOperation::DIV) {
        if (value_two == 0) {
          log_error("Unable to divide by 0");
          return false;
        }
        out->value_.reset(new Int2DataType(value_one / value_two));
      } else if (op == ArithmeticOperation::MIN) {
        out->value_.reset(new Int2DataType(min(value_one, value_two)));
      } else if (op == ArithmeticOperation::MAX) {
        out->value_.reset(new Int2DataType(max(value_one, value_two)));
      } else if (op == ArithmeticOperation::POW) {
        if (value_two == 0 && value_one == 0) {
          log_error("Unable to evaluate 0^0");
          return false;
        }
        char temp = pow(value_one, value_two);
        while (temp < -2)
          temp += 4;
        while (temp > 1)
          temp -= 4;
        out->value_.reset(new Int2DataType(temp));
      } else {
        log_error("Unexpected op: %s", GetOpString(op).c_str());
        return false;
      }
      return true;
    }
  } else {
    bool value_one, value_two;
    if (!GetIntegerValue<bool>(one, &value_one) ||
        !GetIntegerValue<bool>(two, &value_two)) {
      return false;
    }
    out->type_ = DataType::BOOL;
    if (op == ArithmeticOperation::ADD) {
      out->value_.reset(new BoolDataType(value_one != value_two));
    } else if (op == ArithmeticOperation::SUB) {
      out->value_.reset(new BoolDataType(value_one != value_two));
    } else if (
        op == ArithmeticOperation::MULT ||
        op == ArithmeticOperation::INNER_PRODUCT) {
      out->value_.reset(new BoolDataType(value_one && value_two));
    } else if (op == ArithmeticOperation::DIV) {
      if (value_two == 0) {
        log_error("Unable to divide by 0");
        return false;
      }
      out->value_.reset(new BoolDataType(value_one));
    } else if (op == ArithmeticOperation::MIN) {
      out->value_.reset(new BoolDataType(value_one && value_two));
    } else if (op == ArithmeticOperation::MAX) {
      out->value_.reset(new BoolDataType(value_one || value_two));
    } else if (op == ArithmeticOperation::POW) {
      if (value_two == 0 && value_one == 0) {
        log_error("Unable to evaluate 0^0");
        return false;
      }
      out->value_.reset(new BoolDataType(value_one));
    } else {
      log_error("Unexpected op: %s", GetOpString(op).c_str());
      return false;
    }
  }

  return true;
}

bool MergeValuesViaOperator(
    const CircuitOperation & op,
    const GenericValue & one,
    const GenericValue & two,
    GenericValue * out) {
  switch (op) {
    case CircuitOperation::AND: {
      return MergeValuesViaOperator(
          BooleanOperation::AND, one, two, out);
    }
    case CircuitOperation::NAND: {
      return MergeValuesViaOperator(
          BooleanOperation::NAND, one, two, out);
    }
    case CircuitOperation::OR: {
      return MergeValuesViaOperator(
          BooleanOperation::OR, one, two, out);
    }
    case CircuitOperation::NOR: {
      return MergeValuesViaOperator(
          BooleanOperation::NOR, one, two, out);
    }
    case CircuitOperation::XOR: {
      return MergeValuesViaOperator(
          BooleanOperation::XOR, one, two, out);
    }
    case CircuitOperation::EQ: {
      return MergeValuesViaOperator(
          BooleanOperation::EQ, one, two, out);
    }
    case CircuitOperation::GT: {
      return MergeValuesViaOperator(
          BooleanOperation::GT, one, two, out);
    }
    case CircuitOperation::GTE: {
      return MergeValuesViaOperator(
          BooleanOperation::GTE, one, two, out);
    }
    case CircuitOperation::LT: {
      return MergeValuesViaOperator(
          BooleanOperation::LT, one, two, out);
    }
    case CircuitOperation::LTE: {
      return MergeValuesViaOperator(
          BooleanOperation::LTE, one, two, out);
    }
    case CircuitOperation::ADD: {
      return MergeValuesViaOperator(
          ArithmeticOperation::ADD, one, two, out);
    }
    case CircuitOperation::SUB: {
      return MergeValuesViaOperator(
          ArithmeticOperation::SUB, one, two, out);
    }
    case CircuitOperation::MULT: {
      return MergeValuesViaOperator(
          ArithmeticOperation::MULT, one, two, out);
    }
    case CircuitOperation::DIV: {
      return MergeValuesViaOperator(
          ArithmeticOperation::DIV, one, two, out);
    }
    case CircuitOperation::POW: {
      return MergeValuesViaOperator(
          ArithmeticOperation::POW, one, two, out);
    }
    case CircuitOperation::MIN: {
      return MergeValuesViaOperator(
          ArithmeticOperation::MIN, one, two, out);
    }
    case CircuitOperation::MAX: {
      return MergeValuesViaOperator(
          ArithmeticOperation::MAX, one, two, out);
    }
    default: {
      log_error(
          "Unsupported operation in MergeValuesViaOperator(): %s",
          Itoa(static_cast<int>(op)).c_str());
      return false;
    }
  }

  return true;
}

bool MergeValuesViaOperator(
    const CircuitOperation & op,
    const bool & one,
    const bool & two,
    bool * out) {
  switch (op) {
    case CircuitOperation::AND: {
      *out = one && two;
      return true;
    }
    case CircuitOperation::NAND: {
      *out = !(one && two);
      return true;
    }
    case CircuitOperation::OR: {
      *out = one || two;
      return true;
    }
    case CircuitOperation::NOR: {
      *out = !(one || two);
      return true;
    }
    case CircuitOperation::XOR: {
      *out = one != two;
      return true;
    }
    case CircuitOperation::EQ: {
      *out = one == two;
      return true;
    }
    case CircuitOperation::GT: {
      *out = one && !two;
      return true;
    }
    case CircuitOperation::GTE: {
      *out = one || !two;
      return true;
    }
    case CircuitOperation::LT: {
      *out = !one && two;
      return true;
    }
    case CircuitOperation::LTE: {
      *out = !one || two;
      return true;
    }
    default: {
      log_error(
          "Unsupported operation in MergeValuesViaOperator(): %s",
          Itoa(static_cast<int>(op)).c_str());
      return false;
    }
  }

  return true;
}

bool ApplyOperator(
    const OperationHolder & op,
    const GenericValue & one,
    GenericValue * out) {
  if (op.type_ == OperationType::MATH)
    return ApplyOperator(op.math_op_, one, out);
  if (op.type_ == OperationType::BOOLEAN)
    return ApplyOperator(op.gate_op_, one, out);
  if (op.type_ == OperationType::ARITHMETIC)
    return ApplyOperator(op.arithmetic_op_, one, out);
  // ComparisonOperation don't have any single-term operators.
  return false;
}

bool ApplyOperator(
    const MathOperation & op,
    const GenericValue & one,
    GenericValue * out) {
  log_fatal("This method not yet implemented.");
  return false;
}

bool ApplyOperator(
    const BooleanOperation & op,
    const GenericValue & one,
    GenericValue * out) {
  if (op == BooleanOperation::IDENTITY) {
    *out = one;
  } else if (op == BooleanOperation::NOT) {
    if (one.type_ == DataType::DOUBLE)
      return false;
    out->type_ = one.type_;

    vector<unsigned char> bytes = GetTwosComplementByteString(one);
    vector<unsigned char> result(bytes.size());
    for (size_t i = 0; i < bytes.size(); ++i) {
      result[i] = ~bytes[i];
    }
    if (IsStringDataType(one)) {
      out->SetStringGenericValue(string(result.begin(), result.end()));
    } else if (IsSignedDataType(one.type_)) {
      if (one.type_ == DataType::INT2) {
        out->value_.reset(new Int2DataType(
            ByteStringToValue<char>(result) & (unsigned char)3));
      } else if (one.type_ == DataType::INT4) {
        out->value_.reset(new Int4DataType(
            ByteStringToValue<char>(result) & (unsigned char)15));
      } else if (one.type_ == DataType::INT8) {
        out->value_.reset(
            new Int8DataType(ByteStringToValue<char>(result)));
      } else if (one.type_ == DataType::INT16) {
        out->value_.reset(
            new Int16DataType(ByteStringToValue<short>(result)));
      } else if (one.type_ == DataType::INT32) {
        out->value_.reset(
            new Int32DataType(ByteStringToValue<int32_t>(result)));
      } else if (one.type_ == DataType::INT64) {
        out->value_.reset(
            new Int64DataType(ByteStringToValue<int64_t>(result)));
      } else {
        log_error(
            "Operator is not a single-term op: %s",
            GetOpString(op).c_str());
        return false;
      }
    } else {
      if (one.type_ == DataType::BOOL) {
        out->value_.reset(new BoolDataType(
            ByteStringToValue<unsigned char>(result) &
            (unsigned char)1));
      } else if (one.type_ == DataType::UINT2) {
        out->value_.reset(new Uint2DataType(
            ByteStringToValue<unsigned char>(result) &
            (unsigned char)3));
      } else if (one.type_ == DataType::UINT4) {
        out->value_.reset(new Uint4DataType(
            ByteStringToValue<unsigned char>(result) &
            (unsigned char)15));
      } else if (one.type_ == DataType::UINT8) {
        out->value_.reset(new Uint8DataType(
            ByteStringToValue<unsigned char>(result)));
      } else if (one.type_ == DataType::UINT16) {
        out->value_.reset(new Uint16DataType(
            ByteStringToValue<unsigned short>(result)));
      } else if (one.type_ == DataType::UINT32) {
        out->value_.reset(
            new Uint32DataType(ByteStringToValue<uint32_t>(result)));
      } else if (one.type_ == DataType::UINT64) {
        out->value_.reset(
            new Uint64DataType(ByteStringToValue<uint64_t>(result)));
      } else if (one.type_ == DataType::SLICE) {
        out->value_.reset(
            new SliceDataType(ByteStringToValue<slice>(result)));
      } else {
        log_error(
            "Operator is not a single-term op: %s",
            GetOpString(op).c_str());
        return false;
      }
    }

    return true;
  }

  log_error(
      "Operator is not a single-term op: %s", GetOpString(op).c_str());
  return false;
}

bool ApplyOperator(
    const ArithmeticOperation & op,
    const GenericValue & one,
    GenericValue * out) {
  if (out == nullptr || !IsNumericDataType(one))
    return false;

  if (one.type_ == DataType::DOUBLE) {
    out->type_ = DataType::DOUBLE;
    const double * value =
        &GetValue<double>(*((DoubleDataType *)one.value_.get()));
    if (op == ArithmeticOperation::ABS) {
      if (*value < 0.0)
        out->value_.reset(new DoubleDataType(-1.0 * (*value)));
      else
        out->value_.reset(new DoubleDataType(*value));
    } else if (op == ArithmeticOperation::FLIP_SIGN) {
      out->value_.reset(new DoubleDataType(-1.0 * (*value)));
    } else if (op == ArithmeticOperation::FACTORIAL) {
      if (*value < 0.0 || ceil(*value) != *value) {
        log_error(
            "Unable to do factorial of negative value: %s",
            Itoa(*value).c_str());
        return false;
      }
      uint64_t int_value = (uint64_t)*value;
      double temp = 1.0;
      for (int i = 2; i <= int_value; ++i) {
        temp *= i;
      }
      out->value_.reset(new DoubleDataType(temp));
    } else if (op == ArithmeticOperation::SQRT) {
      if (*value < 0.0) {
        log_error(
            "Unable to take square root of negative value: %s",
            Itoa(*value).c_str());
        return false;
      }
      out->value_.reset(new DoubleDataType(sqrt(*value)));
    } else {
      log_error(
          "Operator is not a single-term op: %s",
          GetOpString(op).c_str());
      return false;
    }
  } else if (IsSignedDataType(one.type_)) {
    int64_t value;
    if (!GetSignedIntegerValue(one, &value))
      return false;
    if (op == ArithmeticOperation::ABS ||
        (op == ArithmeticOperation::FLIP_SIGN && value <= 0)) {
      if (value >= 0) {
        *out = one;
        return true;
      }
      value *= -1;
      // Only interesting/problematic case is if value = -2^(N/2), where
      // N = num bits in type_. In this case, convention (II) says to
      // not do anything (e.g. |-8| = 8 = 1000 -> -8 as INT4.
      if (one.type_ == DataType::INT64) {
        out->type_ = DataType::INT64;
        out->value_.reset(new Int64DataType(value));
      } else if (one.type_ == DataType::INT32) {
        if (value == INT64_MIN) {
          *out = one;
          return true;
        }
        out->type_ = DataType::INT32;
        out->value_.reset(new Int32DataType(value));
      } else if (one.type_ == DataType::INT16) {
        if (value == SHRT_MAX + 1) {
          *out = one;
          return true;
        }
        out->type_ = DataType::INT16;
        out->value_.reset(new Int16DataType(value));
      } else if (one.type_ == DataType::INT8) {
        if (value == CHAR_MAX + 1) {
          *out = one;
          return true;
        }
        out->type_ = DataType::INT8;
        out->value_.reset(new Int8DataType(value));
      } else if (one.type_ == DataType::INT4) {
        if (value == 8) {
          *out = one;
          return true;
        }
        out->type_ = DataType::INT4;
        out->value_.reset(new Int4DataType(value));
      } else { // INT2
        if (value == 2) {
          *out = one;
          return true;
        }
        out->type_ = DataType::INT2;
        out->value_.reset(new Int2DataType(value));
      }
    } else if (op == ArithmeticOperation::FLIP_SIGN) {
      value *= -1;
      if (one.type_ == DataType::INT64) {
        out->type_ = DataType::INT64;
        out->value_.reset(new Int64DataType(value));
      } else if (one.type_ == DataType::INT32) {
        out->type_ = DataType::INT32;
        out->value_.reset(new Int32DataType(value));
      } else if (one.type_ == DataType::INT16) {
        out->type_ = DataType::INT16;
        out->value_.reset(new Int16DataType(value));
      } else if (one.type_ == DataType::INT8) {
        out->type_ = DataType::INT8;
        out->value_.reset(new Int8DataType(value));
      } else if (one.type_ == DataType::INT4) {
        out->type_ = DataType::INT4;
        out->value_.reset(new Int4DataType(value));
      } else { // INT2
        out->type_ = DataType::INT2;
        out->value_.reset(new Int2DataType(value));
      }
    } else if (op == ArithmeticOperation::FACTORIAL) {
      if (value < 0 || value > 20) {
        log_error("Unable to take factorial of negative value.");
        return false;
      }
      int64_t temp = 1;
      for (int i = 2; i <= value; ++i) {
        temp *= i;
      }
      if (one.type_ == DataType::INT64) {
        out->type_ = DataType::INT64;
        out->value_.reset(new Int64DataType(temp));
      } else if (one.type_ == DataType::INT32) {
        out->type_ = DataType::INT32;
        out->value_.reset(new Int32DataType(temp));
      } else if (one.type_ == DataType::INT16) {
        out->type_ = DataType::INT16;
        out->value_.reset(new Int16DataType(temp));
      } else if (one.type_ == DataType::INT8) {
        out->type_ = DataType::INT8;
        out->value_.reset(new Int8DataType(temp));
      } else if (one.type_ == DataType::INT4) {
        char temp_cast = temp;
        while (temp_cast > 7)
          temp_cast -= 16;
        out->type_ = DataType::INT4;
        out->value_.reset(new Int4DataType(temp_cast));
      } else { // INT2
        char temp_cast = temp;
        while (temp_cast > 1)
          temp_cast -= 4;
        out->type_ = DataType::INT2;
        out->value_.reset(new Int2DataType(temp_cast));
      }
    } else if (op == ArithmeticOperation::SQRT) {
      if (value < 0) {
        log_error(
            "Unable to take square root of negative value: %s",
            Itoa(value).c_str());
        return false;
      }
      value = sqrt(value);
      if (one.type_ == DataType::INT64) {
        out->type_ = DataType::INT64;
        out->value_.reset(new Int64DataType(value));
      } else if (one.type_ == DataType::INT32) {
        out->type_ = DataType::INT32;
        out->value_.reset(new Int32DataType(value));
      } else if (one.type_ == DataType::INT16) {
        out->type_ = DataType::INT16;
        out->value_.reset(new Int16DataType(value));
      } else if (one.type_ == DataType::INT8) {
        out->type_ = DataType::INT8;
        out->value_.reset(new Int8DataType(value));
      } else if (one.type_ == DataType::INT4) {
        out->type_ = DataType::INT4;
        out->value_.reset(new Int4DataType(value));
      } else { // INT2
        out->type_ = DataType::INT2;
        out->value_.reset(new Int2DataType(value));
      }
    } else {
      log_error(
          "Operator is not a single-term op: %s",
          GetOpString(op).c_str());
      return false;
    }
  } else {
    if (op == ArithmeticOperation::ABS) {
      *out = one;
      return true;
    } else if (op == ArithmeticOperation::FLIP_SIGN) {
      log_error("Unable to flip sign of unsigned DataType");
      return false;
    } else if (op == ArithmeticOperation::FACTORIAL) {
      if (one.type_ == DataType::BOOL) {
        *out = GenericValue(true);
        return true;
      }
      uint64_t orig_value;
      if (!GetUnsignedIntegerValue(one, &orig_value))
        return false;
      if (orig_value > 20) {
        log_error("Unable to take factorial of values larger than 20.");
        return false;
      }
      uint64_t temp = 1;
      for (int i = 2; i <= orig_value; ++i) {
        temp *= i;
      }
      if (one.type_ == DataType::UINT64) {
        out->type_ = DataType::UINT64;
        out->value_.reset(new Uint64DataType(temp));
      } else if (one.type_ == DataType::UINT32) {
        out->type_ = DataType::UINT32;
        out->value_.reset(new Uint32DataType(temp));
      } else if (one.type_ == DataType::UINT16) {
        out->type_ = DataType::UINT16;
        out->value_.reset(new Uint16DataType(temp));
      } else if (one.type_ == DataType::UINT8) {
        out->type_ = DataType::UINT8;
        out->value_.reset(new Uint8DataType(temp));
      } else if (one.type_ == DataType::UINT4) {
        char temp_cast = temp;
        while (temp_cast > 15)
          temp_cast -= 16;
        out->type_ = DataType::UINT4;
        out->value_.reset(new Uint4DataType(temp_cast));
      } else { // UINT2
        char temp_cast = temp;
        while (temp_cast > 3)
          temp_cast -= 4;
        out->type_ = DataType::UINT2;
        out->value_.reset(new Uint2DataType(temp_cast));
      }
    } else if (op == ArithmeticOperation::SQRT) {
      if (one.type_ == DataType::BOOL) {
        *out = one;
        return true;
      }
      uint64_t value;
      if (!GetUnsignedIntegerValue(one, &value))
        return false;
      value = sqrt(value);
      if (one.type_ == DataType::UINT64) {
        out->type_ = DataType::UINT64;
        out->value_.reset(new Uint64DataType(value));
      } else if (one.type_ == DataType::UINT32) {
        out->type_ = DataType::UINT32;
        out->value_.reset(new Uint32DataType(value));
      } else if (one.type_ == DataType::UINT16) {
        out->type_ = DataType::UINT16;
        out->value_.reset(new Uint16DataType(value));
      } else if (one.type_ == DataType::UINT8) {
        out->type_ = DataType::UINT8;
        out->value_.reset(new Uint8DataType(value));
      } else if (one.type_ == DataType::UINT4) {
        out->type_ = DataType::UINT4;
        out->value_.reset(new Uint4DataType(value));
      } else { // UINT2
        out->type_ = DataType::UINT2;
        out->value_.reset(new Uint2DataType(value));
      }
    } else {
      log_error(
          "Operator is not a single-term op: %s",
          GetOpString(op).c_str());
      return false;
    }
  }
  return true;
}

bool ApplyOperator(
    const CircuitOperation & op,
    const GenericValue & one,
    GenericValue * out) {
  switch (op) {
    case CircuitOperation::IDENTITY: {
      return ApplyOperator(BooleanOperation::IDENTITY, one, out);
    }
    case CircuitOperation::NOT: {
      return ApplyOperator(BooleanOperation::NOT, one, out);
    }
    case CircuitOperation::SELF: {
      return ApplyOperator(ArithmeticOperation::IDENTITY, one, out);
    }
    case CircuitOperation::ABS: {
      return ApplyOperator(ArithmeticOperation::ABS, one, out);
    }
    case CircuitOperation::FLIP_SIGN: {
      return ApplyOperator(ArithmeticOperation::FLIP_SIGN, one, out);
    }
    case CircuitOperation::FACTORIAL: {
      return ApplyOperator(ArithmeticOperation::FACTORIAL, one, out);
    }
    case CircuitOperation::SQRT: {
      return ApplyOperator(ArithmeticOperation::SQRT, one, out);
    }
    default: {
      log_error(
          "Unsupported operation in ApplyOperator(): %s",
          Itoa(static_cast<int>(op)).c_str());
      return false;
    }
  }

  return true;
}

bool GetSignedIntegerValue(
    const GenericValue & input, int64_t * output) {
  if (!IsIntegerDataType(input) || input.type_ == DataType::UINT64 ||
      input.type_ == DataType::SLICE) {
    return false;
  }

  if (input.type_ == DataType::INT64) {
    *output = GetValue<int64_t>(*((Int64DataType *)input.value_.get()));
  } else if (input.type_ == DataType::UINT32) {
    *output =
        GetValue<uint32_t>(*((Uint32DataType *)input.value_.get()));
  } else if (input.type_ == DataType::INT32) {
    *output = GetValue<int32_t>(*((Int32DataType *)input.value_.get()));
  } else if (input.type_ == DataType::UINT16) {
    *output = GetValue<unsigned short>(
        *((Uint16DataType *)input.value_.get()));
  } else if (input.type_ == DataType::INT16) {
    *output = GetValue<short>(*((Int16DataType *)input.value_.get()));
  } else if (input.type_ == DataType::UINT8) {
    *output =
        GetValue<unsigned char>(*((Uint8DataType *)input.value_.get()));
  } else if (input.type_ == DataType::INT8) {
    *output = GetValue<char>(*((Int8DataType *)input.value_.get()));
  } else if (input.type_ == DataType::UINT4) {
    *output =
        GetValue<unsigned char>(*((Uint4DataType *)input.value_.get()));
  } else if (input.type_ == DataType::INT4) {
    *output = GetValue<char>(*((Int4DataType *)input.value_.get()));
  } else if (input.type_ == DataType::UINT2) {
    *output =
        GetValue<unsigned char>(*((Uint2DataType *)input.value_.get()));
  } else if (input.type_ == DataType::INT2) {
    *output = GetValue<char>(*((Int2DataType *)input.value_.get()));
  } else if (input.type_ == DataType::BOOL) {
    *output = GetValue<bool>(*((BoolDataType *)input.value_.get()));
  } else {
    log_error(
        "Unrecognized type: %s",
        Itoa(static_cast<int>(input.type_)).c_str());
    return false;
  }
  return true;
}

bool GetUnsignedIntegerValue(
    const GenericValue & input, uint64_t * output) {
  if (!IsIntegerDataType(input)) {
    return false;
  }

  // Check for negative.
  if (IsSignedDataType(input.type_)) {
    int64_t temp;
    if (!GetSignedIntegerValue(input, &temp) || temp < 0)
      return false;
    *output = (uint64_t)temp;
    return true;
  }

  if (input.type_ == DataType::UINT64) {
    *output =
        GetValue<uint64_t>(*((Uint64DataType *)input.value_.get()));
  } else if (input.type_ == DataType::UINT32) {
    *output =
        GetValue<uint32_t>(*((Uint32DataType *)input.value_.get()));
  } else if (input.type_ == DataType::UINT16) {
    *output = GetValue<unsigned short>(
        *((Uint16DataType *)input.value_.get()));
  } else if (input.type_ == DataType::UINT8) {
    *output =
        GetValue<unsigned char>(*((Uint8DataType *)input.value_.get()));
  } else if (input.type_ == DataType::UINT4) {
    *output =
        GetValue<unsigned char>(*((Uint4DataType *)input.value_.get()));
  } else if (input.type_ == DataType::UINT2) {
    *output =
        GetValue<unsigned char>(*((Uint2DataType *)input.value_.get()));
  } else if (input.type_ == DataType::BOOL) {
    *output = GetValue<bool>(*((BoolDataType *)input.value_.get()));
  } else {
    log_error(
        "Unrecognized type: %s",
        Itoa(static_cast<int>(input.type_)).c_str());
    return false;
  }
  return true;
}

bool StripEnclosingGroupingSymbols(
    const string & input, string * output) {
  *output = input;

  for (int i = 0; i < kNumGroupingSymbols - 1; ++i) {
    if (HasPrefixString(input, string(&kGroupingSymbols[i].first, 1))) {
      if (!HasSuffixString(
              input, string(&kGroupingSymbols[i].second, 1))) {
        return false;
      }
      *output = input.substr(1, input.length() - 2);
      return true;
    }
  }
  return true;
}

bool GetClosingParentheses(const string & input, size_t * closing_pos) {
  if (input.empty() || input.substr(0, 1) != "(")
    return false;
  int current_count = 1;
  for (int i = 1; i < input.length(); ++i) {
    if (input.substr(i, 1) == "(")
      current_count++;
    if (input.substr(i, 1) == ")")
      current_count--;
    if (current_count == 0) {
      *closing_pos = i;
      return true;
    }
  }
  return false;
}

bool GetClosingSymbol(
    const string & input,
    const pair<char, char> grouping_symbols,
    size_t * closing_pos) {
  if (input.empty() || input.at(0) != grouping_symbols.first)
    return false;

  // Handle the special case that the left and right grouping symbols
  // are the same (e.g. for absolute value "|"): In this case, just
  // return the next instance of the symbol.
  if (grouping_symbols.first == grouping_symbols.second) {
    *closing_pos = input.find(grouping_symbols.first, 1);
    return *closing_pos != string::npos;
  }

  int current_count = 1;
  for (int i = 1; i < input.length(); ++i) {
    if (input.at(i) == grouping_symbols.first)
      current_count++;
    if (input.at(i) == grouping_symbols.second)
      current_count--;
    if (current_count == 0) {
      *closing_pos = i;
      return true;
    }
  }

  return false;
}

bool CountOpenCloseParentheses(
    const bool force_consistency,
    const string & input,
    int * num_unmatched) {
  if (num_unmatched == nullptr ||
      (force_consistency && *num_unmatched < 0)) {
    log_fatal("Mismatching parentheses");
  }

  for (string::const_iterator itr = input.begin(); itr != input.end();
       ++itr) {
    if (*itr == '(') {
      ++(*num_unmatched);
    } else if (*itr == ')') {
      --(*num_unmatched);
      if (*num_unmatched < 0 && force_consistency)
        return false;
    }
  }

  return true;
}

string GetExpressionString(const Expression & expression) {
  // Self-Operations.
  if (expression.op_ == MathOperation::IDENTITY) {
    if (expression.is_constant_) {
      return Itoa(expression.value_);
    } else {
      return expression.var_name_;
    }
    // 2-term Operations.
  } else if (
      expression.op_ == MathOperation::ADD ||
      expression.op_ == MathOperation::SUB ||
      expression.op_ == MathOperation::MULT ||
      expression.op_ == MathOperation::DIV ||
      expression.op_ == MathOperation::POW ||
      expression.op_ == MathOperation::MODULUS ||
      expression.op_ == MathOperation::INC_GAMMA_FN ||
      expression.op_ == MathOperation::REG_INC_GAMMA_FN ||
      expression.op_ == MathOperation::EQ ||
      expression.op_ == MathOperation::FLOAT_EQ ||
      expression.op_ == MathOperation::GT ||
      expression.op_ == MathOperation::GTE ||
      expression.op_ == MathOperation::LT ||
      expression.op_ == MathOperation::LTE) {
    if (expression.subterm_one_ == nullptr ||
        expression.subterm_two_ == nullptr) {
      return "Unable to perform add/mult/pow: one of the subterms is "
             "null.";
    }
    if (expression.op_ == MathOperation::ADD) {
      return "(" + GetExpressionString(*expression.subterm_one_) +
          " + " + GetExpressionString(*expression.subterm_two_) + ")";
    } else if (expression.op_ == MathOperation::INC_GAMMA_FN) {
      return "IncGamma(" +
          GetExpressionString(*expression.subterm_one_) + ", " +
          GetExpressionString(*expression.subterm_two_) + ")";
    } else if (expression.op_ == MathOperation::REG_INC_GAMMA_FN) {
      return "RegIncGamma(" +
          GetExpressionString(*expression.subterm_one_) + ", " +
          GetExpressionString(*expression.subterm_two_) + ")";
    } else if (expression.op_ == MathOperation::SUB) {
      return "(" + GetExpressionString(*expression.subterm_one_) +
          " - " + GetExpressionString(*expression.subterm_two_) + ")";
    } else if (expression.op_ == MathOperation::MULT) {
      return "(" + GetExpressionString(*expression.subterm_one_) +
          " * " + GetExpressionString(*expression.subterm_two_) + ")";
    } else if (expression.op_ == MathOperation::DIV) {
      return "(" + GetExpressionString(*expression.subterm_one_) +
          " / " + GetExpressionString(*expression.subterm_two_) + ")";
    } else if (expression.op_ == MathOperation::POW) {
      return "(" + GetExpressionString(*expression.subterm_one_) +
          ")^(" + GetExpressionString(*expression.subterm_two_) + ")";
    } else if (expression.op_ == MathOperation::MODULUS) {
      return "(" + GetExpressionString(*expression.subterm_one_) +
          ")%(" + GetExpressionString(*expression.subterm_two_) + ")";
    } else if (expression.op_ == MathOperation::EQ) {
      return "(" + GetExpressionString(*expression.subterm_one_) +
          ")==(" + GetExpressionString(*expression.subterm_two_) + ")";
    } else if (expression.op_ == MathOperation::FLOAT_EQ) {
      return "(" + GetExpressionString(*expression.subterm_one_) +
          ")~=(" + GetExpressionString(*expression.subterm_two_) + ")";
    } else if (expression.op_ == MathOperation::GT) {
      return "(" + GetExpressionString(*expression.subterm_one_) +
          ")>(" + GetExpressionString(*expression.subterm_two_) + ")";
    } else if (expression.op_ == MathOperation::GTE) {
      return "(" + GetExpressionString(*expression.subterm_one_) +
          ")>=(" + GetExpressionString(*expression.subterm_two_) + ")";
    } else if (expression.op_ == MathOperation::LT) {
      return "(" + GetExpressionString(*expression.subterm_one_) +
          ")<(" + GetExpressionString(*expression.subterm_two_) + ")";
    } else if (expression.op_ == MathOperation::LTE) {
      return "(" + GetExpressionString(*expression.subterm_one_) +
          ")<=(" + GetExpressionString(*expression.subterm_two_) + ")";
    }
    // 1-term Operations.
  } else if (
      expression.op_ == MathOperation::TAN ||
      expression.op_ == MathOperation::SIN ||
      expression.op_ == MathOperation::COS ||
      expression.op_ == MathOperation::LOG ||
      expression.op_ == MathOperation::EXP ||
      expression.op_ == MathOperation::FACTORIAL ||
      expression.op_ == MathOperation::GAMMA_FN ||
      expression.op_ == MathOperation::PHI_FN ||
      expression.op_ == MathOperation::ABS ||
      expression.op_ == MathOperation::INDICATOR) {
    if (expression.subterm_one_ == nullptr) {
      return "Unable to perform evaluation: subterm_one_ is null";
    }
    if (expression.subterm_two_ != nullptr) {
      return "Unable to perform evaluation: subterm_two_ is non-null";
    }
    if (expression.op_ == MathOperation::ABS) {
      return "|" + GetExpressionString(*expression.subterm_one_) + "|";
    } else if (expression.op_ == MathOperation::GAMMA_FN) {
      return "Gamma(" + GetExpressionString(*expression.subterm_one_) +
          ")";
    } else if (expression.op_ == MathOperation::PHI_FN) {
      return "Phi(" + GetExpressionString(*expression.subterm_one_) +
          ")";
    } else if (expression.op_ == MathOperation::EXP) {
      return "exp(" + GetExpressionString(*expression.subterm_one_) +
          ")";
    } else if (expression.op_ == MathOperation::FACTORIAL) {
      return "(" + GetExpressionString(*expression.subterm_one_) + ")!";
    } else if (expression.op_ == MathOperation::LOG) {
      return "log(" + GetExpressionString(*expression.subterm_one_) +
          ")";
    } else if (expression.op_ == MathOperation::SQRT) {
      return "sqrt(" + GetExpressionString(*expression.subterm_one_) +
          ")";
    } else if (expression.op_ == MathOperation::SIN) {
      return "sin(" + GetExpressionString(*expression.subterm_one_) +
          ")";
    } else if (expression.op_ == MathOperation::COS) {
      return "cos(" + GetExpressionString(*expression.subterm_one_) +
          ")";
    } else if (expression.op_ == MathOperation::TAN) {
      return "tan(" + GetExpressionString(*expression.subterm_one_) +
          ")";
    } else if (expression.op_ == MathOperation::INDICATOR) {
      return "I(" + GetExpressionString(*expression.subterm_one_) + ")";
    }
  }
  return "ERROR: Unsupported operation: " +
      Itoa(static_cast<int>(expression.op_));
}

string GetDataHolderString(const DataHolder & data) {
  if (data.type_ != StatsDataType::DATA_TYPE_STRING &&
      data.type_ != StatsDataType::DATA_TYPE_NUMERIC) {
    return "";
  }

  if (data.type_ == StatsDataType::DATA_TYPE_STRING) {
    return data.name_;
  }

  return Itoa(data.value_);
}

string GetDataHolderString(
    const DataHolder & data, const bool use_name, const int precision) {
  if (data.type_ != StatsDataType::DATA_TYPE_STRING &&
      data.type_ != StatsDataType::DATA_TYPE_NUMERIC) {
    return "";
  }

  if (data.type_ == StatsDataType::DATA_TYPE_STRING ||
      (use_name && !data.name_.empty())) {
    return data.name_;
  }

  return Itoa(data.value_, precision);
}

string
GetDataHolderString(const DataHolder & data, const int precision) {
  return GetDataHolderString(data, false, precision);
}

// DEPRECATED.
void RewriteVariables(const string & var_name, string * term) {
  if (term == nullptr)
    return;
  size_t var_pos = term->find(var_name);
  size_t chars_processed = 0;
  while (var_pos != string::npos) {
    chars_processed += var_pos + 1;
    int numeric_tester;
    bool updated_formula = false;
    if (chars_processed - 1 > var_name.length()) {
      if (term->substr(
              chars_processed - 1 - var_name.length(),
              var_name.length()) == var_name) {
        *term = term->substr(0, chars_processed - 1) + "*" +
            term->substr(chars_processed - 1);
        chars_processed += 1;
        updated_formula = true;
      }
    }
    if (!updated_formula && chars_processed > 1) {
      if (Stoi(term->substr(chars_processed - 2, 1), &numeric_tester)) {
        *term = term->substr(0, chars_processed - 1) + "*" +
            term->substr(chars_processed - 1);
        chars_processed += 1;
      }
    }
    if (chars_processed >= term->length())
      break;
    var_pos = term->substr(chars_processed).find(var_name);
  }
}

/* DEPRECATED
bool GetLinearTerms(
    const string& input, bool is_first_try, const string& current_term,
    vector<pair<string, bool>>* terms) {
  if (input.empty()) {
    if (!current_term.empty()) {
      terms->push_back(make_pair(current_term, false));
    }
    return true;
  }
  // Don't try to split around a leading minus sign.
  if (is_first_try && input.substr(0, 1) == "-") {
    return GetLinearTerms(input.substr(1), false, "-", terms);
  }
  size_t add_pos = input.find("+");
  size_t neg_pos = input.find("-");
  if (neg_pos < add_pos) add_pos = neg_pos;
  if (add_pos == string::npos) {
    terms->push_back(make_pair(input, false));
    return true;
  }

  // If addition sign lies inside of parentheses, it isn't the addition
  // sign we're looking for.
  size_t parentheses_pos = input.find("(");
  if (parentheses_pos != string::npos && parentheses_pos < add_pos) {
    const string after_parentheses = input.substr(parentheses_pos);
    size_t closing_pos;
    if (!GetClosingParentheses(after_parentheses, &closing_pos)) {
      cout << "ERROR: Unable to GetClosingParentheses for: '"
           << after_parentheses << "'" << endl;
      return false;
    }
    add_pos = after_parentheses.find("+");
    neg_pos = after_parentheses.find("-");
    if (neg_pos < add_pos) add_pos = neg_pos;
    return GetLinearTerms(
        input.substr(parentheses_pos + closing_pos + 1), false,
        (current_term + input.substr(0, parentheses_pos) +
         after_parentheses.substr(0, closing_pos + 1)),
        terms);
  }
  terms->push_back(make_pair(
      current_term + input.substr(0, add_pos), add_pos != neg_pos));
  return GetLinearTerms(input.substr(add_pos + 1), false, "", terms);
}
*/

bool GetAdditiveTerms(
    const string & input,
    const string & current_term,
    vector<pair<string, int>> * terms) {
  vector<pair<char, int>> split_chars(2);
  split_chars[0] = make_pair('+', 1);
  split_chars[1] = make_pair('-', 1);

  // Don't try to split around a leading minus sign.
  if (!input.empty() && input.at(0) == '-') {
    return GetTerms<char>(
        false, input.substr(1), "-", split_chars, terms);
  }

  return GetTerms<char>(false, input, "", split_chars, terms);
}

bool GetMultiplicativeTerms(
    const string & input,
    const string & current_term,
    vector<pair<string, int>> * terms) {
  vector<pair<char, int>> split_chars(2);
  split_chars[0] = make_pair('*', 1);
  split_chars[1] = make_pair('/', 1);

  return GetTerms<char>(false, input, "", split_chars, terms);
}

bool GetArgumentTerms(
    const string & input,
    const string & current_term,
    vector<pair<string, int>> * terms) {
  vector<pair<char, int>> split_chars(1, make_pair(',', 1));
  return GetTerms<char>(false, input, "", split_chars, terms);
}

bool GetExponentTerms(
    const string & input,
    const string & current_term,
    vector<pair<string, int>> * terms) {
  vector<pair<char, int>> split_chars(1, make_pair('^', 1));
  return GetTerms<char>(false, input, "", split_chars, terms);
}

// TODO(PHB): Deprecate this (can use GetArgumentTerms() API instead),
// or keep it but get rid of the recursion call.
bool GetCommaTerms(
    const string & input, string * first_term, string * second_term) {
  size_t comma_pos = input.find(",");
  if (comma_pos == string::npos) {
    *first_term += input;
    return true;
  }

  // If comma lies inside of parentheses, it may not be the comma
  // we're looking for.
  size_t parentheses_pos = input.find("(");
  if (parentheses_pos != string::npos && parentheses_pos < comma_pos) {
    const string after_parentheses = input.substr(parentheses_pos);
    comma_pos = after_parentheses.find(",");
    size_t closing_pos;
    if (!GetClosingParentheses(after_parentheses, &closing_pos)) {
      cout << "ERROR: Unable to GetClosingParentheses for: '"
           << after_parentheses << "'" << endl;
      return false;
    }
    *first_term += input.substr(0, parentheses_pos) +
        after_parentheses.substr(0, closing_pos + 1);
    return GetCommaTerms(
        input.substr(parentheses_pos + closing_pos + 1),
        first_term,
        second_term);
  }
  *first_term += input.substr(0, comma_pos);
  *second_term = input.substr(comma_pos + 1);
  return true;
}

bool GetBooleanTerms(
    const string & input,
    string * first_term,
    string * second_term,
    BooleanOperation * op) {

  // NOTE: Order that BooleanOperations are inserted into 'boolean_operators'
  // is important! We want to check longer operation names (e.g. NAND) before
  // operations that are substrings of those (e.g. AND), as otherwise we'll
  // misproperly diagnose e.g. "x NAND y" as "(xN) AND (y)"
  vector<pair<string, BooleanOperation>> boolean_operators;
  boolean_operators.push_back(make_pair("NOR", BooleanOperation::NOR));
  boolean_operators.push_back(make_pair("XOR", BooleanOperation::XOR));
  boolean_operators.push_back(make_pair("OR", BooleanOperation::OR));
  boolean_operators.push_back(
      make_pair("NAND", BooleanOperation::NAND));
  boolean_operators.push_back(make_pair("AND", BooleanOperation::AND));
  boolean_operators.push_back(make_pair("EQ", BooleanOperation::EQ));
  boolean_operators.push_back(make_pair("GTE", BooleanOperation::GTE));
  boolean_operators.push_back(make_pair("GT", BooleanOperation::GT));
  boolean_operators.push_back(make_pair("LTE", BooleanOperation::LTE));
  boolean_operators.push_back(make_pair("LT", BooleanOperation::LT));
  for (int i = 0; i < boolean_operators.size(); ++i) {
    vector<pair<string, int>> terms;
    const vector<pair<string, int>> split_terms(
        1,
        make_pair(
            boolean_operators[i].first,
            boolean_operators[i].first.length()));
    if (!GetTerms<string>(true, input, "", split_terms, &terms)) {
      return false;
    }
    if (terms.empty() || terms.size() > 2) {
      return false;
    }
    if (terms.size() == 2) {
      *first_term = terms[0].first;
      *second_term = terms[1].first;
      *op = boolean_operators[i].second;
      return true;
    }
  }

  return true;
}

bool GetComparisonTerms(
    const string & input,
    string * first_term,
    string * second_term,
    ComparisonOperation * op) {
  vector<pair<string, ComparisonOperation>> comparison_operators;
  comparison_operators.push_back(
      make_pair("==", ComparisonOperation::COMP_EQ));
  comparison_operators.push_back(
      make_pair("!=", ComparisonOperation::COMP_NEQ));
  // NOTE: Important: testing for ">=" must happen BEFORE testing for ">", since
  // we are using (string::)find(). Ditto for "<=" and "<".
  comparison_operators.push_back(
      make_pair(">=", ComparisonOperation::COMP_GTE));
  comparison_operators.push_back(
      make_pair("<=", ComparisonOperation::COMP_LTE));
  comparison_operators.push_back(
      make_pair(">", ComparisonOperation::COMP_GT));
  comparison_operators.push_back(
      make_pair("<", ComparisonOperation::COMP_LT));
  for (int i = 0; i < comparison_operators.size(); ++i) {
    vector<pair<string, int>> terms;
    const vector<pair<string, int>> split_terms(
        1,
        make_pair(
            comparison_operators[i].first,
            comparison_operators[i].first.length()));
    if (!GetTerms<string>(true, input, "", split_terms, &terms)) {
      return false;
    }
    if (terms.empty() || terms.size() > 2)
      return false;
    if (terms.size() == 2) {
      *first_term = terms[0].first;
      *second_term = terms[1].first;
      *op = comparison_operators[i].second;
      return true;
    }
  }

  return true;
}

bool IsEmptyExpression(const Expression & exp) {
  if (exp.op_ != MathOperation::IDENTITY)
    return false;
  return exp.var_name_.empty() && !exp.is_constant_;
}

// TODO(PHB): Update this function to get rid of recursion; same way as
// was done for ParseFormula. Better yet, if possible, template that fn,
// so it can do both?
// Also, consider extending this to handle \sum and \product, like was done for Formula.
bool ParseExpression(
    const bool clean_input,
    const string & term_str,
    const bool enforce_var_names,
    const set<string> & var_names,
    Expression * expression) {
  if (clean_input) {
    // Remove all whitespace.
    string term_str_cleaned = RemoveAllWhitespace(term_str);

    // Replace all scientific notation with actual numbers, e.g.
    //   5.1e-2 -> 5.1 * 0.01
    term_str_cleaned = RemoveScientificNotation(term_str_cleaned);
    return ParseExpression(
        false,
        term_str_cleaned,
        enforce_var_names,
        var_names,
        expression);
  }

  // Try to split formula into two LINEAR terms, if possible. For example:
  //   (2x + 1)(x - 1) - x(2 + x)(x - 1) + ((x + 1)(x - 1)) * sqrt(2x)
  // Would be split as:
  //   (2x + 1)(x - 1),
  //   -x(2 + x)(x - 1) + ((x + 1)(x - 1)) * sqrt(2x)
  string first_term = "";
  string second_term = "";
  string input = term_str;
  vector<pair<string, int>> terms;
  if (!GetAdditiveTerms(input, "", &terms))
    return false;
  if (terms.size() > 1) {
    Expression * current_expression = expression;
    bool prev_term_was_subtraction = false;
    for (int i = 1; i < terms.size(); ++i) {
      if (terms[i - 1].second == 0) {
        current_expression->op_ = prev_term_was_subtraction ?
            MathOperation::SUB :
            MathOperation::ADD;
      } else {
        current_expression->op_ = prev_term_was_subtraction ?
            MathOperation::ADD :
            MathOperation::SUB;
        prev_term_was_subtraction = !prev_term_was_subtraction;
      }
      current_expression->subterm_one_ =
          unique_ptr<Expression>(new Expression());
      if (!ParseExpression(
              false,
              terms[i - 1].first,
              enforce_var_names,
              var_names,
              current_expression->subterm_one_.get())) {
        return false;
      }
      current_expression->subterm_two_ =
          unique_ptr<Expression>(new Expression());
      if (i == terms.size() - 1) {
        // Reached the last term; make it the second term of "current_expression", as
        // opposed to creating a new expression like the 'else' part of this
        // conditional block.
        return ParseExpression(
            false,
            terms[i].first,
            enforce_var_names,
            var_names,
            current_expression->subterm_two_.get());
      } else {
        // Not at the last term; so we'll need to create multiple terms to
        // hold the remaining terms.
        current_expression = current_expression->subterm_two_.get();
      }
    }
  }

  // Primary formula is not the sum of two (or more) terms. Try splitting it
  // into MULTIPLICATIVE terms.
  terms.clear();
  if (!GetMultiplicativeTerms(input, "", &terms))
    return false;
  if (terms.size() > 1) {
    Expression * current_expression = expression;
    bool last_term_was_division = false;
    for (int i = 1; i < terms.size(); ++i) {
      if (terms[i - 1].second == 0) {
        current_expression->op_ = last_term_was_division ?
            MathOperation::DIV :
            MathOperation::MULT;
      } else {
        current_expression->op_ = last_term_was_division ?
            MathOperation::MULT :
            MathOperation::DIV;
        last_term_was_division = !last_term_was_division;
      }
      current_expression->subterm_one_ =
          unique_ptr<Expression>(new Expression());
      if (!ParseExpression(
              false,
              terms[i - 1].first,
              enforce_var_names,
              var_names,
              current_expression->subterm_one_.get())) {
        return false;
      }
      current_expression->subterm_two_ =
          unique_ptr<Expression>(new Expression());
      if (i == terms.size() - 1) {
        // Reached the last term; make it the second term of "current_expression", as
        // opposed to creating a new term like the 'else' part of this
        // conditional block.
        return ParseExpression(
            false,
            terms[i].first,
            enforce_var_names,
            var_names,
            current_expression->subterm_two_.get());
      } else {
        // Not at the last term; so we'll need to create multiple terms to
        // hold the remaining terms.
        current_expression = current_expression->subterm_two_.get();
      }
    }
  }

  // Primary formula is not the product of two (or more) terms. Try splitting it
  // around '^' into base, exponent.
  terms.clear();
  if (!GetExponentTerms(term_str, "", &terms))
    return false;
  if (terms.size() > 1) {
    if (terms.size() > 2) {
      return false;
    }
    expression->op_ = MathOperation::POW;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    expression->subterm_two_ = unique_ptr<Expression>(new Expression());
    return (
        ParseExpression(
            false,
            terms[0].first,
            enforce_var_names,
            var_names,
            expression->subterm_one_.get()) &&
        ParseExpression(
            false,
            terms[1].first,
            enforce_var_names,
            var_names,
            expression->subterm_two_.get()));
  }

  // Primary formula is not a power (base, exponent). Try parsing it as a boolean
  // expression: split around '==', '~=', '>', '>=', '<', '<=".
  ComparisonOperation comp_op = ComparisonOperation::UNKNOWN;
  first_term = "";
  second_term = "";
  if (!GetComparisonTerms(
          term_str, &first_term, &second_term, &comp_op))
    return false;
  if (comp_op != ComparisonOperation::UNKNOWN) {
    if (first_term.empty() || second_term.empty())
      return false;
    if (comp_op == ComparisonOperation::COMP_GT) {
      expression->op_ = MathOperation::GT;
    } else if (comp_op == ComparisonOperation::COMP_GTE) {
      expression->op_ = MathOperation::GTE;
    } else if (comp_op == ComparisonOperation::COMP_LT) {
      expression->op_ = MathOperation::LT;
    } else if (comp_op == ComparisonOperation::COMP_LTE) {
      expression->op_ = MathOperation::LTE;
    } else if (comp_op == ComparisonOperation::COMP_EQ) {
      expression->op_ = MathOperation::EQ;
    } else if (comp_op == ComparisonOperation::COMP_NEQ) {
      expression->op_ = MathOperation::NEQ;
    } else {
      return false;
    }
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    expression->subterm_two_ = unique_ptr<Expression>(new Expression());
    return (
        ParseExpression(
            false,
            first_term,
            enforce_var_names,
            var_names,
            expression->subterm_one_.get()) &&
        ParseExpression(
            false,
            second_term,
            enforce_var_names,
            var_names,
            expression->subterm_two_.get()));
  }
  // Check for special MathOperation '~=', which doesn't have a ComparisonOperation
  // equivalent.
  terms.clear();
  const vector<pair<string, int>> split_terms(1, make_pair("~=", 2));
  if (!GetTerms<string>(true, term_str, "", split_terms, &terms)) {
    return false;
  }
  if (terms.empty() || terms.size() > 2)
    return false;
  if (terms.size() == 2) {
    first_term = terms[0].first;
    second_term = terms[1].first;
    if (first_term.empty() || second_term.empty())
      return false;
    expression->op_ = MathOperation::FLOAT_EQ;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    expression->subterm_two_ = unique_ptr<Expression>(new Expression());
    return (
        ParseExpression(
            false,
            first_term,
            enforce_var_names,
            var_names,
            expression->subterm_one_.get()) &&
        ParseExpression(
            false,
            second_term,
            enforce_var_names,
            var_names,
            expression->subterm_two_.get()));
    return true;
  }

  // Expression has a single primary term. Check if it is a special function.
  if (HasPrefixString(term_str, "LOG(") ||
      HasPrefixString(term_str, "Log(") ||
      HasPrefixString(term_str, "log(")) {
    if (!HasSuffixString(term_str, ")"))
      return false;
    expression->op_ = MathOperation::LOG;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    return ParseExpression(
        false,
        term_str.substr(4, term_str.length() - 5),
        enforce_var_names,
        var_names,
        expression->subterm_one_.get());
  } else if (
      HasPrefixString(term_str, "LN(") ||
      HasPrefixString(term_str, "Ln(") ||
      HasPrefixString(term_str, "ln(")) {
    if (!HasSuffixString(term_str, ")"))
      return false;
    expression->op_ = MathOperation::LOG;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    return ParseExpression(
        false,
        term_str.substr(3, term_str.length() - 4),
        enforce_var_names,
        var_names,
        expression->subterm_one_.get());
  } else if (
      HasPrefixString(term_str, "EXP(") ||
      HasPrefixString(term_str, "Exp(") ||
      HasPrefixString(term_str, "exp(")) {
    if (!HasSuffixString(term_str, ")"))
      return false;
    expression->op_ = MathOperation::EXP;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    return ParseExpression(
        false,
        term_str.substr(4, term_str.length() - 5),
        enforce_var_names,
        var_names,
        expression->subterm_one_.get());
  } else if (
      HasPrefixString(term_str, "ABS(") ||
      HasPrefixString(term_str, "Abs(") ||
      HasPrefixString(term_str, "abs(")) {
    if (!HasSuffixString(term_str, ")"))
      return false;
    expression->op_ = MathOperation::ABS;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    return ParseExpression(
        false,
        term_str.substr(4, term_str.length() - 5),
        enforce_var_names,
        var_names,
        expression->subterm_one_.get());
  } else if (
      HasPrefixString(term_str, "SQRT(") ||
      HasPrefixString(term_str, "Sqrt(") ||
      HasPrefixString(term_str, "sqrt(")) {
    if (!HasSuffixString(term_str, ")"))
      return false;
    expression->op_ = MathOperation::SQRT;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    return ParseExpression(
        false,
        term_str.substr(5, term_str.length() - 6),
        enforce_var_names,
        var_names,
        expression->subterm_one_.get());
  } else if (
      HasPrefixString(term_str, "SIN(") ||
      HasPrefixString(term_str, "Sin(") ||
      HasPrefixString(term_str, "sin(")) {
    if (!HasSuffixString(term_str, ")"))
      return false;
    expression->op_ = MathOperation::SIN;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    return ParseExpression(
        false,
        term_str.substr(4, term_str.length() - 5),
        enforce_var_names,
        var_names,
        expression->subterm_one_.get());
  } else if (
      HasPrefixString(term_str, "COS(") ||
      HasPrefixString(term_str, "Cos(") ||
      HasPrefixString(term_str, "cos(")) {
    if (!HasSuffixString(term_str, ")"))
      return false;
    expression->op_ = MathOperation::COS;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    return ParseExpression(
        false,
        term_str.substr(4, term_str.length() - 5),
        enforce_var_names,
        var_names,
        expression->subterm_one_.get());
  } else if (
      HasPrefixString(term_str, "TAN(") ||
      HasPrefixString(term_str, "Tan(") ||
      HasPrefixString(term_str, "tan(")) {
    if (!HasSuffixString(term_str, ")"))
      return false;
    expression->op_ = MathOperation::TAN;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    return ParseExpression(
        false,
        term_str.substr(4, term_str.length() - 5),
        enforce_var_names,
        var_names,
        expression->subterm_one_.get());
  } else if (HasPrefixString(term_str, "I(")) {
    if (!HasSuffixString(term_str, ")"))
      return false;
    expression->op_ = MathOperation::INDICATOR;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    return ParseExpression(
        false,
        term_str.substr(2, term_str.length() - 3),
        enforce_var_names,
        var_names,
        expression->subterm_one_.get());
  } else if (HasPrefixString(term_str, "Phi(")) {
    if (!HasSuffixString(term_str, ")"))
      return false;
    expression->op_ = MathOperation::PHI_FN;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    return ParseExpression(
        false,
        term_str.substr(4, term_str.length() - 5),
        enforce_var_names,
        var_names,
        expression->subterm_one_.get());
  } else if (HasPrefixString(term_str, "Gamma(")) {
    if (!HasSuffixString(term_str, ")"))
      return false;
    expression->op_ = MathOperation::GAMMA_FN;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    return ParseExpression(
        false,
        term_str.substr(6, term_str.length() - 7),
        enforce_var_names,
        var_names,
        expression->subterm_one_.get());
  } else if (HasPrefixString(term_str, "IncGamma(")) {
    if (!HasSuffixString(term_str, ")"))
      return false;
    expression->op_ = MathOperation::INC_GAMMA_FN;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    expression->subterm_two_ = unique_ptr<Expression>(new Expression());
    string first_term, second_term;
    string input = StripExtraneousParentheses(
        term_str.substr(9, term_str.length() - 10));
    if (!GetCommaTerms(input, &first_term, &second_term)) {
      return false;
    }
    return (
        ParseExpression(
            false,
            first_term,
            enforce_var_names,
            var_names,
            expression->subterm_one_.get()) &&
        ParseExpression(
            false,
            second_term,
            enforce_var_names,
            var_names,
            expression->subterm_two_.get()));
  } else if (HasPrefixString(term_str, "RegIncGamma(")) {
    if (!HasSuffixString(term_str, ")"))
      return false;
    expression->op_ = MathOperation::REG_INC_GAMMA_FN;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    expression->subterm_two_ = unique_ptr<Expression>(new Expression());
    string first_term, second_term;
    string input = StripExtraneousParentheses(
        term_str.substr(12, term_str.length() - 13));
    if (!GetCommaTerms(input, &first_term, &second_term)) {
      return false;
    }
    return (
        ParseExpression(
            false,
            first_term,
            enforce_var_names,
            var_names,
            expression->subterm_one_.get()) &&
        ParseExpression(
            false,
            second_term,
            enforce_var_names,
            var_names,
            expression->subterm_two_.get()));
  } else if (
      HasPrefixString(term_str, "POW(") ||
      HasPrefixString(term_str, "Pow(") ||
      HasPrefixString(term_str, "pow(")) {
    if (!HasSuffixString(term_str, ")"))
      return false;
    expression->op_ = MathOperation::POW;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    expression->subterm_two_ = unique_ptr<Expression>(new Expression());
    string first_term, second_term;
    string input = StripExtraneousParentheses(
        term_str.substr(4, term_str.length() - 5));
    if (!GetCommaTerms(input, &first_term, &second_term)) {
      return false;
    }
    return (
        ParseExpression(
            false,
            first_term,
            enforce_var_names,
            var_names,
            expression->subterm_one_.get()) &&
        ParseExpression(
            false,
            second_term,
            enforce_var_names,
            var_names,
            expression->subterm_two_.get()));
  }

  // Primary formula has only one term. Try to parse.

  // Check factorial first, since it is the unique time a formula may
  // start with '(' but not end in ')', as in e.g. (n - 1)!.
  if (HasSuffixString(term_str, "!")) {
    if (term_str.length() < 2)
      return false;
    expression->op_ = MathOperation::FACTORIAL;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    // Check if character before '!' is a closing parenthesis.
    if (term_str.substr(term_str.length() - 2, 1) == ")") {
      if (!HasPrefixString(term_str, "("))
        return false;
      return ParseExpression(
          false,
          term_str.substr(1, term_str.length() - 3),
          enforce_var_names,
          var_names,
          expression->subterm_one_.get());
    }
    return ParseExpression(
        false,
        term_str.substr(0, term_str.length() - 1),
        enforce_var_names,
        var_names,
        expression->subterm_one_.get());
  }

  // Check for extraneous extra enclosing parentheses.
  if (HasPrefixString(term_str, "(")) {
    if (!HasSuffixString(term_str, ")"))
      return false;
    return ParseExpression(
        false,
        term_str.substr(1, term_str.length() - 2),
        enforce_var_names,
        var_names,
        expression);
  }

  // Check for leading negative sign.
  if (HasPrefixString(term_str, "-")) {
    expression->op_ = MathOperation::MULT;
    expression->subterm_one_ = unique_ptr<Expression>(new Expression());
    expression->subterm_one_->op_ = MathOperation::IDENTITY;
    expression->subterm_one_->value_ = -1.0;
    expression->subterm_one_->is_constant_ = true;
    expression->subterm_two_ = unique_ptr<Expression>(new Expression());
    return ParseExpression(
        false,
        term_str.substr(1),
        enforce_var_names,
        var_names,
        expression->subterm_two_.get());
  }

  // If we've made it here, there is no more reduction that can be done.
  // Check to see if it is a variable name.
  if (var_names.find(term_str) != var_names.end()) {
    expression->op_ = MathOperation::IDENTITY;
    expression->var_name_ = term_str;
    return true;
  }

  // Check if this term is a coefficient followed by a variable name, e.g. "2x".
  for (const string & var_name : var_names) {
    size_t var_pos = term_str.find(var_name);
    if (var_pos != string::npos) {
      const string non_var = term_str.substr(0, var_pos);
      const string var_str = term_str.substr(var_pos);
      double value;
      if (!Stod(non_var, &value))
        return false;
      expression->op_ = MathOperation::MULT;
      expression->subterm_one_ =
          unique_ptr<Expression>(new Expression());
      expression->subterm_one_->op_ = MathOperation::IDENTITY;
      expression->subterm_one_->value_ = value;
      expression->subterm_one_->is_constant_ = true;
      expression->subterm_two_ =
          unique_ptr<Expression>(new Expression());
      return ParseExpression(
          false,
          var_str,
          enforce_var_names,
          var_names,
          expression->subterm_two_.get());
    }
  }

  // Not a variable name. Try to parse as a numeric value; return false if not.
  double value;
  if (Stod(term_str, &value)) {
    expression->op_ = MathOperation::IDENTITY;
    expression->value_ = value;
    expression->is_constant_ = true;
    return true;
  }

  // Failed to parse this term. If enforce_var_names is true, return false.
  // Otherwise, just treat this term as a variable name.
  if (enforce_var_names)
    return false;
  expression->op_ = MathOperation::IDENTITY;
  expression->var_name_ = term_str;
  return true;
}

void CopyExpression(
    const Expression & expression, Expression * new_expression) {
  new_expression->op_ = expression.op_;
  new_expression->var_name_ = expression.var_name_;
  new_expression->value_ = expression.value_;
  new_expression->is_constant_ = expression.is_constant_;

  if (expression.subterm_one_ != nullptr) {
    new_expression->subterm_one_ =
        unique_ptr<Expression>(new Expression());
    CopyExpression(
        *expression.subterm_one_, new_expression->subterm_one_.get());
  }
  if (expression.subterm_two_ != nullptr) {
    new_expression->subterm_two_ =
        unique_ptr<Expression>(new Expression());
    CopyExpression(
        *expression.subterm_two_, new_expression->subterm_two_.get());
  }
}

Expression CopyExpression(const Expression & expression) {
  Expression to_return;
  CopyExpression(expression, &to_return);
  return to_return;
}

string GetTermString(const VariableTerm & term) {
  const MathOperation op = term.op_;
  const string & title = term.term_title_;
  const string exponent =
      term.op_ == MathOperation::POW ? Itoa(term.exponent_) : "";

  switch (op) {
    case MathOperation::IDENTITY:
      return title;
    case MathOperation::EXP:
      return ("exp(" + title + ")");
    case MathOperation::SQRT:
      return ("sqrt(" + title + ")");
    case MathOperation::POW:
      return title + "^" + exponent;
    case MathOperation::LOG:
      return ("Log(" + title + ")");
    default:
      cout << "Unknown operation or unexpected group operation: "
           << static_cast<int>(op) << "\n";
  }
  return "";
}

string GetSamplingParamsString(const SamplingParameters & params) {
  string range = "";
  if (params.first_sample_ >= 0) {
    if (params.last_sample_ >= 0) {
      range = " on Samples [" + Itoa(params.first_sample_) + ", " +
          Itoa(params.last_sample_) + "]";
    } else {
      range = " on Samples [" + Itoa(params.first_sample_) + ", N]";
    }
  } else if (params.last_sample_ >= 0) {
    range = " on Samples [1, " + Itoa(params.last_sample_) + "]";
  }

  const string constant =
      params.constant_ == 1.0 ? "" : Itoa(params.constant_) + " * ";
  switch (params.type_) {
    // TODO(PHB): Implement remaining types.
    case Distribution::BERNOULLI: {
      return "BERNOULLI_STRING";
    }
    case Distribution::BINOMIAL: {
      return "BINOMIAL_STRING";
    }
    case Distribution::CAUCHY: {
      return "CAUCHY_STRING";
    }
    case Distribution::CHI_SQUARED: {
      return "CHI_SQUARED_STRING";
    }
    case Distribution::CONSTANT: {
      return "Constant(" + Itoa(params.constant_) + ")" + range;
    }
    case Distribution::NEGATIVE_BINOMIAL: {
      return "NEGATIVE_BINOMIAL_STRING";
    }
    case Distribution::EXPONENTIAL: {
      return "EXPONENTIAL_STRING";
    }
    case Distribution::GAMMA: {
      return "GAMMA_STRING";
    }
    case Distribution::GEOMETRIC: {
      return "GEOMETRIC_STRING";
    }
    case Distribution::NORMAL: {
      return (constant + "N(" + Itoa(params.mean_) + "," +
              Itoa(params.std_dev_) + ")") +
          range;
    }
    case Distribution::LOG_NORMAL: {
      return "LOG_NORMAL_STRING";
    }
    case Distribution::POISSON: {
      return "POISSON_STRING";
    }
    case Distribution::STUDENT_T: {
      return "STUDENT_T_STRING";
    }
    case Distribution::UNIFORM: {
      return (constant + "U(" + Itoa(params.range_start_) + "," +
              Itoa(params.range_end_) + ")") +
          range;
    }
    case Distribution::DISCRETE_UNIFORM: {
      return ("DU([" + Itoa(params.range_start_) + "," +
              Itoa(params.range_end_) + "], " + Itoa(params.constant_) +
              ")") +
          range;
    }
    case Distribution::LOG_UNIFORM: {
      return (constant + "log U(" + Itoa(params.range_start_) + "," +
              Itoa(params.range_end_) + ")") +
          range;
    }
  }
  return "";
}

double
ComputeSelfOperation(const VariableTerm & term, const double & value) {
  const MathOperation & op = term.op_;
  const double & exponent = term.exponent_;

  switch (op) {
    case MathOperation::IDENTITY:
      return value;
    case MathOperation::EXP:
      return exp(value);
    case MathOperation::SQRT: {
      if (value < 0.0) {
        cout << "Unable to compute sqrt of a negative value: " << value
             << ". Using 0.0 instead.\n";
        return 0.0;
      }
      return pow(value, 0.5);
    }
    case MathOperation::POW: {
      errno = 0;
      double pow_value = pow(value, exponent);
      if (errno == ERANGE || errno == EDOM) {
        cout << "Attempting to compute pow(" << value << ", "
             << exponent << ") resulted in error: " << strerror(errno)
             << ". Using 0.0 instead.\n";
        return 0.0;
      }
      return pow_value;
    }
    case MathOperation::LOG: {
      if (value <= 0.0) {
        cout << "Unable to compute log of a non-positive value: "
             << value << ". Using log(" << value << ") = 0.0.\n";
        return 0.0;
      }
      return log(value);
    }
    default:
      cout << "Unknown operation or unexpected group operation: "
           << static_cast<int>(op) << "\n";
  }
  return 0.0;
}

bool ComputeGroupOperation(
    const MathOperation op,
    const double & v1,
    const double & v2,
    double * output) {
  switch (op) {
    case MathOperation::ADD:
      *output = v1 + v2;
      return true;
    case MathOperation::MULT:
      *output = v1 * v2;
      return true;
    default:
      cout << "Unknown operation, or unexpected self-operation: "
           << static_cast<int>(op) << "\n";
  }
  return false;
}

} // namespace math_utils
