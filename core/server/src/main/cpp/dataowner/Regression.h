/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

/*
 * SAFRN Regression code
 */

#ifndef SAFRN_DATAOWNER_REGRESSION_H
#define SAFRN_DATAOWNER_REGRESSION_H

/* C and POSIX Headers */

/* C++ Headers */
#include <cmath>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

/* 3rd Party Headers */

/* Fortissimo Headers */
#include <ff/Fronctocol.h>
#include <ff/Message.h>

#include <dataowner/RegressionPayloadCompute.h>
#include <mpc/AbstractZipReduceFactory.h>
#include <mpc/Batch.h>
#include <mpc/ModConvUp.h>
#include <mpc/ObservationList.h>
#include <mpc/Randomness.h>
#include <mpc/RandomnessDealer.h>
#include <mpc/SISOSort.h>
#include <mpc/ZipAdjacent.h>
#include <mpc/ZipReduce.h>

#include <dataowner/Lookup.h>
#include <dataowner/RegressionInfo.h>
#include <dataowner/RegressionPatron.h>
#include <dataowner/fortissimo.h>
#include <framework/Framework.h>

#include <dealer/RandomSquareMatrix.h>

/* logging configuration */
#include <ff/logging.h>

namespace safrn {
namespace dataowner {

class Regression : public Fronctocol {
public:
  std::vector<LargeNum> outputWeightShares;

  LargeNum RSquaredShare;
  LargeNum meanSquareErrorShare;

  /*
   * Regression in SAFRN using Fortissimo shuffle and
   *
   */
  Regression(
      ff::mpc::ObservationList<LargeNum> && olist,
      std::string F_tableFile,
      std::string t_tableFile,
      GlobalInfo const * const globals,
      std::unique_ptr<const RegressionInfo> info);

  void init() override;

  void handleReceive(IncomingMessage & imsg) override;

  void handleComplete(Fronctocol & f) override;

  void handlePromise(Fronctocol & fronctocol) override;

  std::string name() override;

private:
  enum RegressionState {
    awaitingRandomnessAndSISOSort,
    awaitingRandomnessOnly,
    awaitingSISOSort,
    awaitingZipAdjacent,
    awaitingZipReduce,
    awaitingBatchedModConvUp,
    awaitingMatrixMultiply,
    awaitingMatrixReveal,
    awaitingRegressionMultiply,
    awaitingFirstCompare,
    awaitingFirstTypeCastFromBit,
    awaitingNegativeCorrectionMultiply,
    awaitingDivision,
    awaitingNegativeRecorrectionMultiply,
    awaitingFirstMultiplyForErrorTerms,
    awaitingSecondMultiplyForErrorTerms,
    awaitingThirdMultiplyForErrorTerms,
    awaitingFourthMultiplyForErrorTerms,
    awaitingFifthMultiplyForErrorTerms,
    awaitingDivisionForErrorTerms,
    awaitingCompareForFtStatistics,
    awaitingBatchedTypeCastFromBit,
    awaitingCompareForFtStatisticsEndModulus,
    awaitingBatchedTypeCastFromBitLast,
    awaitingBatchedLookup
  };

  RegressionState state = awaitingRandomnessAndSISOSort;

  RegressionRandomness randomness;

  void computePayloadVectorAndPadList();
  void setupCrossParties();
  void setupCrossVerticalShares();
  void shareWithCrossVerticalParties();
  void invokeRandomnessPatron();

  void
  rowReduceInTheClear(); // Probably just calls Zane's code, but that has old BIG_NUM stuff

  const std::string F_tableFile;
  const std::string t_tableFile;

  GlobalInfo const * const globals;
  std::unique_ptr<RegressionInfo const> const info;

  std::vector<RegressionPayloadComputeFactory> fronctocolFactories;
  std::vector<
      ff::mpc::ZipAdjacentInfo<safrn::Identity, LargeNum, SmallNum>>
      zipAdjacentInfo;

  ff::mpc::ObservationList<LargeNum> ownList;

