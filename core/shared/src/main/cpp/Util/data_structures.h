/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 *
 * Description: Defines objects that will be useful data storage
 * holders for solving systems and running simulations.
 */

#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include "constants.h"
#include "string_utils.h"

#include <cerrno>
#include <cfloat> // For DBL_MIN, DBL_MAX
#include <climits> // For ULLONG_MAX, etc.
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <memory> // For unique_ptr.
#include <set>
#include <string>
#include <tuple> // For pair.
#include <typeinfo> // For typeid, bad_cast
#include <vector>

/* Logging Configuration */
#include <ff/logging.h>

namespace math_utils {

namespace {
static const int kManyCharsToPrint = 10000;
} // namespace

/* ================================== Enums ================================= */

// Describes the nature of a field: string, numeric, etc.
// NOTE: DOUBLE not supported (not sure if it should be?)
enum class DataType {
  UNKNOWN,
  // String types.
  // The numeric suffix refers to (upper-bound on) number of bytes (characters).
  STRING8,
  STRING16,
  STRING24,
  STRING32,
  STRING64,
  STRING128,
  // Numeric types.
  // The numeric suffix on the int types refers to (upper-bound on) number of bits.
  BOOL,
  INT2,
  UINT2,
  INT4,
  UINT4,
  INT8,
  UINT8,
  INT16,
  UINT16,
  INT32,
  UINT32,
  INT64,
  UINT64,
  SLICE,
  // The following are (currently) only partially supported; use with care.
  DOUBLE,
  VECTOR,
};

// Parses DataType from string.
extern DataType StringToDataType(const std::string & input);
// Returns a string representation of the DataType.
extern std::string GetDataTypeString(const DataType type);
// Returns true if type is any of the STRINGXXX types.
extern bool IsStringDataType(const DataType type);
// Returns true if type is any of the [U]INTXX or SLICE or BOOL types.
extern bool IsIntegerDataType(const DataType type);
// Same as above, but also returns true for DOUBLE.
extern bool IsNumericDataType(const DataType type);
// Returns whether the input DataType is represented as 2's complement (as
// opposed to direct binary string).
extern bool IsDataTypeTwosComplement(const DataType type);
// Returns whether the provided DataType is a signed type.
// This is identical to the above, except that DOUBLE also returns true.
extern bool IsSignedDataType(const DataType type);
// Returns whether this DataType's size is less than 1 byte.
inline bool IsSmallDataType(const DataType type) {
  return (
      type == DataType::BOOL || type == DataType::INT2 ||
      type == DataType::UINT2 || type == DataType::INT4 ||
      type == DataType::UINT4);
}

// Describes the nature of a field: string, numeric, etc.
// Used by statistics package (under CodeBase/Regression/).
enum StatsDataType {
  DATA_TYPE_UNKNOWN,
  DATA_TYPE_STRING,
  DATA_TYPE_NUMERIC,
  DATA_TYPE_NUMERIC_RANGE,
};

// There are various 'Operation' enums below, which specify the set of
// operations that are valid, for various use-cases:
//   - MATH: enum MathOperation was originally built for UNC BIOS applications (stats)
//   - BOOLEAN: enum BooleanOperation represents the (2-in-1-out) gate operations
//   - COMPARISON: enum ComparisonOperation holds the 6 comparison operators ([N]EQ, [GL]T[E])
//   - ARITHMETIC: enum ArithmeticOperation holds the operations permitted in an arith circuit
enum class OperationType {
  UNKNOWN,
  MATH,
  BOOLEAN,
  COMPARISON,
  ARITHMETIC,
  CIRCUIT_GATE
};

// An enum representing various operations to apply to a single
// (double) value, or to combine two (double) values. Each
// function that takes in this enum will be responsible for actually
// determining how the operation is performed (e.g. via a 'switch' statement).
enum class MathOperation {
  // Self-Operation.
  IDENTITY,

  // 1-Term Operations (i.e. these operations take a single argument)
  ABS,
  EXP, // exp(x), i.e. e^x.
  FACTORIAL,
  LOG,
  SQRT,
  SIN,
  COS,
  TAN,
  INDICATOR,
  GAMMA_FN, // The Gamma function \Gamma(z) = \int_0^{\inf} t^{z - 1} * e^{-t} dt
  PHI_FN, // The \Phi function (Standard Normal CDF).

  // 2-Term Operations (i.e. these operations take in two arguments).
  ADD,
  SUB,
  MULT,
  DIV,
  POW,
  MODULUS, // Computes a (mod b) for integers a, b with b > 0.
  EQ, // Equals (==)
  NEQ, // Not Equal (!=)
  FLOAT_EQ, // FloatEq()
  GT, // Greater Than (>)
  GTE, // Greater Than or Equals (>=)
  LT, // Less Than (<)
  LTE, // Less Than or Equals (<=)
  INC_GAMMA_FN, // (Lower) Incomplete Gamma fn \gamma(df, x) = \int_0^x t^{df - 1} * e^{-t} dt
  REG_INC_GAMMA_FN, // Regularized Inc. Gamma fn P(df, x) = \gamma(df, x) / \Gamma(df)
};

inline bool IsSingleInputOperation(const MathOperation op) {
  return (
      op == MathOperation::ABS || op == MathOperation::EXP ||
      op == MathOperation::FACTORIAL || op == MathOperation::LOG ||
      op == MathOperation::SQRT || op == MathOperation::SIN ||
      op == MathOperation::COS || op == MathOperation::TAN ||
      op == MathOperation::INDICATOR || op == MathOperation::GAMMA_FN ||
      op == MathOperation::PHI_FN);
}

inline bool IsDoubleInputOperation(const MathOperation op) {
  return (
      op == MathOperation::ADD || op == MathOperation::SUB ||
      op == MathOperation::MULT || op == MathOperation::DIV ||
      op == MathOperation::POW || op == MathOperation::EQ ||
      op == MathOperation::MODULUS || op == MathOperation::FLOAT_EQ ||
      op == MathOperation::GT || op == MathOperation::GTE ||
      op == MathOperation::LT || op == MathOperation::LTE ||
      op == MathOperation::INC_GAMMA_FN ||
      op == MathOperation::REG_INC_GAMMA_FN);
}

// Represents all possible operations at a standard (2-in, 1-out) gate.
// Discussion: Since there are 2 input wires, there are four possible input
// combinations, and hence the truth table has 2^4 = 16 possible columns.
// However, 2 of these represent a gate that does not depend on either input
// wire (all 0's and all 1's), 2 of these represent a gate that only depends
// on the left wire (output bit matches (resp. doesn't match) input bit on
// that wire), and 2 represent a gate that only depends on the right wire.
// All 6 of these are not valid "standard" gates, since they don't depend
// on two input wires (and in particular, we don't need a gate at all for them).
// Hence, there are 10 (= 16 - 2 - 2 - 2) distinct standard gate types below.
enum class BooleanOperation {
  UNKNOWN,

  // The following two gate types are not "standard", in that they (intuitively)
  // have 1-input, 1-output (as opposed to 2-inputs, 1-output). However, having
  // them is necessary to support various things that need a general interface
  // (e.g. constructing circuits, which itself relies on having a single gate
  // circuit with one input and one output, with output = input); plus, these
  // could be viewed as one of the 4 circuits that depend only on one wire
  // (see comments above, about the 4 standard gates not included).
  // In terms of evaluating these gates, we use the convention that the *left*
  // wire is the one that is used (and the *right* wire is ignored).
  IDENTITY,
  NOT,

  // The 10 standard gate types:
  AND,
  NAND,
  OR,
  NOR,
  XOR,
  // The following comparisons are at the bit level (since inputs are slices of
  // 64 bits); i.e. the outputs of each such gate will have '0's and '1's in
  // the appropriate places (as opposed to all 0's or all 1's).
  EQ, // Same as !XOR, i.e. checks bitwise equality.
  GT, // If left input wire is (strictly) greater than right input wire
  GTE, // If left input wire is greater than or equal to right input wire
  LT, // If left input wire is (strictly) less than right input wire (same as !GTE)
  LTE, // If left input wire is less than or equal to right input wire (same as !GT)
};

inline bool IsSingleInputOperation(const BooleanOperation op) {
  return (
      op == BooleanOperation::IDENTITY || op == BooleanOperation::NOT);
}

inline bool IsDoubleInputOperation(const BooleanOperation op) {
  return (
      op == BooleanOperation::AND || op == BooleanOperation::NAND ||
      op == BooleanOperation::OR || op == BooleanOperation::NOR ||
      op == BooleanOperation::XOR || op == BooleanOperation::EQ ||
      op == BooleanOperation::GT || op == BooleanOperation::GTE ||
      op == BooleanOperation::LT || op == BooleanOperation::LTE);
}

// An enum representing various COMPARISON operations that can be applied to
// compare two (single or multiple bit) values. NOTE: We use prefix "COMP_"
// (short for "comparison") in order to distinguish these from the similar
// (but different!) BooleanOperations.
enum class ComparisonOperation {
  UNKNOWN,
  COMP_EQ,
  COMP_NEQ,
  COMP_GT,
  COMP_GTE,
  COMP_LT,
  COMP_LTE
};

// An enum representing various ARITHMETIC operations that can be applied to a
// single (integer) value, or to combine two (integer) values.
// NOTE: Only ADD, SUB, and MULT are currently supported.
// NOTE: Consider allowing more general arithmetic operators;
// see MathOperation for some other potentially useful operators.
enum class ArithmeticOperation {
  UNKNOWN,
  // Self-Operation.
  IDENTITY,

  // 1-Term Operations (i.e. these operations take a single argument)
  ABS,
  FLIP_SIGN, // Positive <-> Negative
  FACTORIAL, // For circuits, only supported for x <= 20.
  SQRT,

