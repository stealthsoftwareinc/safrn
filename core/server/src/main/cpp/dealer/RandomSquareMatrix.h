/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef FF_RANDOM_SQUARE_MATRIX_H_
#define FF_RANDOM_SQUARE_MATRIX_H_

/* C and POSIX Headers */

/* C++ Headers */
#include <memory>
#include <vector>

/* 3rd Party Headers */

/* Fortissimo Headers */
#include <mpc/MatrixMult.h> // For Matrix.
#include <mpc/Randomness.h>

#include <mpc/templates.h>

/* Logging config */
#include <ff/logging.h>

namespace safrn {
namespace dealer {

template<typename Value_T, typename MatrixValue_T>
struct RandomSquareMatrixInfo;

template<typename Value_T>
struct RandomSquareMatrix {
public:
  RandomSquareMatrix(const size_t d) : values_(d, d) {
    d_ = d;
  }

  RandomSquareMatrix(RandomSquareMatrix const &) = default;
  RandomSquareMatrix & operator=(RandomSquareMatrix const &) = default;

  template<typename InfoValue_T>
  RandomSquareMatrix(
      RandomSquareMatrixInfo<InfoValue_T, Value_T> const & info);

  ff::mpc::Matrix<Value_T> values_;
  size_t d_;
  Value_T det_of_inverse_;

  static std::string name() {
    return std::string("Random Square Matrix");
  }
};

template<typename Value_T, typename MatrixValue_T>
struct RandomSquareMatrixInfo {
public:
  RandomSquareMatrixInfo(
      const size_t d, const Value_T & field_characteristic) {
    d_ = d;
    field_characteristic_ = field_characteristic;
  }

  RandomSquareMatrixInfo() = default;

  size_t instanceSize() const {
    return (
        sizeof(uint64_t) + // For d_
        sizeof(MatrixValue_T) * d_ * d_ + // For values_
        sizeof(MatrixValue_T)); // For det_of_inverse_.
  }

  void generate(
      size_t n_parties,
      size_t,
      std::vector<RandomSquareMatrix<MatrixValue_T>> & vals) const;

  bool operator==(RandomSquareMatrixInfo const & other) const {
    return (this->d_ == other.d_) &&
        (this->field_characteristic_ == other.field_characteristic_);
  }
  bool operator!=(RandomSquareMatrixInfo const & other) const {
    return !(*this == other);
  }

  size_t d_ = 0;
  Value_T field_characteristic_ = 0;
};

} // namespace dealer
} // namespace safrn

namespace ff {
// ==================== Helper functions: msg_read and msg_write ===============
// Code below instantiates msg_read/write for template type RandomSquareMatrix[Info].
template<typename Identity_T, typename Value_T, typename MatrixValue_T>
bool msg_read(
    ff::IncomingMessage<Identity_T> & msg,
    safrn::dealer::RandomSquareMatrixInfo<Value_T, MatrixValue_T> &
        input) {
  uint64_t local_d = 0;
  bool success = msg.template read<uint64_t>(local_d);
  input.d_ = (size_t)local_d;
  success = success &&
      msg.template read<Value_T>(input.field_characteristic_);

  return success;
}

template<typename Identity_T, typename Value_T, typename MatrixValue_T>
bool msg_write(
    ff::OutgoingMessage<Identity_T> & msg,
    safrn::dealer::
        RandomSquareMatrixInfo<Value_T, MatrixValue_T> const & input) {
  bool success = msg.template write<uint64_t>((uint64_t)input.d_);
  success = success &&
      msg.template write<Value_T>(input.field_characteristic_);
  return success;
}

template<typename Identity_T, typename Value_T>
bool msg_read(
    ff::IncomingMessage<Identity_T> & msg,
    safrn::dealer::RandomSquareMatrix<Value_T> & input) {
  uint64_t local_d = 0;
  bool success = msg.template read<uint64_t>(local_d);
  input.d_ = (size_t)local_d;
  success =
      success && msg.template read<Value_T>(input.det_of_inverse_);

  for (size_t row = 0; row < input.d_; ++row) {
    for (size_t col = 0; col < input.d_; ++col) {
      success = success &&
          msg.template read<Value_T>(input.values_.at(row, col));
    }
  }

  return success;
}

template<typename Identity_T, typename Value_T>
bool msg_write(
    ff::OutgoingMessage<Identity_T> & msg,
    safrn::dealer::RandomSquareMatrix<Value_T> const & input) {
  bool success = msg.template write<uint64_t>((uint64_t)input.d_);
  success =
      success && msg.template write<Value_T>(input.det_of_inverse_);

  for (size_t row = 0; row < input.d_; ++row) {
    for (size_t col = 0; col < input.d_; ++col) {
      success = success &&
          msg.template write<Value_T>(input.values_.at(row, col));
    }
  }

  return success;
}

} // namespace ff

#include <dealer/RandomSquareMatrix.t.h>

#define LOG_UNCLUDE
#include <ff/logging.h>

#endif