  std::vector<ff::mpc::ObservationList<LargeNum>>
      outgoingListShares; // one for each cross-vertical party.
  std::vector<ff::mpc::ObservationList<LargeNum>>
      sharedLists; // one for each cross-vertical party.

  std::vector<ff::mpc::ObservationList<LargeNum>> vectorZippedAdjacent;

  std::vector<LargeNum>
      startModulusPayloadVector; // matrixShare followed by vectorShare

  /* in Ax = b, this is A.
      It's a dxd matrix, so A[i][j] = matrixShare[d*i + j] */
  std::vector<LargeNum> matrixShare;

  /** in Ax = b, this is b, a vector of length d */
  std::vector<LargeNum> vectorShare;

  LargeNum ySquaredShare;
  LargeNum yShare;
  LargeNum oneShare;

  ff::mpc::Matrix<LargeNum> matMultiplyOutput;

  dealer::RandomTableLookupInfo F_info;
  dealer::RandomTableLookupInfo t_info;

  size_t numPartiesAwaiting = 0;

  std::map<safrn::Identity, size_t> indexOfCrossParties;

  bool randomnessDone = false;

  std::vector<MultiplyInfo<BeaverInfo<LargeNum>>> factoryMultiplyInfo;

  std::vector<ff::mpc::Matrix<LargeNum>> m;
  std::vector<ff::mpc::Matrix<LargeNum>> r;

  LargeNum detShare = 0;

  LargeNum det = 0;
  std::vector<LargeNum> regressionMultiplyOutput;
  std::vector<LargeNum> negativeOrPositiveOneShares;
  std::vector<LargeNum> negativeCorrectionMultiplyOutput;

  std::vector<LargeNum> beta_i;
  std::vector<LargeNum> beta_ibeta_j;
  std::vector<LargeNum> x_ix_jn;
  std::vector<LargeNum> beta_ix_iy;
  LargeNum ySquaredn = 0;
  LargeNum ySumThenSquared = 0;

  LargeNum numer_MSE;
  LargeNum denom_MSE;
  LargeNum numer_RSquared;
  LargeNum denom_RSquared;
  LargeNum numer_F_statistic;
  LargeNum denom_F_statistic;

  bool abortFlag = false;

  std::vector<LargeNum> beta_ibeta_jx_ix_jn;
  std::vector<LargeNum> beta_ibeta_jx_ix_j;
  std::vector<LargeNum> beta_ix_iyn;

  std::vector<LargeNum> X_T_X_inv_diag_reweighted;
  std::vector<LargeNum> X_T_X_inv_diag_s_e;

  std::vector<LargeNum> meanSquareErrorCoeffs;
  LargeNum denom_MSE_reweighted;
  LargeNum F_statistic;
  std::vector<LargeNum> t_statisticsSquared;
  std::vector<LargeNum> t_statisticNumerators;

  /** F and t test look-ups */
  std::vector<size_t> F_row_ids;
  std::vector<size_t> t_row_ids;

  std::vector<std::vector<Boolean_t>>
      F_table_data; // indexed by (row*i + j) and then w/i a cell
  std::vector<std::vector<Boolean_t>>
      t_table_data; // indexed by (row*i + j) and then w/i a cell

  LargeNum F_row_id_share;
  LargeNum t_row_id_share;

  LargeNum num_F_cols;
  size_t F_cols_bits_of_precision;
  LargeNum F_cols_step_size;

  LargeNum num_t_cols;
  size_t t_cols_bits_of_precision;
  LargeNum t_cols_step_size;

  LargeNum F_statistic_col_index;
  std::vector<LargeNum> t_statistic_col_indices;

  std::vector<Boolean_t> F_p_value; // indexed w/i a cell
  std::vector<std::vector<Boolean_t>>
      t_p_values; // indexed by IV and then w/i a cell
};

} // namespace dataowner
} // namespace safrn

#define LOG_UNCLUDE
#include <ff/logging.h>

#endif //SAFRN_DATAOWNER_REGRESSION_H