  // 2-Term Operations (i.e. these operations take in two arguments).
  ADD,
  SUB,
  MULT,
  DIV,
  POW, // Only for 2-argument: pow(a, b).
  // NOTE: For POW gates ins a circuit, we force a to be [U]INT, and current
  // restriction on 'b' is that it be a positive integer in 0..63, where:
  //   - We restrict b to be an integer because circuits only currently
  //     supported for [U]INT DataTypes
  //   - We restrict b to be positive because DIV not yet supported
  //   - We restrict max to 63: For [U]INTXX DataTypes, since the largest
  //     currently available is 64-bits, the max range of b is 63.

  // >=2-Term Operations (i.e. these operations specify a list of arguments).
  VEC, // A vector of values. Has two arguments VEC(a, b), but can be chained,
  // e.g. (a, b, c, d) = VEC(VEC(a,b), VEC(c,d))
  INNER_PRODUCT, // Inner-Product. Format: IP(x1, x2, ..., xN; y1, y2, ..., yN)
  // Computes: x1 * y1 + ... + xN * YN
  MIN, // Only for 2-argument: min(a, b). For more arguments, chain them,
  MAX, // e.g. min(min(a, b), min(min(c, d), min(e, f))).
  ARGMIN, // Same as MIN/MAX, but rather than return the minimum *value*, this
  ARGMAX, // outputs a characteristic vector with the '1' in the position of the min.
  // NOTES:
  //   - This can take in arbitrarily many elements, not just two
  //   - ARGMIN/MAX could conceivable output one of two things:
  //       i) A characteristic vector, with a '1' in the min position
  //      ii) A value, representing the index of the minimum
  //     There are trade-offs for (i) vs. (ii):
  //       - (ii) is consistent with all of the other operations, in
  //         that it outputs a *value* (not a vector)
  //       - (i) is consistent with how ARGMIN/ARGMAX is likely
  //         to be used: as a selector of a list of values
  //       - Both are not easy to implement: both require a circuit
  //         that outputs multiple things: The value of the min
  //         (needed for chaining, i.e. when the number of args
  //         to ARGMIN/ARGMAX is more than just two), and also
  //         the actual ARGMIN/ARGMAX output (a value or vector).
  //     The implementation of (i) vs. (ii) will depend on where
  //     this operation is used. For Formula and StandardCircuits
  //     (currently the only place that uses these ops), (i) is done.
  //   - There are two natural ways to handle multiple values equaling the min:
  //       i) If we are allowed to output a characteristic vector (instead
  //          of the index/location of the min), then put a '1' in every
  //          coordinate that equals the min.
  //      ii) Output one of the coordinates that equals the min. Choice
  //          of which coordinate to use could be:
  //            a) Deterministic (e.g. always choose lowest order index)
  //            b) Random/weighted
  //          Notice that (b) could be obtained from (a) by having
  //          the randomization occur on the input-ordering.
  //     Notice that (i) is more general (in that (ii) can be recovered
  //     from (i), but not vice-versa), however, there will need to
  //     be extra logic to go from (i) to (ii), and (ii) is likely
  //     to be what is needed anyway.
  //     The implementation of (i) vs. (ii) will depend on where
  //     this operation is used. For Formula and StandardCircuits
  //     (currently the only place that uses these ops), (ii.a) is done;
  //     and more specifically, the *smallest* satisfying index is
  //     deterministically chosen by default (although this can be
  //     changed to deterministically choose the *largest* index by,
  //     during circuit construction, first calling:
  //     SetArgminBreakTiesPos(false)).
  ARGMIN_INTERNAL, // Same as ARGMIN,ARGMAX, except that instead of outputting
  ARGMAX_INTERNAL, // just the characteristic vector, also outputs the *value*
  // of the minimum (thus output has n+1 values). Useful for
  // circuits, when taking the ARGMIN/ARGMAX of more than
  // two values (so that we can chain together circuits).
  // DEPRECATED. Identical to EQ in ComparisonOperation above, so use that instead.
  // EQ,   // Equality (==)
};

inline bool IsSingleInputOperation(const ArithmeticOperation op) {
  return (
      op == ArithmeticOperation::ABS ||
      op == ArithmeticOperation::FLIP_SIGN ||
      op == ArithmeticOperation::FACTORIAL ||
      op == ArithmeticOperation::SQRT);
}

inline bool IsDoubleInputOperation(const ArithmeticOperation op) {
  return (
      op == ArithmeticOperation::ADD ||
      op == ArithmeticOperation::SUB ||
      op == ArithmeticOperation::MULT ||
      op == ArithmeticOperation::DIV ||
      op == ArithmeticOperation::POW ||
      op == ArithmeticOperation::VEC ||
      op == ArithmeticOperation::MIN ||
      op == ArithmeticOperation::MAX ||
      op == ArithmeticOperation::ARGMIN ||
      op == ArithmeticOperation::ARGMAX ||
      op == ArithmeticOperation::ARGMAX_INTERNAL ||
      op == ArithmeticOperation::ARGMIN_INTERNAL ||
      op == ArithmeticOperation::INNER_PRODUCT);
}

// The following enum class is a merged class of BooleanOperation and
// ArithmeticOperation. I could just have any Object that requires one
// of these two just have two fields (one for each), but this would waste
// space (memory). So, the following enum specifies all possible
// gate operations that could appear in a Boolean or Arithmetic circuit.
// NOTE: It may have been possible to design things differently and utilize
//       'bit field' (a la http://en.cppreference.com/w/cpp/language/bit_field)
//       to pack bits efficiently (and so to cram the bit indicating Bool vs.
//       Arithmetic, as well as the other bits that indicate which op it is),
//       but I didn't do this, because: a) I discovered 'bit field' only after
//       implementing things the present way; and b) I'm not convinced the
//       bit field approach would work anyway (compiler dependent optimization).
enum class CircuitOperation {
  UNKNOWN,

  // Boolean Operations.
  IDENTITY,
  NOT,
  AND,
  NAND,
  OR,
  NOR,
  XOR,
  EQ, // Same as !XOR, i.e. checks bitwise equality.
  GT, // If left input wire is (strictly) greater than right input wire
  GTE, // If left input wire is greater than or equal to right input wire
  LT, // If left input wire is (strictly) less than right input wire (same as !GTE)
  LTE, // If left input wire is less than or equal to right input wire (same as !GT)

  // Arithmetic Operations.
  SELF, // Same as Identity, but needed a different keyword to distinguish from
  // the Boolean IDENTITY operation.
  ABS,
  FLIP_SIGN, // Positive <-> Negative
  FACTORIAL, // For circuits, only supported for x <= 20.
  SQRT, // NOT YET SUPPORTED
  ADD,
  SUB,
  MULT,
  DIV, // NOT YET SUPPORTED
  POW, // Only for 2-argument: pow(a, b).
  MIN, // Only for 2-argument: min(a, b). For more arguments, chain them,
  MAX, // e.g. min(min(a, b), min(min(c, d), min(e, f))).
  ARGMIN, // Outputs a (characteristic) vector of length equal to the number
  ARGMAX, // of inputs, with the '1' in the coordinate corresponding to the
  // (first) minimum/maximum.
  ARGMIN_INTERNAL,
  ARGMAX_INTERNAL,
  INNER_PRODUCT,
};

inline bool IsSingleInputOperation(const CircuitOperation op) {
  return (
      op == CircuitOperation::IDENTITY ||
      op == CircuitOperation::SELF || op == CircuitOperation::NOT ||
      op == CircuitOperation::ABS ||
      op == CircuitOperation::FLIP_SIGN ||
      op == CircuitOperation::FACTORIAL ||
      op == CircuitOperation::SQRT);
}

inline bool IsDoubleInputOperation(const CircuitOperation op) {
  return (
      op == CircuitOperation::AND || op == CircuitOperation::NAND ||
      op == CircuitOperation::OR || op == CircuitOperation::NOR ||
      op == CircuitOperation::XOR || op == CircuitOperation::EQ ||
      op == CircuitOperation::GT || op == CircuitOperation::GTE ||
      op == CircuitOperation::LT || op == CircuitOperation::LTE ||
      op == CircuitOperation::ADD || op == CircuitOperation::SUB ||
      op == CircuitOperation::MULT || op == CircuitOperation::DIV ||
      op == CircuitOperation::POW || op == CircuitOperation::MIN ||
      op == CircuitOperation::MAX || op == CircuitOperation::ARGMIN ||
      op == CircuitOperation::ARGMAX ||
      op == CircuitOperation::ARGMAX_INTERNAL ||
      op == CircuitOperation::ARGMIN_INTERNAL ||
      op == CircuitOperation::INNER_PRODUCT);
}

inline bool IsBooleanOperation(const CircuitOperation op) {
  return (
      op == CircuitOperation::IDENTITY || op == CircuitOperation::NOT ||
      op == CircuitOperation::AND || op == CircuitOperation::NAND ||
      op == CircuitOperation::OR || op == CircuitOperation::NOR ||
      op == CircuitOperation::XOR || op == CircuitOperation::EQ ||
      op == CircuitOperation::GT || op == CircuitOperation::GTE ||
      op == CircuitOperation::LT || op == CircuitOperation::LTE);
}

extern CircuitOperation BoolOpToCircuitOp(const BooleanOperation op);

// Operations for vector values.
// TODO(PHB): Implement functionalities for these, where appropriate.
enum class VectorOperation {
  SUM,
  DIFF,
  SCALAR_ADD, // Add a constant to all coordinates.
  SCALAR_MULT, // Multiply all coordinates by a constant.
  INNER_PRODUCT, // NOTE: Cross-Product is not supported, as the range
  //       of cross-product is m x n matrices (not 1-D).
  ARGMIN, // See notes on ARGMIN/MAX in ArithmeticOperation above.
  ARGMAX, // In particular, this version will output a characteristic
  // vector with a '1' in the appropriate coordinate.
  // NOTE: This isn't yet implemented, so the decision on
  // how to handle multiple values equaling the min has
  // not been made (choose one coordinate vs. have all
  // matching coordinates equal '1').
};

// Operations for matrices.
// TODO(PHB): Implement functionalities for these, where appropriate.
enum class MatrixOperation {
  SUM,
  DIFF,
  SCALAR_ADD, // Add a constant to all coordinates.
  SCALAR_MULT, // Multiply all coordinates by a constant.
  CROSS_PRODUCT,
  TRANSPOSE,
  DET, // Determinant
  INV, // Inverse.
  ARGMIN, // Outputs a matrix with a '1' in every position that
  ARGMAX, // equals the min/max.
};

// Various Distributions that are supported for sampling.
enum Distribution {
  BERNOULLI, // "bern"
  BINOMIAL, // "bin"
  CAUCHY, // "cauchy"
  CHI_SQUARED, // "chi_sq"
  CONSTANT, // "C", "const", "constant"
  EXPONENTIAL, // "exp"
  GAMMA, // "gamma"
  GEOMETRIC, // "geo"
  LOG_NORMAL, // "log_N", "log_n"
  LOG_UNIFORM, // "log_U", "log_u"
  NEGATIVE_BINOMIAL, // "neg_bin"
  NORMAL, // "N", "normal", "norm"
  POISSON, // "P", "poisson"
  STUDENT_T, // "t"
  UNIFORM, // "U", "uniform", "unif"
  DISCRETE_UNIFORM, // "DU", "discrete uniform", "disc unif"
};

enum KaplanMeierEstimatorType {
  NONE, // KME = 1.0 for all samples
  LEFT_CONTINUOUS, // Used when computing statistics (e.g. log-rank, cox)
  RIGHT_CONTINUOUS, // Used when just printing KME values
};

/* ================================ END Enums =============================== */

/* =============================== Structures =============================== */

// A structure that allows a general API that can represent an operation of one
// of four types: MathOperation, BooleanOperation, ComparisonOperation, or ArithmeticOperation.
struct OperationHolder {
  OperationType type_;
  MathOperation math_op_;
  BooleanOperation gate_op_;
  ComparisonOperation comparison_op_;
  ArithmeticOperation arithmetic_op_;
  CircuitOperation circuit_op_;

