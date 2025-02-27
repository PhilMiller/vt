/*
//@HEADER
// *****************************************************************************
//
//                                 stats_msg.h
//                           DARMA Toolkit v. 1.0.0
//                       DARMA/vt => Virtual Transport
//
// Copyright 2019 National Technology & Engineering Solutions of Sandia, LLC
// (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact darma@sandia.gov
//
// *****************************************************************************
//@HEADER
*/

#if !defined INCLUDED_VRT_COLLECTION_BALANCE_STATS_MSG_H
#define INCLUDED_VRT_COLLECTION_BALANCE_STATS_MSG_H

#include "vt/config.h"
#include "vt/vrt/collection/balance/lb_common.h"
#include "vt/vrt/collection/balance/lb_invoke/start_lb_msg.h"
#include "vt/vrt/collection/messages/user.h"
#include "vt/collective/reduce/reduce.h"
#include "vt/messaging/message.h"
#include "vt/timing/timing.h"

#include <algorithm>

namespace vt { namespace vrt { namespace collection { namespace balance {

struct LoadData {
  LoadData() = default;
  LoadData(TimeType const y)
    : max_(y), sum_(y), min_(y), avg_(y), M2_(0.0f), M3_(0.0f), M4_(0.0f),
      N_(1), P_(y not_eq 0.0f)
  {
    debug_print(lb, node, "LoadData: in={}, N_={}\n", y, N_);
  }

  friend LoadData operator+(LoadData a1, LoadData const& a2) {
    debug_print(lb, node, "operator+: a1.N_={}, a2.N_={}\n", a1.N_, a2.N_);
    debug_print(lb, node, "operator+: a1.avg_={}, a2.avg_={}\n", a1.avg_, a2.avg_);

    int32_t N            = a1.N_ + a2.N_;
    double delta         = a2.avg_ - a1.avg_;
    double delta_sur_N   = delta / static_cast<double>(N);
    double delta2_sur_N2 = delta_sur_N * delta_sur_N;
    int32_t n2           = a1.N_ * a1.N_;
    int32_t n_c2         = a2.N_ * a2.N_;
    int32_t prod_n       = a1.N_ * a2.N_;
    int32_t n_c          = a2.N_;
    int32_t n            = a1.N_;
    double M2            = a1.M2_;
    double M2_c          = a2.M2_;
    double M3            = a1.M3_;
    double M3_c          = a2.M3_;

    a1.M4_ += a2.M4_
        + prod_n * ( n2 - prod_n + n_c2 ) * delta * delta_sur_N * delta2_sur_N2
        + 6. * ( n2 * M2_c + n_c2 * M2 ) * delta2_sur_N2
        + 4. * ( n * M3_c - n_c * M3 ) * delta_sur_N;

    a1.M3_ += a2.M3_
        + prod_n * ( n - n_c ) * delta * delta2_sur_N2
        + 3. * ( n * M2_c - n_c * M2 ) * delta_sur_N;

    a1.M2_  += a2.M2_ + prod_n * delta * delta_sur_N;
    a1.avg_ += n_c * delta_sur_N;
    a1.N_    = N;
    a1.min_  = std::min(a1.min_, a2.min_);
    a1.max_  = std::max(a1.max_, a2.max_);
    a1.sum_ += a2.sum_;
    a1.P_   += a2.P_;

    return a1;
  }

  TimeType max() const { return max_; }
  TimeType sum() const { return sum_; }
  TimeType min() const { return min_; }
  TimeType avg() const { return avg_; }
  TimeType var() const { return M2_ * (1.0f / N_); }
  TimeType skew() const {
    double nm1 = N_ - 1;
    double inv_n = 1. / N_;
    double var_inv = nm1 / M2_;
    double nvar_inv = var_inv * inv_n;
    return nvar_inv * std::sqrt( var_inv ) * M3_;
  }
  TimeType krte() const {
    double nm1 = N_ - 1;
    double inv_n = 1. / N_;
    double var_inv = nm1 / M2_;
    double nvar_inv = var_inv * inv_n;
    return nvar_inv * var_inv * M4_ - 3.;
  }
  TimeType I() const { return (max() / avg()) - 1.0f; }
  TimeType stdv() const { return std::sqrt(var()); }
  int32_t  npr() const { return P_; }

  TimeType max_ = 0.0;
  TimeType sum_ = 0.0;
  TimeType min_ = 0.0;
  TimeType avg_ = 0.0;
  TimeType M2_  = 0.0;
  TimeType M3_  = 0.0;
  TimeType M4_  = 0.0;
  int32_t  N_ = 0;
  int32_t  P_ = 0;
};

template <typename ColT>
struct LoadStatsMsg : CollectionMessage<ColT>, LoadData {
  LoadStatsMsg() = default;
  LoadStatsMsg(LoadData const& in_load_data, PhaseType const& phase)
    : LoadData(in_load_data), cur_phase_(phase)
  {}

  PhaseType getPhase() const { return cur_phase_; }

private:
  PhaseType cur_phase_ = fst_lb_phase;
};

struct ProcStatsMsg : collective::ReduceTMsg<LoadData> {
  ProcStatsMsg() = default;
  ProcStatsMsg(lb::Statistic in_stat, TimeType const in_total_load)
    : ReduceTMsg<LoadData>(LoadData(in_total_load)),
      stat_(in_stat)
  { }
  ProcStatsMsg(lb::Statistic in_stat, LoadData&& ld)
    : ReduceTMsg<LoadData>(std::move(ld)),
      stat_(in_stat)
  { }

  lb::Statistic stat_ = lb::Statistic::P_l;
};

template <typename ColT>
struct StatsMsg : collective::ReduceTMsg<LoadData> {
  using ProxyType = typename ColT::CollectionProxyType;

  StatsMsg() = default;
  StatsMsg(
    PhaseType const& in_cur_phase, TimeType const& in_total_load,
    ProxyType const& in_proxy
  ) : ReduceTMsg<LoadData>({in_total_load}),
      proxy_(in_proxy), cur_phase_(in_cur_phase)
  { }

  ProxyType getProxy() const { return proxy_; }
  PhaseType getPhase() const { return cur_phase_; }
private:
  ProxyType proxy_ = {};
  PhaseType cur_phase_ = fst_lb_phase;
};

}}}} /* end namespace vt::vrt::collection::balance */

#endif /*INCLUDED_VRT_COLLECTION_BALANCE_STATS_MSG_H*/
