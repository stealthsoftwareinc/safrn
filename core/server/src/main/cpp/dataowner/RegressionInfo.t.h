/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

namespace ff {
namespace mpc {

template<typename Identity_T>
RegressionInfo<Identity_T>::RegressionInfo(
    size_t ownVertical,
    size_t verticalDV,
    size_t columnIndexDV,
    size_t columnIndexKey,
    const std::vector<size_t> & columnIndices_IV_OppFrom_DV_Vertical,
    const std::vector<size_t> & columnIndices_IV_SameAs_DV_Vertical,
    const Identity_T * revealer,
    const Identity_T * dealer) :
    ownVertical(ownVertical),
    verticalDV(verticalDV),
    columnIndexDV(columnIndexDV),
    columnIndexKey(columnIndexKey),
    columnIndices_IV_OppFrom_DV_Vertical(
        columnIndices_IV_OppFrom_DV_Vertical),
    columnIndices_IV_SameAs_DV_Vertical(
        columnIndices_IV_SameAs_DV_Vertical),
    num_IVs(
        this->columnIndices_IV_OppFrom_DV_Vertical.size() +
        this->columnIndices_IV_SameAs_DV_Vertical.size()),
    revealer(revealer),
    dealer(dealer) {
  if (((this->columnIndices_IV_OppFrom_DV_Vertical.size() *
        (this->columnIndices_IV_OppFrom_DV_Vertical.size() + 3)) /
       2) > ((this->columnIndices_IV_SameAs_DV_Vertical.size() *
              (this->columnIndices_IV_SameAs_DV_Vertical.size() + 5)) /
                 2 +
             1)) {
    this->payloadLength =
        (this->columnIndices_IV_OppFrom_DV_Vertical.size() *
         (this->columnIndices_IV_OppFrom_DV_Vertical.size() + 3)) /
        2;
  } else {
    this->payloadLength =
        ((this->columnIndices_IV_SameAs_DV_Vertical.size() *
          (this->columnIndices_IV_SameAs_DV_Vertical.size() + 5)) /
             2 +
         1);
  }
  size_t n = this->maxListSize;
  this->startModulus = nextPrime(static_cast<SmallNum>(
      (1 << (2UL * this->bitsOfPrecision + this->securityParameter)) +
      static_cast<size_t>(floor(n))));
  this->endModulus = nextPrime(static_cast<SmallNum>(
      1 << static_cast<size_t>(ceil(
          static_cast<double>(this->num_IVs) *
          (log2(this->num_IVs) + 2 * this->bitsOfPrecision +
           log2(n))))));
}

} // namespace mpc
} // namespace ff