  OperationHolder() {
    type_ = OperationType::UNKNOWN;
  }
  // Rule of 5 Constructors.
  //  - Copy constructor.
  OperationHolder(const OperationHolder & other) {
    type_ = other.type_;
    math_op_ = other.math_op_;
    gate_op_ = other.gate_op_;
    comparison_op_ = other.comparison_op_;
    arithmetic_op_ = other.arithmetic_op_;
  }
  //  - Move Constructor.
  OperationHolder(OperationHolder && other) {
    type_ = other.type_;
    math_op_ = other.math_op_;
    gate_op_ = other.gate_op_;
    comparison_op_ = other.comparison_op_;
    arithmetic_op_ = other.arithmetic_op_;
  }
  //  - Destructor.
  ~OperationHolder() noexcept {
  }
  //  - Copy-Assignment.
  OperationHolder & operator=(const OperationHolder & other) {
    OperationHolder tmp(other); // Re-use copy-constructor.
    *this = std::move(tmp); // Re-use move-assignment.
    return *this;
  }
  //  - Move-Assignment.
  OperationHolder & operator=(OperationHolder && other) {
    type_ = other.type_;
    math_op_ = other.math_op_;
    gate_op_ = other.gate_op_;
    comparison_op_ = other.comparison_op_;
    arithmetic_op_ = other.arithmetic_op_;
    return *this;
  }

  // Returns whether exactly one of the is_* fields is true.
  bool IsValid() const {
    return type_ != OperationType::UNKNOWN;
  }
};

inline bool IsSingleInputOperation(const OperationHolder op) {
  if (op.type_ == OperationType::MATH)
    return IsSingleInputOperation(op.math_op_);
  if (op.type_ == OperationType::BOOLEAN)
    return IsSingleInputOperation(op.gate_op_);
  if (op.type_ == OperationType::ARITHMETIC)
    return IsSingleInputOperation(op.arithmetic_op_);
  if (op.type_ == OperationType::CIRCUIT_GATE)
    return IsSingleInputOperation(op.circuit_op_);
  // To reach here, op type must be ComparisonOperation, for which all
  // operators are double-input.
  return false;
}

inline bool IsDoubleInputOperation(const OperationHolder op) {
  if (op.type_ == OperationType::MATH)
    return IsDoubleInputOperation(op.math_op_);
  if (op.type_ == OperationType::BOOLEAN)
    return IsDoubleInputOperation(op.gate_op_);
  if (op.type_ == OperationType::ARITHMETIC)
    return IsDoubleInputOperation(op.arithmetic_op_);
  if (op.type_ == OperationType::CIRCUIT_GATE)
    return IsDoubleInputOperation(op.circuit_op_);
  // To reach here, op type must be ComparisonOperation, for which all
  // operators are double-input.
  return true;
}

// A helper function to get the actual lvalue that a (inheriting child of)
// GenericDataType represents. Type 'U' should be an inheriting child of
// GenericDataType.
// Note that this function should probably never be called by an external/new
// function, except for where it is currently used in data_structures.cpp.
template<typename T, typename U>
const T & GetValue(const U & input) {
  return input.value_;
}

// An empty container that will represent an arbitrary lvalue.
// NOTE: An alternative design that utilizes a templated struct there, where the template
// type(name) encapsulates the data type, was considered:
//   template <typename T>
//   GenericDataType {
//     T value_;
//   };
// The reason this design was abandoned is because in order to have the 'value_'
// field of GenericValue, which is a (smart) pointer to a GenericDataType, we would
// need to template that 'value_' field, which would mean that GenericValue itself
// would have to be templated, which defeats much of the purpose of using it,
// where e.g. when describing inputs as a vector<GenericValue>, this would force
// all elements of the vector to have the same type, which is not desired for
// most current use-cases of GenericValue.
// Instead, we use a generic parent class 'GenericDataType', which gets instantiated
// by pecific data types; and then rely on the above 'GetValue()' API to return
// the appropriate value.
struct GenericDataType {
  virtual GenericDataType * clone() const = 0;

  virtual ~GenericDataType() = default;
};

struct StringDataType : public GenericDataType {
  std::string value_;
  StringDataType() {
    value_ = "";
  }
  StringDataType(const std::string & value) {
    value_ = value;
  }
  StringDataType * clone() const override {
    return new StringDataType(*this);
  }
};

struct BoolDataType : public GenericDataType {
  bool value_;
  BoolDataType() {
    value_ = false;
  }
  BoolDataType(const bool value) {
    value_ = value;
  }
  BoolDataType * clone() const override {
    return new BoolDataType(*this);
  }
};

struct Int2DataType : public GenericDataType {
  char value_;
  Int2DataType() {
    value_ = 0;
  }
  Int2DataType(const char value) {
    value_ = value;
  }
  Int2DataType * clone() const override {
    return new Int2DataType(*this);
  }
};

struct Uint2DataType : public GenericDataType {
  unsigned char value_;
  Uint2DataType() {
    value_ = 0;
  }
  Uint2DataType(const unsigned char value) {
    value_ = value;
  }
  Uint2DataType * clone() const override {
    return new Uint2DataType(*this);
  }
};

struct Int4DataType : public GenericDataType {
  char value_;
  Int4DataType() {
    value_ = 0;
  }
  Int4DataType(const char value) {
    value_ = value;
  }
  Int4DataType * clone() const override {
    return new Int4DataType(*this);
  }
};

struct Uint4DataType : public GenericDataType {
  unsigned char value_;
  Uint4DataType() {
    value_ = 0;
  }
  Uint4DataType(const unsigned char value) {
    value_ = value;
  }
  Uint4DataType * clone() const override {
    return new Uint4DataType(*this);
  }
};

struct Int8DataType : public GenericDataType {
  char value_;
  Int8DataType() {
    value_ = 0;
  }
  Int8DataType(const char value) {
    value_ = value;
  }
  Int8DataType * clone() const override {
    return new Int8DataType(*this);
  }
};

struct Uint8DataType : public GenericDataType {
  unsigned char value_;
  Uint8DataType() {
    value_ = 0;
  }
  Uint8DataType(const unsigned char value) {
    value_ = value;
  }
  Uint8DataType * clone() const override {
    return new Uint8DataType(*this);
  }
};

struct Int16DataType : public GenericDataType {
  short value_;
  Int16DataType() {
    value_ = 0;
  }
  Int16DataType(const short & value) {
    value_ = value;
  }
  Int16DataType * clone() const override {
    return new Int16DataType(*this);
  }
};

struct Uint16DataType : public GenericDataType {
  unsigned short value_;
  Uint16DataType() {
    value_ = 0;
  }
  Uint16DataType(const unsigned short & value) {
    value_ = value;
  }
  Uint16DataType * clone() const override {
    return new Uint16DataType(*this);
  }
};

struct Int32DataType : public GenericDataType {
  int32_t value_;
  Int32DataType() {
    value_ = 0;
  }
  Int32DataType(const int32_t & value) {
    value_ = value;
  }
  Int32DataType * clone() const override {
    return new Int32DataType(*this);
  }
};

struct Uint32DataType : public GenericDataType {
  uint32_t value_;
  Uint32DataType() {
    value_ = 0;
  }
  Uint32DataType(const uint32_t & value) {
    value_ = value;
  }
  Uint32DataType * clone() const override {
    return new Uint32DataType(*this);
  }
};

struct Int64DataType : public GenericDataType {
  int64_t value_;
  Int64DataType() {
    value_ = 0;
  }
  Int64DataType(const int64_t & value) {
    value_ = value;
  }
  Int64DataType * clone() const override {
    return new Int64DataType(*this);
  }
};

struct Uint64DataType : public GenericDataType {
  uint64_t value_;
  Uint64DataType() {
    value_ = 0;
  }
  Uint64DataType(const uint64_t & value) {
    value_ = value;
  }
  Uint64DataType * clone() const override {
    return new Uint64DataType(*this);
  }
};

struct SliceDataType : public GenericDataType {
  math_utils::slice value_;
  SliceDataType() {
    value_ = 0;
  }
  SliceDataType(const slice & value) {
    value_ = value;
  }
  SliceDataType * clone() const override {
    return new SliceDataType(*this);
  }
};

struct DoubleDataType : public GenericDataType {
  double value_;
  DoubleDataType() {
    value_ = 0.0;
  }
  DoubleDataType(const double & value) {
    value_ = value;
  }
  DoubleDataType * clone() const override {
    return new DoubleDataType(*this);
  }
};

// A generic data type container that can represent any of the supported
// Data Types (see DataType enum above). Only a single field of the structure
// should be populated (non-trivial).
struct GenericValue {
public:
  DataType type_;
  std::unique_ptr<GenericDataType> value_;

