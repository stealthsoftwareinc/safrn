/**
 * Copyright (C) 2020 Stealth Software Technologies Commercial, Inc.
 */

#ifndef SAFRN_DATAOWNER_FORTISSIMO_H_
#define SAFRN_DATAOWNER_FORTISSIMO_H_

#include <framework/Framework.h>

#include <mpc/Multiply.h>
#include <mpc/templates.h>
//#include <mpc/Division.h>
#include <Identity.h>
#include <PeerSet.h>
#include <mpc/ModConvUp.h>
#include <mpc/SISOSort.h>

namespace safrn {
namespace dataowner {

using SmallNum = ff::mpc::SmallNum;
using LargeNum = ff::mpc::LargeNum;

template<typename Number_T, typename Info_T>
using Multiply = ff::mpc::Multiply<SAFRN_TYPES, Number_T, Info_T>;
template<typename Info_T>
using MultiplyInfo = ff::mpc::MultiplyInfo<safrn::Identity, Info_T>;
template<typename Number_T>
using BeaverInfo = ff::mpc::BeaverInfo<Number_T>;
using SISOSort = ff::mpc::SISOSort<SAFRN_TYPES, LargeNum, SmallNum>;

template<
    typename SmallNumber_T,
    typename MediumNumber_T,
    typename LargeNumber_T>
using ModConvUp = ff::mpc::ModConvUp<
    SAFRN_TYPES,
    SmallNumber_T,
    MediumNumber_T,
    LargeNumber_T>;
using Batch = ff::mpc::Batch<SAFRN_TYPES>;

} // namespace dataowner
} // namespace safrn

#endif // SAFRN_DATAOWNER_FORTISSIMO_H_
