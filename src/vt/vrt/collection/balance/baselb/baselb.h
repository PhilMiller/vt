/*
//@HEADER
// *****************************************************************************
//
//                                   baselb.h
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

#if !defined INCLUDED_VT_VRT_COLLECTION_BALANCE_BASELB_BASELB_H
#define INCLUDED_VT_VRT_COLLECTION_BALANCE_BASELB_BASELB_H

#include "vt/config.h"
#include "vt/vrt/collection/balance/lb_common.h"
#include "vt/vrt/collection/balance/lb_invoke/start_lb_msg.h"
#include "vt/vrt/collection/balance/baselb/baselb_msgs.h"
#include "vt/vrt/collection/balance/proc_stats.h"
#include "vt/vrt/collection/balance/lb_comm.h"
#include "vt/objgroup/headers.h"

#include <set>
#include <map>
#include <unordered_map>
#include <tuple>

namespace vt { namespace vrt { namespace collection { namespace lb {

static constexpr int32_t const default_bin_size = 10;

struct BaseLB {
  using ObjIDType        = balance::ElementIDType;
  using ObjBinType       = int32_t;
  using ObjBinListType   = std::list<ObjIDType>;
  using ObjSampleType    = std::map<ObjBinType, ObjBinListType>;
  using ElementLoadType  = std::unordered_map<ObjIDType,TimeType>;
  using ElementCommType  = balance::CommMapType;
  using StatsMsgType     = balance::ProcStatsMsg;
  using TransferDestType = std::tuple<ObjIDType,NodeType>;
  using TransferVecType  = std::vector<TransferDestType>;
  using TransferType     = std::map<NodeType, TransferVecType>;
  using LoadType         = double;
  using QuantityType     = std::map<StatisticQuantity, double>;
  using StatisticMapType = std::unordered_map<Statistic, QuantityType>;

  explicit BaseLB(
    bool in_comm_aware = false,
    bool in_comm_collectives = false,
    int32_t in_bin_size = default_bin_size
  ) : bin_size_(in_bin_size),
      comm_aware_(in_comm_aware),
      comm_collectives_(in_comm_collectives)
  { }

  void startLBHandler(balance::StartLBMsg* msg, objgroup::proxy::Proxy<BaseLB> proxy);
  void computeStatistics();
  void importProcessorData(ElementLoadType const& ld, ElementCommType const& cm);
  void statsHandler(StatsMsgType* msg);
  void finishedStats();

  ObjBinType histogramSample(LoadType const& load) const;
  LoadType loadMilli(LoadType const& load) const;
  int32_t getBinSize() const { return bin_size_; }
  NodeType objGetNode(ObjIDType const id) const;

  EpochType getMigrationEpoch() const;
  EpochType startMigrationCollective();
  void finishMigrationCollective();
  void migrationDone();
  void migrateObjectTo(ObjIDType const obj_id, NodeType const node);
  void transferSend(NodeType from, TransferVecType const& transfer, EpochType ep);
  void transferMigrations(TransferMsg<TransferVecType>* msg);
  void finalize(CountMsg* msg);

  virtual void runLB() = 0;

private:
  balance::LoadData reduceVec(std::vector<balance::LoadData>&& vec) const;
  bool isCollectiveComm(balance::CommCategory cat) const;
  void computeStatisticsOver(Statistic stats);
  void readLB(PhaseType phase);

protected:
  virtual double getDefaultMinThreshold()  const = 0;
  virtual double getDefaultMaxThreshold()  const = 0;
  virtual bool   getDefaultAutoThreshold() const = 0;

protected:
  double max_threshold                  = 0.0f;
  double min_threshold                  = 0.0f;
  bool auto_threshold                   = true;
  double start_time_                    = 0.0f;
  int32_t bin_size_                     = 10;
  ObjSampleType obj_sample              = {};
  LoadType this_load                    = 0.0f;
  ElementLoadType const* load_data      = nullptr;
  ElementCommType const* comm_data      = nullptr;
  StatisticMapType stats                = {};
  EpochType migration_epoch_            = no_epoch;
  TransferType off_node_migrate_        = {};
  objgroup::proxy::Proxy<BaseLB> proxy_ = {};
  int32_t local_migration_count_        = 0;
  PhaseType phase_                      = 0;
  int32_t num_reduce_stats_             = 0;
  bool comm_aware_                      = false;
  bool comm_collectives_                = false;
};

}}}} /* end namespace vt::vrt::collection::balance::lb */

#endif /*INCLUDED_VT_VRT_COLLECTION_BALANCE_BASELB_BASELB_H*/