  // Default constructor.
  GenericValue() {
    type_ = DataType::UNKNOWN;
    value_ = nullptr;
  }
  GenericValue(const GenericDataType & value) {
    value_.reset(value.clone());
  }

  // Rule of 5 Constructors.
  //  - Copy constructor.
  GenericValue(const GenericValue & other) {
    type_ = other.type_;
    value_ = other.value_ == nullptr ?
        nullptr :
        std::unique_ptr<GenericDataType>(other.value_->clone());
  }
  //  - Move Constructor.
  GenericValue(GenericValue && other) noexcept {
    type_ = other.type_;
    value_ = std::move(other.value_);
  }
  //  - Destructor.
  ~GenericValue() noexcept {
  }
  //  - Copy-Assignment.
  GenericValue & operator=(const GenericValue & other) {
    GenericValue tmp(other); // Re-use copy-constructor.
    *this = std::move(tmp); // Re-use move-assignment.
    return *this;
  }
  //  - Move-Assignment.
  GenericValue & operator=(GenericValue && other) noexcept {
    type_ = other.type_;
    value_ = std::move(other.value_);
    return *this;
  }

  // Constructor with just the DataType specified.
  explicit GenericValue(const DataType type) : GenericValue() {
    type_ = type;
  }

  // Per-type constructors.
  //   - STRING
  explicit GenericValue(const std::string & value) : GenericValue() {
    SetStringGenericValue(value);
  }
  //   - BOOL
  explicit GenericValue(const bool value) : GenericValue() {
    type_ = DataType::BOOL;
    value_.reset(new BoolDataType(value));
  }
  //   - INT8
  explicit GenericValue(const char value) : GenericValue() {
    type_ = DataType::INT8;
    value_.reset(new Int8DataType(value));
  }
  //   - UINT8
  explicit GenericValue(const unsigned char value) : GenericValue() {
    type_ = DataType::UINT8;
    value_.reset(new Uint8DataType(value));
  }
  //   - INT16
  explicit GenericValue(const short value) : GenericValue() {
    type_ = DataType::INT16;
    value_.reset(new Int16DataType(value));
  }
  //   - UINT16
  explicit GenericValue(const unsigned short value) : GenericValue() {
    type_ = DataType::UINT16;
    value_.reset(new Uint16DataType(value));
  }
  //   - INT32
  explicit GenericValue(const int32_t value) : GenericValue() {
    type_ = DataType::INT32;
    value_.reset(new Int32DataType(value));
  }
  //   - UINT32
  explicit GenericValue(const uint32_t value) : GenericValue() {
    type_ = DataType::UINT32;
    value_.reset(new Uint32DataType(value));
  }
  //   - INT64
  explicit GenericValue(const int64_t & value) : GenericValue() {
    type_ = DataType::INT64;
    value_.reset(new Int64DataType(value));
  }
  //   - UINT64
  explicit GenericValue(const uint64_t & value) : GenericValue() {
    type_ = DataType::UINT64;
    value_.reset(new Uint64DataType(value));
  }
  //   - SLICE
  //     Note: since slice is actually just an uint32_t or uint64_t, we cannot
  //     have the same constructor API as for all the other DataTypes (due to
  //     ambiguity); so we artificially add an input parameter to distinguish it.
  GenericValue(const math_utils::slice & value, const DataType & type) :
      GenericValue() {
    if (type != DataType::SLICE)
      log_fatal("Bad use of 'slice' constructor.");
    type_ = type;
    value_.reset(new SliceDataType(value));
  }
  //   - DOUBLE
  explicit GenericValue(const double & value) : GenericValue() {
    type_ = DataType::DOUBLE;
    value_.reset(new DoubleDataType(value));
  }
  //   - Vector of values.
  //     This is implemented in the .cpp, since it will need to know fields/fns
  //     of VectorDataType, which is itself defined *lower* in this file (and
  //     hence cannot be used here); note the it appears lower because it
  //     itself needs to know about GenericValue in order to use it as the
  //     underlying data type of the vector elements.
  explicit GenericValue(const std::vector<GenericValue> & value);

  // Constructor for (signed) numeric type with DataType and value specified.
  GenericValue(const DataType type, const int64_t & value) :
      GenericValue() {
    type_ = type;
    if (type == DataType::BOOL) {
      if (value != 1 && value != 0) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new BoolDataType((bool)value));
    } else if (type == DataType::INT2) {
      if (value > 1 || value < -2) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Int2DataType((char)value));
    } else if (type == DataType::UINT2) {
      if (value > 3 || value < 0) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Uint2DataType((unsigned char)value));
    } else if (type == DataType::INT4) {
      if (value > 7 || value < -8) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Int4DataType((char)value));
    } else if (type == DataType::UINT4) {
      if (value > 15 || value < 0) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Uint4DataType((unsigned char)value));
    } else if (type == DataType::INT8) {
      if (value > SCHAR_MAX || value < SCHAR_MIN) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Int8DataType((char)value));
    } else if (type == DataType::UINT8) {
      if (value > UCHAR_MAX || value < 0) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Uint8DataType((unsigned char)value));
    } else if (type == DataType::INT16) {
      if (value > SHRT_MAX || value < SHRT_MIN) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Int16DataType((short)value));
    } else if (type == DataType::UINT16) {
      if (value > USHRT_MAX || value < 0) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Uint16DataType((unsigned short)value));
    } else if (type == DataType::INT32) {
      if (value > LONG_MAX || value < LONG_MIN) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Int32DataType((int32_t)value));
    } else if (type == DataType::UINT32) {
      if (value > ULONG_MAX || value < 0) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Uint32DataType((uint32_t)value));
    } else if (type == DataType::INT64) {
      if (value > LLONG_MAX || value < LLONG_MIN) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Int64DataType((int64_t)value));
    } else if (type == DataType::UINT64) {
      if (value > ULLONG_MAX || value < 0) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Uint64DataType((uint64_t)value));
    } else if (type == DataType::SLICE) {
      if (value > ULLONG_MAX || value < 0) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new SliceDataType((slice)value));
    } else {
      log_fatal(
          "Unsupported constructor for %s",
          GetDataTypeString(type).c_str());
    }
  }

  // Constructor for (unsigned) numeric type with DataType and value specified.
  GenericValue(const DataType type, const uint64_t & value) :
      GenericValue() {
    type_ = type;
    if (type == DataType::BOOL) {
      if (value > 1) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new BoolDataType((bool)value));
    } else if (type == DataType::INT2) {
      if (value > 1) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Int2DataType((char)value));
    } else if (type == DataType::UINT2) {
      if (value > 3) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Uint2DataType((unsigned char)value));
    } else if (type == DataType::INT4) {
      if (value > 7) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Int4DataType((char)value));
    } else if (type == DataType::UINT4) {
      if (value > 15) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Uint4DataType((unsigned char)value));
    } else if (type == DataType::INT8) {
      if (value > SCHAR_MAX) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Int8DataType((char)value));
    } else if (type == DataType::UINT8) {
      if (value > UCHAR_MAX) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Uint8DataType((unsigned char)value));
    } else if (type == DataType::INT16) {
      if (value > SHRT_MAX) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Int16DataType((short)value));
    } else if (type == DataType::UINT16) {
      if (value > USHRT_MAX) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Uint16DataType((unsigned short)value));
    } else if (type == DataType::INT32) {
      if (value > LONG_MAX) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Int32DataType((int32_t)value));
    } else if (type == DataType::UINT32) {
      if (value > ULONG_MAX) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Uint32DataType((uint32_t)value));
    } else if (type == DataType::INT64) {
      if (value > LLONG_MAX) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Int64DataType((int64_t)value));
    } else if (type == DataType::UINT64) {
      if (value > ULLONG_MAX) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new Uint64DataType((uint64_t)value));
    } else if (type == DataType::SLICE) {
      if (value > ULLONG_MAX) {
        log_fatal(
            "%s too big for %s",
            string_utils::Itoa(value).c_str(),
            GetDataTypeString(type).c_str());
      }
      value_.reset(new SliceDataType((slice)value));
    } else {
      log_fatal(
          "Unsupported constructor for %s",
          GetDataTypeString(type).c_str());
    }
  }

  GenericValue * clone() const {
    return new GenericValue(*this);
  }

  // Operator overloads.
  // NOTE: There are two ways we can imagine comparing two GenericValues:
  //   - Compare value_ only
  //   - Compare value_ and type_
  // Neither is inherently the "right" interpretation, but for most applications,
  // user probably just wants to compare value_. Thus, the following just check
  // the value_ field (thus, an INT4 type_ with value_ = 5 will have the == operator
  // return true for UINT16 type_ with value_ = 5).
  // If user wants to also compare the type_, user can simply do so directly (by
  // comparing the .type_ fields); or for equality, use the Equals() function below.
  bool operator==(const GenericValue & other) const;
  bool operator!=(const GenericValue & other) const;
  bool operator<(const GenericValue & other) const;
  bool operator<=(const GenericValue & other) const;
  bool operator>(const GenericValue & other) const;
  bool operator>=(const GenericValue & other) const;
  // Similar to operator== above, but also demands type_ is the same.
  bool Equals(const GenericValue & other) const;
  // Math Operation overloads.
  GenericValue & operator+=(const GenericValue & other);
  GenericValue & operator-=(const GenericValue & other);
  GenericValue & operator*=(const GenericValue & other);
  GenericValue & operator/=(const GenericValue & other);
  GenericValue & operator%=(const GenericValue & other);
  friend GenericValue
  operator+(GenericValue left, const GenericValue & right) {
    left += right;
    return left;
  }
  friend GenericValue
  operator-(GenericValue left, const GenericValue & right) {
    left -= right;
    return left;
  }
  friend GenericValue
  operator*(GenericValue left, const GenericValue & right) {
    left *= right;
    return left;
  }
  friend GenericValue
  operator/(GenericValue left, const GenericValue & right) {
    left /= right;
    return left;
  }
  friend GenericValue
  operator%(GenericValue left, const GenericValue & right) {
    left %= right;
    return left;
  }
  // Note that additional operators are available via the MergeValuesViaOperator()
  // and the ApplyOperator() API's; see below.

  void SetStringGenericValue(const std::string & value) {
    const size_t length = value.length();
    if (length > 128) {
      log_fatal(
          "Unsupported string length (%s):\n%s",
          string_utils::Itoa(length).c_str(),
          value.substr(0, kManyCharsToPrint).c_str());
    }
    value_.reset(new StringDataType(value));
    if (length <= 8) {
      type_ = DataType::STRING8;
    } else if (length <= 16) {
      type_ = DataType::STRING16;
    } else if (length <= 24) {
      type_ = DataType::STRING24;
    } else if (length <= 32) {
      type_ = DataType::STRING32;
    } else if (length <= 64) {
      type_ = DataType::STRING64;
    } else {
      type_ = DataType::STRING128;
    }
  }
};

