/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

namespace safrn {
namespace dealer {

template<typename Value_T>
template<typename InfoValue_T>
RandomSquareMatrix<Value_T>::RandomSquareMatrix(
    RandomSquareMatrixInfo<InfoValue_T, Value_T> const & info) :
    RandomSquareMatrix(info.d_) {
}

template<typename Value_T, typename MatrixValue_T>
void RandomSquareMatrixInfo<Value_T, MatrixValue_T>::generate(
    size_t n_parties,
    size_t,
    std::vector<RandomSquareMatrix<MatrixValue_T>> & vals) const {
  vals.clear();
  vals.reserve(n_parties);
  for (size_t i = 0; i < n_parties; i++) {
    vals.emplace_back(RandomSquareMatrix<MatrixValue_T>(this->d_));
  }

  /* Step 1. Randomly create the "original" random matrix instance. */
  RandomSquareMatrix<MatrixValue_T> & orig = vals[0];
  for (size_t row = 0; row < this->d_; ++row) {
    for (size_t col = 0; col < this->d_; ++col) {
      orig.values_.at(row, col) = ff::mpc::randomModP<MatrixValue_T>(
          this->field_characteristic_);
    }
  }

  /* Step 2. compute determinant of inverse. */
  MatrixValue_T det = orig.values_.Det(this->field_characteristic_);
  orig.det_of_inverse_ = ff::mpc::modInvert<MatrixValue_T>(
      det, this->field_characteristic_);

  /* Step 3. randomly secret share the original. */
  for (size_t i = 1; i < n_parties; i++) {
    RandomSquareMatrix<MatrixValue_T> & share_i = vals[i];
    for (size_t row = 0; row < this->d_; ++row) {
      for (size_t col = 0; col < this->d_; ++col) {
        MatrixValue_T & current = share_i.values_.at(row, col);
        current = ff::mpc::randomModP<MatrixValue_T>(
            this->field_characteristic_);
        orig.values_.at(row, col) =
            (orig.values_.at(row, col) +
             (this->field_characteristic_ - current)) %
            (this->field_characteristic_);
      }
    }
    // Also, generate random share of the det(inverse).
    MatrixValue_T & det_share = share_i.det_of_inverse_;
    det_share =
        ff::mpc::randomModP<MatrixValue_T>(this->field_characteristic_);
    orig.det_of_inverse_ = (orig.det_of_inverse_ +
                            (this->field_characteristic_ - det_share)) %
        (this->field_characteristic_);
  }
}

} // namespace dealer
} // namespace safrn