struct VectorDataType : public GenericDataType {
  std::vector<GenericValue> value_;
  VectorDataType() {
  }
  VectorDataType(const std::vector<GenericValue> & value) {
    value_.resize(value.size());
    for (size_t i = 0; i < value.size(); ++i) {
      value_[i] = value[i];
    }
  }
  VectorDataType * clone() const override {
    return new VectorDataType(*this);
  }
};

// Parameters that specify how a given variable should be sampled.
struct SamplingParameters {
public:
  // Name of the variable, as it appears in the model.
  std::string variable_name_;
  // User may want to specify one distribution type for some samples, and
  // a different one for other samples. The next two fields allow the user
  // to specify the range of samples to apply this distribution type to.
  int first_sample_;
  int last_sample_;

  // Type of distribution.
  Distribution type_;
  // Range of distribution.
  double range_start_;
  double range_end_;
  // The following can be the mean (e.g. for Normal distribution), or represent
  // a different paramter to the distribution (e.g. for Poisson, 'lambda' would
  // be stored in 'mean_' field).
  double mean_;
  // The second parameter of a distribution, typically (always?) actually does
  // represent standard deviation.
  double std_dev_;
  // A constant multiplier for the distribution.
  double constant_;

  SamplingParameters() {
    variable_name_ = "";
    first_sample_ = -1;
    last_sample_ = -1;
    range_start_ = 0.0;
    range_end_ = 0.0;
    mean_ = 0.0;
    std_dev_ = 0.0;
    constant_ = 1.0;
  }
};

// Each element of input data will have this type. It is general enough to
// encapsulate either double (e.g. ratio) or string (e.g. nominal or ordinal)
// values.
// For any particular DataHolder item, either value_ should equal 0.0, or
// name_ should equal "". If both are true, we use the double value_ (i.e.
// we assume the item is a double).
struct DataHolder {
public:
  double value_;
  std::string name_;
  StatsDataType type_;

  DataHolder() {
    value_ = 0.0;
    name_ = "";
    type_ = StatsDataType::DATA_TYPE_UNKNOWN;
  }
};

// Holds the info for the "dependent variable" for the Cox Proportional Hazards
// Model.
struct CensoringData {
public:
  double survival_time_;
  double censoring_time_;
  bool is_alive_;

  CensoringData() {
    survival_time_ = DBL_MIN;
    censoring_time_ = DBL_MIN;
  }
};

// Holds the info for the "dependent variable" for the Interval-Censored Surival
// Model.
struct IntervalCensoredData {
public:
  double left_time_;
  double right_time_;

  IntervalCensoredData() {
    left_time_ = 0.0;
    right_time_ = std::numeric_limits<double>::infinity();
  }
};

// Holds all possible information for a given row (Sample, Patient, etc.) of
// input data.
struct DataRow {
  std::string id_;
  double weight_;
  std::vector<std::string> families_;
  double dep_var_value_;
  CensoringData cox_dep_var_value_;
  // Note: This vector is meaningful only if there is a corresponding
  // vector of variable names indicating which column each index corresponds to.
  std::vector<double> indep_var_values_;

  DataRow() {
    id_ = "";
    weight_ = -1.0;
    dep_var_value_ = DBL_MIN;
  }
};

// Holds information necessary to find a root (solution) of an equation.
struct RootFinder {
  // Fields used in the algorithm to find solution.
  double value_at_prev_guess_;
  double closest_neg_;
  double closest_pos_;
  double orig_guess_;
  double curr_guess_;
  double prev_guess_;

  // Fields to determine convergence to the root. At least one of the following
  // three booleans (not counting the last one) must be true.
  bool use_absolute_distance_to_zero_;
  double absolute_distance_to_zero_;
  bool use_absolute_distance_from_prev_guess_;
  double absolute_distance_from_prev_guess_;
  bool use_relative_distance_from_prev_guess_;
  double relative_distance_from_prev_guess_;
  // Whether midpoint method is required to have found a valid interval [a, b],
  // such that f(a) and f(b) have opposite signs, before returning.
  bool demand_pos_and_neg_values_found_;

  // Fields tracking number of iterations.
  int iteration_index_;
  int max_iterations_;

  // Fields that store information (to print) about how the algorithm did.
  std::string summary_info_;
  std::string debug_info_;
  std::string error_msg_;

  RootFinder() {
    // Note that there is no reason why closest_neg_ should be set to neg
    // infinity (DBL_MIN) and closest_pos_ to pos infinity (DBL_MAX), i.e.
    // a priori we don't know what the function approaches at both ends.
    // However, it is convenient to set these to something (so that we
    // can determine if they have been set for real yet), and these choices
    // have the advantage of being different from each other, as well as
    // matching intuition of the basic function y = x. Note that when using
    // these, it should NOT be assumed that closest_neg_ < closest_pos_.
    closest_neg_ = DBL_MIN;
    closest_pos_ = DBL_MAX;
    value_at_prev_guess_ = DBL_MIN;
    curr_guess_ = 0.0;
    orig_guess_ = 0.0;
    prev_guess_ = DBL_MIN;

    use_absolute_distance_to_zero_ = true;
    absolute_distance_to_zero_ = EPSILON;
    use_absolute_distance_from_prev_guess_ = false;
    absolute_distance_from_prev_guess_ = EPSILON;
    use_relative_distance_from_prev_guess_ = true;
    relative_distance_from_prev_guess_ = EPSILON;
    demand_pos_and_neg_values_found_ = true;

    iteration_index_ = 0;
    max_iterations_ = 100;

    summary_info_ = "";
    debug_info_ = "";
    error_msg_ = "";
  }

  void Reset() {
    closest_neg_ = DBL_MIN;
    closest_pos_ = DBL_MAX;
    value_at_prev_guess_ = DBL_MIN;
    curr_guess_ = orig_guess_;
    prev_guess_ = DBL_MIN;
    iteration_index_ = 0;
  }
};

// A generic structure that can represent a constant value, a variable,
// a term in an expression, or a full expression (function). Structure:
//   - If op_ = IDENTITY, then exactly one of {var_name_, value_} should be set
//     (depending on whether this is a variable or a constant), and
//     subterm_one_ and subterm_two_ should both be NULL
//   - For any other operation op_, var_name_ should be empty and value_ should
//     be NULL; either one or both of {subterm_one_, subterm_two_} should be set:
//   - If op_ is EXP, LOG, SQRT, SIN, COS, or TAN, then subterm_two_ should be NULL
//   - If op_ is ADD, SUB, MULT, DIV, or POW, then subterm_one_ and subterm_two_
//     should both be non-null.
// Example:
//   log(2x^2 + 3x - sin(x))
// Then:
//   Outermost Expression log(2x^2 + 3x - sin(x)):
//     op_ = LOG, subterm_one_ = &A
//   Expression A (2x^2 + 3x - sin(x)):
//     op_ = ADD, subterm_one_ = &B, subterm_two_ = &C
//   Expression B (2x^2 + 3x):
//     op_ = ADD, subterm_one_ = &D, subterm_two_ = &E
//   Expression C (-sin(x)):
//     op_ = MULT, subterm_one_ = &F, subterm_two_ = &G
//   Expression D (2x^2):
//     op_ = MULT, subterm_one_ = &H, subterm_two_ = &I
//   Expression E (3x):
//     op_ = MULT, subterm_one_ = &J, subterm_two_ = &K
//   Expression F (-1):
//     op_ = IDENTITY, value_ = -1.0
//   Expression G (sin(x)):
//     op_ = SIN, subterm_one_ = &L
//   Expression H (2):
//     op_ = IDENTITY, value_ = 2.0
//   Expression I (x^2):
//     op_ = POW, subterm_one_ = &M, subterm_two_ = &N
//   Expression J (3):
//     op_ = IDENTITY, value_ = 3.0
//   Expression K (x):
//     op_ = IDENTITY, var_name_ = "x"
//   Expression L (x):
//     op_ = IDENTITY, var_name_ = "x"
//   Expression M (x):
//     op_ = IDENTITY, var_name_ = "x"
//   Expression N (^2):
//     op_ = IDENTITY, value_ = 2.0
//
// Expressions can also be used to encapsulate Indicator functions and boolean
// expressions. For example:
//   I(X \in [a, b])
// Then:
//   Outermost Expression: I(X \in [a, b]) = I(X >= a) * I(X <= b)
//     op_ = MULT, subterm_one = &A, subterm_two = &B
//   Expression A: I(X >= a):
//     op_ = INDICATOR, subterm_one = &C
//   Expression B: I(X <= b):
//     op_ = INDICATOR, subterm_one = &D
//   Expression C: X >= a:
//     op_ = GTE, subterm_one = &E, subterm_two = &F
//   Expression D: X <= b:
//     op_ = LTE, subterm_one = &G, subterm_two = &H
//   Expression E: X
//     op_ = IDENTITY, name_ = X
//   Expreesion F: a:
//     op_ = IDENTITY, value_ = a
//   Expression G: X
//     op_ = IDENTITY, name_ = X
//   Expreesion H: b:
//     op_ = IDENTITY, value_ = b
// Note: To do X | Y (for binary variables X and Y): X | Y = X + Y - X * Y
struct Expression {
  // The operation to apply to the value (e.g. 'IDENTITY' or 'LOG').
  MathOperation op_;

  // Indicates the title of the variable name, or empty if this expression is
  // a numeric value (i.e. a constant).
  std::string var_name_;

  // Indicates the value of this variable.
  double value_;
  // Indicates if this expression is a constant (i.e. ignore var_name_ and use value_).
  bool is_constant_;

  // More complicated equations can be handled by recursively combining Expressions.
  // NOTE: (Pointers to) Expressions coming from subterms should *always* be created
  // on the heap: This is becuase sometimes they need to be (in case we want to
  // generate an expression locally in a function, and use it outside, which would
  // be impossible if pointing to an Expression on the stack); and since sometimes
  // they need to be on the heap, we require that they are always on the heap, so
  // that we're not left with the bad scenario where user doesn't know if they
  // need to call 'delete' on the subterms (since there is no way in C++ to
  // determine if a pointer is heap or stack, see:
  // stackoverflow.com/questions/3230420/how-to-know-if-a-pointer-points-to-the-heap-or-the-stack
  std::unique_ptr<Expression> subterm_one_;
  std::unique_ptr<Expression> subterm_two_;

  Expression() {
    op_ = MathOperation::IDENTITY;
    var_name_ = "";
    value_ = 0.0;
    is_constant_ = false;
    subterm_one_ = nullptr;
    subterm_two_ = nullptr;
  }
  Expression(const Expression & e) {
    op_ = e.op_;
    var_name_ = e.var_name_;
    value_ = e.value_;
    is_constant_ = e.is_constant_;
    subterm_one_ = e.subterm_one_ == nullptr ?
        nullptr :
        std::unique_ptr<Expression>(e.subterm_one_->clone());
    subterm_two_ = e.subterm_two_ == nullptr ?
        nullptr :
        std::unique_ptr<Expression>(e.subterm_two_->clone());
  }
  Expression(Expression && e) = default;
  Expression & operator=(const Expression & e) {
    Expression temp(e); // Re-use copy-constructor.
    *this = std::move(temp); // Re-use move-assignment.
    return *this;
  }
  Expression * clone() const {
    return new Expression(*this);
  }
};

// Holds a sub-term of one of the additive terms in the linear regression
// formula. For example, for formula:
//   Y = c_0 + c_1 * Log(X_1) * X_2 + c_2 * X_2
// this struct will represent e.g. Log(X_1), or X_2.
// This struct represents the 'title' of the variable term (just describes the
// structure of the term) and the (self) operation applied to it. For example,
// for term Log(X_1), the term_title_ is "X_1" and op_ is LOG, while for term
// X_2, the term_title_ is "X_2" and op_ is IDENTITY.
struct VariableTerm {
  // Indicates the title of the variable name.
  std::string term_title_;

  // The operation to apply to the value (e.g. 'IDENTITY' or 'LOG').
  MathOperation op_;

  // In the case op_ indicates POW or EXP, this holds the power (exponent) to apply.
  double exponent_;

  // In case this variable should be simulated (as opposed to read in from
  // a file), this specifies the parameters for how it should be generated.
  SamplingParameters sampling_params_;
};

// Holds one of the additive terms in the linear regression formula.
// For example, for formula:
//   Y = c_0 + c_1 * Log(X_1) * X_2 + c_2 * X_2
// this struct will represent the first linear term: c_1 * Log(X_1) * X_2,
// or the second linear term c_2 * X_2.
struct LinearTerm {
  // The sub-terms comprising this linear term.
  // For example, if this LinearTerm is: c_1 * Log(X_1) * X_2, then terms_
  // will be of size two, with one variable term for Log(X_1), and one
  // for X_2. Or if the LinearTerm is c_2 * X_2, then terms_ is of size one
  // representing X_2. Or if the LinearTerm is c_0, then terms_ will have
  // size zero.
  std::vector<VariableTerm> terms_;

  // The operation to combine the sub-terms; only used if terms_.size() > 1.
  // Typically, this should be MULT.
  MathOperation op_;

  // The constant multiplier for this term.
  double constant_;
};
/* ============================= END Structures ============================= */

/* =============================== Functions ================================ */

/* ===================== Functions For Operations ===================== */

// Returns the enum corresponding to the given 'input' value, or UNKNOWN
// on failure to match.
extern BooleanOperation GetBooleanOperation(const std::string & input);
// Returns a string representation of the operation.
// NOTE: This is identical to GetOpString() (the API for BooleanOperation)
// below; we keep this around for old uses of it (though probably just as
// easy to update code to point to GetOpString).
extern std::string
GetBooleanOperationString(const BooleanOperation type);

// For 2-Term operations, returns the string representation of 'op';
// returns empty string if this is not a 2-Term operation.
extern std::string GetOpString(const MathOperation op);
extern std::string GetOpString(const BooleanOperation op);
extern std::string GetOpString(const ComparisonOperation op);
extern std::string GetOpString(const ArithmeticOperation op);
extern std::string GetOpString(const CircuitOperation op);
extern std::string GetOpString(const OperationHolder op);
// For 1-Term operations, returns the string representation of the
// provided string surrounded by 'op';
// returns empty string if this is not a 1-Term operation.
extern std::string
GetOpString(const MathOperation op, const std::string & argument);
extern std::string
GetOpString(const BooleanOperation op, const std::string & argument);
extern std::string
GetOpString(const ArithmeticOperation op, const std::string & argument);
// NOTE: No API for ComparisonOperation, since these are all 2-Term operations.

/* =============== Functions For DataHolder ================== */
// Returns the string representation of the DataHolder. If the DataHolder's
// type_ is not NUMERIC or STRING, returns empty string. If type_ is
// NUMERIC, will either return name_ if non-empty and use_name is true;
// otherwise it will do Itoa() at the desired precision.
extern std::string GetDataHolderString(
    const DataHolder & data, const bool use_name, const int precision);
// Same as above, but uses default use_name = false.
extern std::string
GetDataHolderString(const DataHolder & data, const int precision);
// Same as above, using default precision (6 digits) for double values.
extern std::string GetDataHolderString(const DataHolder & data);

/* =============== Functions For DataType and GenericValue ================== */

// Returns true if value.type_ is any of the STRINGXXX types.
inline bool IsStringDataType(const GenericValue & value) {
  return IsStringDataType(value.type_);
}
// Returns true if value.type_ is any of the [U]INTXX or SLICE or BOOL types.
inline bool IsIntegerDataType(const GenericValue & value) {
  return IsIntegerDataType(value.type_);
}
// Same as above, but also returns true for DOUBLE.
inline bool IsNumericDataType(const GenericValue & value) {
  return IsNumericDataType(value.type_);
}

// Returns whether 'subtype' can be considered to be of type 'parent_type':
//   1) Number of bits in subtype must be <= Number of bits in parent_type
//   2) If subtype is signed, so must be parent_type
extern bool
IsDataTypeSubType(const DataType subtype, const DataType parent_type);

// Returns the i^th bit of the appropriate field (as determined by input.type_)
// of input, where the i^th bit is for when the input's value is viewed as a
// binary string (respectively 2's complement, for signed integer types), and
// the index 'i' refers to the 2^i bit position; e.g. for i = 0, returns the
// least significant (right-most) bit.
// NOTE: This function is system-independent; i.e. no matter how the OS represents
// (signed) integers, GetBit() will operate in a consistent manner, by selecting
// the 2^i bit of the binary string (resp. 2's complement representation).
extern bool GetBit(const uint64_t & i, const GenericValue & input);
// Returns a char vector representing input's value. In particular, this char
// vector represents the 2's complement expression of the value, so the leading
// bit of the 0^th entry will be the -2^n bit, and the last byte will be the
// least-significant bits 2^7, 2^6, ..., 2^0 (where the 2^7 may be -2^7, in
// case GetValueNumBytes(input) = 1).
// NOTE 1: For String DataTypes, the length of the vector will be the length of
// the string, with the first (0th) entry the first character of the string, etc.
// NOTE 2: For DataTypes that are less than 1-byte (e.g. BOOL, [U]INT[4, 8]), the
// returned vector will have length 1, and the leading bits will be 0,
// independent of sign (i.e. the -2^n bit is in the proper spot, and the
// leading bits (> n) are all zero.
extern std::vector<unsigned char>
GetTwosComplementByteString(const GenericValue & input);

// Returns the number of bits needed to represent the value represented by 'input'.
extern uint64_t GetValueNumBits(const GenericValue & input);
// Same as above, for based on DataType.
extern uint64_t GetValueNumBits(const DataType type);
// Returns the number of bytes needed to represent the value represented by 'input'.
extern uint64_t GetValueNumBytes(const GenericValue & input);
// Same as above, for based on DataType.
extern uint64_t GetValueNumBytes(const DataType type);

// Opposite direction as above: Determines the smallest (Integer) DataType
// for the given number of bits.
extern bool GetIntegerDataType(
    const bool is_signed, const int num_bits, DataType * type);

// Checks whether the input string represents an integer, and if so and
// the input GenericValue 'value' is non-null, then it populates it:
// DataType is determined by either a suffix "_DATATYPE" on the input
// string, or by identifying the smallest Integer DataType that can store it.
// If input cannot be parsed as an integer, returns false (and does nothing with 'output').
extern bool
ParseIfInteger(const std::string & input, GenericValue * output);
// Same as above, but 'input' should have the negative sign stripped.
extern bool ParseIfInteger(
    const bool is_negative,
    const std::string & input,
    GenericValue * output);
// Same as above, but allows for more numeric representations (representing
// a double), e.g. scientific notation and decimals.
extern bool
ParseIfDouble(const std::string & input, GenericValue * output);
// Same as above, but 'input' should have the negative sign stripped.
extern bool ParseIfDouble(
    const bool is_negative,
    const std::string & input,
    GenericValue * output);

// Attempts to cast the input string as a GenericValue with the indicated type
// (uses Stoi/Stod for numeric types).
// Returns true on success.
extern bool ParseGenericValue(
    const DataType type,
    const std::string & input,
    GenericValue * output);
// Similar to above, but input string is the 2's Complement String representation of
// the value.
extern bool ParseGenericValueFromTwosComplementString(
    const DataType type,
    const std::string & binary_string,
    GenericValue * output);
// Same as above, where 2's Complement string is in vector<unsigned char> format.
// See comments above GetTwosComplementByteString() above for how the vector of
// bytes, which represents the 2's complement string, should be formatted; indeed,
// GetTwosComplementByteString() and ParseGenericValueFromTwosComplementString()
// are 'inverses' of each other: one followed by the other is the Identity.
extern bool ParseGenericValueFromTwosComplementString(
    const DataType type,
    const std::vector<unsigned char> & bytes,
    GenericValue * output);
// Same as above, where 2's Complement string is in vector<char> format.
extern bool ParseGenericValueFromTwosComplementString(
    const DataType type,
    const std::vector<char> & bytes,
    GenericValue * output);

// Returns the string representation of the GenericValue, based on its type_:
//   - For type_ == STRINGXX:        Returns name_
//   - For type_ == BOOL:            Returns "0" or "1" (based on bit_)
//   - For type_ == [U]INT[8 | 16 | 32 | 64]: Returns Itoa([u]int[8 | 16 | 32 | 64])
//   - For type_ == DOUBLE:          Returns Itoa(double_, precision)
//   - For type_ == VECTOR:          Returns (v0, v1, ..., vn), where each
//                                   coordinate vi is printed via these rules
//   - For any other type_ (e.g. UNKNOWN):          Returns empty string
extern std::string
GetGenericValueString(const GenericValue & input, const int precision);
// Same as above, using default precision (6 digits) for double values.
extern std::string GetGenericValueString(const GenericValue & input);

// Combines two given GenericValues via the specified operation.
// (For specific behavior, see the appropriate MergeValuesViaOperator() below
// that handles the underlying OpHolder's operation type).
// NOTE ON OUTPUT DATATYPE:
// For many of the MergeValuesViaOperator() API's below, there may be ambiguity
// on the output DataType, especially when the two inputs have *different*
// DataTypes. (In fact, even if DataTypes are the same, there is still a
// question if the output DataType should match it, e.g. if input types are
// both UINT32 and operation is MULT, should we: a) Throw Overflow Error
// if output value exceeds 32 bits; b) Allow output DataType to be UINT64;
// or c) View arithmetic mod Z_N (for N = 2^32 - 1)?) Another example:
// what if one DataType is INT4, and the other is UINT8, and op is SUB?
// Should we take output to be signed (since INT4 being signed means answer
// is possibly signed)? If so, Should it be INT4 (to match input type) or
// INT8 (to match num bits in the larger input) or INT16 (to account for
// all possible answers, since subtracting UINT8_MAX requires INT16)? Or
// should we have the output default to the larger of the two inputs
// (and hence be UINT8 for this case)? Or, should we determine the output
// type based on the actual computation/output value?
// I don't see a clear favorite, so I'll pick what seems reasonable for
// most use-cases, and stick to that convention:
//   I) If op is BooleanOperation:
//        NUM_BITS: MAX(NUM_BITS_1, NUM_BITS_2)
//        SIGN: Unsigned iff *both* inputs are unsigned
//        DISSCUSSION: When inputs are not BOOL, this means the op should be
//        applied bitwise, and hence it makes sense that the output should have
//        the same number of bits as the larger input. Signed vs. unsigned is
//        unclear (if inputs have different sign), but is unlikely to matter
//        (since applying a bool operation bitwise probably means user
//        ultimately only cares about the bit representation of these values
//        anyway), so we'll go ahead and say that the output will have signed
//        type if *either* input (even the smaller one) has signed type.
//  II) If op is ArithmeticOperation:
//        NUM_BITS: MAX(NUM_BITS_1, NUM_BITS_2)
//        SIGN: Unsigned iff *both* inputs are unsigned
//        DISSCUSSION: As discussed above, there is a potential issue even
//        if the two inputs have the same DataType. We want to respect the
//        use case that BOOL + BOOL = BOOL, i.e. Arithmetic mod Z_N. Then,
//        respecting arithmetic mod Z_N means that it also makes sense
//        that e.g. UINT2 + UINT4 should be Arithmetic mod Z_N, for
//        N = 2^4 - 1. Thus, UINT2 - UINT4 will also be Arithmetic mod Z_N.
//        By extension, arithmetic on *signed* datatypes will also be
//        interpretted as arithmetic mod Z_N, shifted (i.e. choose negative
//        coset representatives for half of the range). So, e.g. for two
//        INT2 = {-2, -1, 0, 1} inputs, we would have: -2 + -1 = 1. Two
//        inputs with opposite sign will take the larger number of bits,
//        but treat it as signed. Thus, you could have a wacky situation
//        where e.g. input one is UINT4 = 14 \in [0..15], and input two
//        is INT2 = 0, and then 14 + 0 = -2 is INT4, since 14 would be
//        expressed as -2 in INT4 = [-8..7].
// III) If op is ComparisonOperation:
//        OUTPUT_TYPE = BOOL
//        DISCUSSION: BOOL is the only output type that makes sense for comparison op.
//  IV) If op is CircuitOperation or MathOperation:
//        OUTPUT_TYPE = Default to the Output DataType of the corresponding
//                      Arithmetic, Boolean, or Comparison Operation.
//        DISCUSSION: The only currently supported CircuitOperation and MathOperation
//        operators have an analog in one of [Arithmetic | Boolean | Comparison]Operation,
//        so use the appropriate Output Rule (I) - (III) above.
extern bool MergeValuesViaOperator(
    const OperationHolder & op,
    const GenericValue & one,
    const GenericValue & two,
    GenericValue * out);
// Same as above, where op is explicitly a MathOperation.
// Currently, this is only supported for a subset of MathOperations, namely,
// the ones that have an equivalent ArithmeticOperation or ComparisonOperation.
// For specific behavior, see the appropriate MergeValuesViaOperator() below
// that handles ArithmeticOperation or ComparisonOperation.
extern bool MergeValuesViaOperator(
    const MathOperation & op,
    const GenericValue & one,
    const GenericValue & two,
    GenericValue * out);
// Same as above, where op is explicitly a BooleanOperation.
// NOTE: 'one' and 'two' need not be BOOL; nor do they need to have the same type.
// If either (or both) are non-bool, the operation is applied bitwise (if the two
// inputs have different number of bits, the smaller one is extended to have the
// same number of bits as the larger one, where this extension is 0-padding if
// the DataType of the shorter one is unsigned or if it is a postive value;
// otherwise (in accordance to 2's complement representation) it is a 1-padding).
// See Output Rule (I) above for discussion of output DataType.
extern bool MergeValuesViaOperator(
    const BooleanOperation & op,
    const GenericValue & one,
    const GenericValue & two,
    GenericValue * out);
// Same as above, where op is explicitly a ComparisonOperation.
// Inputs need not have same type; output is always of type BOOL.
extern bool MergeValuesViaOperator(
    const ComparisonOperation & op,
    const GenericValue & one,
    const GenericValue & two,
    GenericValue * out);
// Same as above, where op is explicitly an ArithmeticOperation.
// Inputs need not have same type.
// NOTE: Arithmetic is performed mod Z, for the appropriate Z (based on the
// output DataType). In paritcular, overflow is not an error (value will simply
// "wrap around"), and e.g. 1 + 1 = 0 when adding two BOOLs.
// See Output Rule (II) above for discussion of output DataType.
extern bool MergeValuesViaOperator(
    const ArithmeticOperation & op,
    const GenericValue & one,
    const GenericValue & two,
    GenericValue * out);
// Same as above, where op is explicitly an CircuitOperation.
// Note that all CircuitOperations are either a BooleanOperation or an
// ArithmeticOperation; this API just calls the MergeValuesViaOperator()
// (with the appropriate BooleanOperation vs. ArithmeticOperation) above.
extern bool MergeValuesViaOperator(
    const CircuitOperation & op,
    const GenericValue & one,
    const GenericValue & two,
    GenericValue * out);
// Same as above, where op is explicitly an CircuitOperation, and inputs are
// explicitly bools (useful for boolean circuit evaluation).
// NOTE: Should only be used if 'op' is one of the 2-in-1-out Boolean operators.
extern bool MergeValuesViaOperator(
    const CircuitOperation & op,
    const bool & one,
    const bool & two,
    bool * out);
// Same as above, for 1-term operations.
extern bool ApplyOperator(
    const OperationHolder & op,
    const GenericValue & one,
    GenericValue * out);
// Same as above, where op is explicitly a MathOperation.
extern bool ApplyOperator(
    const MathOperation & op,
    const GenericValue & one,
    GenericValue * out);
// Same as above, where op is explicitly a BooleanOperation.
extern bool ApplyOperator(
    const BooleanOperation & op,
    const GenericValue & one,
    GenericValue * out);
// Same as above, where op is explicitly an ArithmeticOperation.
extern bool ApplyOperator(
    const ArithmeticOperation & op,
    const GenericValue & one,
    GenericValue * out);
// Same as above, where op is explicitly a CircuitOperation.
extern bool ApplyOperator(
    const CircuitOperation & op,
    const GenericValue & one,
    GenericValue * out);
// NOTE: We don't have an 'ApplyOperator' API for ComparisonOperation,
// since all ComparisonOperation's are 2-term.

// Returns false if the DataType of input cannot be cast as int64_t:
//   - If underlying type is not an integer (STRINGXX or DOUBLE)
//   - If underlying type is too big to fit in int64_t (UINT64 and SLICE)
// Otherwise, casts the relevant integer field as an int64_t.
extern bool
GetSignedIntegerValue(const GenericValue & input, int64_t * output);
// Returns false if the DataType of input cannot be cast as uint64_t:
//   - If underlying type is not an integer (STRINGXX or DOUBLE)
//   - If underlying type is signed integer, and value is negative.
// Otherwise, casts the relevant (unsigned) integer field as an uint64_t.
extern bool
GetUnsignedIntegerValue(const GenericValue & input, uint64_t * output);
// Similar to above, but casts value as whatever template type is specified.
// In particular, no sanity-checks are done (except that input.type_ has Integer type).
template<typename value_t>
inline bool
GetIntegerValue(const GenericValue & input, value_t * output) {
  if (!IsIntegerDataType(input)) {
    return false;
  }
  if (input.type_ == DataType::UINT64) {
    *output =
        GetValue<uint64_t>(*((Uint64DataType *)input.value_.get()));
  } else if (input.type_ == DataType::INT64) {
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
    return false;
  }
  return true;
}
/* ============= END Functions For DataType and GenericValue ================ */

/* ================= Functions For Expression ================ */
// Returns the string representation of 'term' (considers term.op_
// and term.term_title_).
extern std::string GetExpressionString(const Expression & expression);

// Rewrites all coefficients to have a multiplication sign, e.g. 2x -> 2 * x.
// Also rewrites xx as x * x.
// DEPRECATED. This used to be only used in ParseExpression(), which no
// longer uses it.
extern void
RewriteVariables(const std::string & var_name, std::string * term);

// Strips leading/trailing parentheses recursively, if present.
// WARNING: Be careful when using this for e.g. parsing math formulas, as this
// blindly strips parentheses as long as there is a leading '(' and trailing ')',
// without regard to whether these parentheses actually match or not. For example:
//   (3 + 2) * (1 + 4)
// Then calling StripExtraneousParentheses() on the above string would indeed strip them,
// i.e. would retur '3 + 2) * (1 + 4', even though the stripped parentheses don't match.
// To only strip matching parentheses, use e.g. GetClosingParentheses() and/or
// CountOpenCloseParentheses() before stripping, to ensure they match.
inline std::string
StripExtraneousParentheses(const std::string & input) {
  std::string to_return = input;
  while (string_utils::HasPrefixString(to_return, "(") &&
         string_utils::HasSuffixString(to_return, ")")) {
    to_return = to_return.substr(1, to_return.length() - 2);
  }
  return to_return;
}
// Similar to above, with differences:
//   - Returns a bool (if an error was encountered while stripping, e.g. an open
//     grouping symbol prefix was found, but no matching closing symbol) instead
//     of the stripped string
//   - Checks more grouping symbols than just parentheses.
//   - Similar to above, does not sanity-check that the parentheses actually 'match'
extern bool StripEnclosingGroupingSymbols(
    const std::string & input, std::string * output);

// Given a string that starts with a parentheses, sets closing_pos to
// be the position of the closing parentheses. Returns false if string
// doesn't start with '(', or if closing (matching) parentheses is not found.
extern bool
GetClosingParentheses(const std::string & input, size_t * closing_pos);

// Given a pair of symbols (representing the left and right characters of
// grouping symbols, e.g. "{" and "}") and an input string (representing
// the suffix starting with the left symbol and everything after it):
// Finds the position of the matching (closing) symbol.
// NOTE: The position of the closing symbol might *not* be the first
// occurrence of that symbol within 'input', e.g. if there are additional
// open symbols first.
extern bool GetClosingSymbol(
    const std::string & input,
    const std::pair<char, char> grouping_symbols,
    size_t * closing_pos);

// Takes as input a string, and computes:
//   #(open parentheses) - #(closed parentheses)
// Notice this is not strictly speaking the number of unmatched parentheses, since e.g.
//   3 + 4) * (2
// will return '0', even though that formula actually has two sets of unmatched
// parentheses.
// If force_consistency is true, then will return false if there are ever
// more close parentheses than open, as parsing from left to right (so e.g.
// the above example would return false).
// Also, the original passed-in value of 'num_unmatched' (which must be non-negative)
// is used as a starting point, so that in the end it will equal:
//   (orig value of num_unmatched) + #(open parentheses) - #(closed parentheses)
extern bool CountOpenCloseParentheses(
    const bool force_consistency,
    const std::string & input,
    int * num_unmatched);

// The following parse a string and separate it into blocks based on the
// appropriate separator. This is more sophisticated than just doing a
// StringUtils::Split(), since the logic includes taking into account the
// structure of a mathematical formula/expression; e.g. if splitting around
// the "+" symbol, it will recognize that some symbols lie inside parentheses
// and thus should not be split around.
//   - Get Additive Terms (split around "+" and "-").
extern bool GetAdditiveTerms(
    const std::string & input,
    const std::string & current_term,
    std::vector<std::pair<std::string, int>> * terms);
//   - Get Multiplicative Terms (split around "*" and "/").
extern bool GetMultiplicativeTerms(
    const std::string & input,
    const std::string & current_term,
    std::vector<std::pair<std::string, int>> * terms);
//   - Get Argument List (split around ",").
extern bool GetArgumentTerms(
    const std::string & input,
    const std::string & current_term,
    std::vector<std::pair<std::string, int>> * terms);
//   - Get Exponent Base and Power.
extern bool GetExponentTerms(
    const std::string & input,
    const std::string & current_term,
    std::vector<std::pair<std::string, int>> * terms);
// DEPRECATED. Use GetAdditiveTerms() instead.
extern bool GetLinearTerms(
    const std::string & input,
    bool is_first_try,
    const std::string & current_term,
    std::vector<std::pair<std::string, bool>> * terms);

// The following are similar to the above, but demand that there be at most
// two terms found (a "left" term and a "right" term).
//   - Get two-term argument list.
extern bool GetCommaTerms(
    const std::string & input,
    std::string * first_term,
    std::string * second_term);
// DEPRECATED. Use GetExponentTerms() API above.
//   - Get Exponent Base and Power.
extern bool GetExponentTerms(
    const std::string & input,
    std::string * first_term,
    std::string * second_term);
//   - Get two terms separated by a BooleanOperator.
extern bool GetBooleanTerms(
    const std::string & input,
    std::string * first_term,
    std::string * second_term,
    BooleanOperation * op);
//   - Get two terms separated by a ComparisonOperator.
extern bool GetComparisonTerms(
    const std::string & input,
    std::string * first_term,
    std::string * second_term,
    ComparisonOperation * op);
// NOTE: We don't have a corresponding API for GetArithmeticTerms() because
// the arithmetic terms are sufficiently different from each other in terms
// of how they should be handled, so parsing a string w.r.t. the various
// ArithmeticOperators must be done manually; see e.g. ParseFormula().

// Returns true if the expression is empty (i.e. was initialized via the
// default constructor, but no fields were set).
extern bool IsEmptyExpression(const Expression & exp);

// Parses a string representation of an expression to an Expression.
extern bool ParseExpression(
    const bool clean_input,
    const std::string & term_str,
    const bool enforce_var_names,
    const std::set<std::string> & var_names,
    Expression * expression);
// Same as above, with vector instead of set.
inline bool ParseExpression(
    const bool clean_input,
    const std::string & term_str,
    const bool enforce_var_names,
    const std::vector<std::string> & var_names,
    Expression * expression) {
  std::set<std::string> names;
  for (const std::string & var_name : var_names)
    names.insert(var_name);
  return ParseExpression(
      clean_input, term_str, enforce_var_names, names, expression);
}
// Same as above, using "x" as the default variable.
inline bool ParseExpression(
    const std::string & term_str,
    const bool enforce_var_names,
    Expression * expression) {
  std::set<std::string> var_names;
  if (enforce_var_names) {
    var_names.insert("x");
  }
  return ParseExpression(
      true, term_str, enforce_var_names, var_names, expression);
}
// Same as above, using "x" as the default variable.
inline bool
ParseExpression(const std::string & term_str, Expression * expression) {
  std::set<std::string> var_names;
  var_names.insert("x");
  return ParseExpression(true, term_str, true, var_names, expression);
}

// Returns a copy of the input expression.
extern Expression CopyExpression(const Expression & expression);
// Same as above, with different API.
extern void CopyExpression(
    const Expression & expression, Expression * new_expression);

// Returns the string representation of 'term' (considers term.op_
// and term.term_title_).
extern std::string GetTermString(const VariableTerm & term);

// Returns the string representation of the input SamplingParameters. For
// example, if params.type_ = NORMAL and params.mean_ = 0 and params.std_dev_
// is 1.0; then would return: N(0.0, 1.0).
extern std::string
GetSamplingParamsString(const SamplingParameters & params);

// Returns:
//   op(value)
// where op is one of:
//   IDENTITY, EXP, SQRT, POW, or LOG
// In the case of 'POW', uses term.exponent_ as the exponent.
// If an error is encountered, it is printed (via cout), and 0.0 is returned.
extern double
ComputeSelfOperation(const VariableTerm & term, const double & value);

// Computes:
//   v1 op v2
// and stores the value in output.
// Currently, only supported values for 'op' are MULT and ADD.
extern bool ComputeGroupOperation(
    const MathOperation op,
    const double & v1,
    const double & v2,
    double * output);

/* ============================= END Functions ============================== */

} // namespace math_utils

#define LOG_UNCLUDE
#include <ff/logging.h>

#endif
