/*
//@HEADER
// *****************************************************************************
//
//                                termination.h
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

#if !defined INCLUDED_TERMINATION_TERMINATION_H
#define INCLUDED_TERMINATION_TERMINATION_H

#include "vt/config.h"
#include "vt/termination/term_common.h"
#include "vt/termination/term_msgs.h"
#include "vt/termination/term_state.h"
#include "vt/termination/term_action.h"
#include "vt/termination/term_interface.h"
#include "vt/termination/term_window.h"
#include "vt/termination/term_parent.h"
#include "vt/termination/dijkstra-scholten/ds_headers.h"
#include "vt/epoch/epoch.h"
#include "vt/activefn/activefn.h"
#include "vt/collective/tree/tree.h"
#include "vt/configs/arguments/args.h"

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <memory>

namespace vt { namespace term {

using DijkstraScholtenTerm = term::ds::StateDS;

struct TerminationDetector :
  TermAction, collective::tree::Tree, DijkstraScholtenTerm, TermInterface
{
  template <typename T>
  using EpochContainerType = std::unordered_map<EpochType, T>;
  using TermStateType      = TermState;
  using TermStateDSType    = term::ds::StateDS::TerminatorType;
  using WindowType         = std::unique_ptr<EpochWindow>;
  using ArgType            = vt::arguments::ArgConfig;
  using ParentBagType      = EpochRelation::ParentBagType;

  TerminationDetector();

  /****************************************************************************
   *
   * Termination interface: produce(..)/consume(..) for 4-counter wave-based
   * termination, send(..) for Dijkstra-Scholten parental responsibility TD
   *
   ***************************************************************************/
  void produce(
    EpochType epoch = any_epoch_sentinel, TermCounterType num_units = 1,
    NodeType node = uninitialized_destination
  );
  void consume(
    EpochType epoch = any_epoch_sentinel, TermCounterType num_units = 1,
    NodeType node = uninitialized_destination
  );
  /***************************************************************************/

  friend struct ds::StateDS;

  bool isRooted(EpochType epoch);
  bool isDS(EpochType epoch);
  TermStateDSType* getDSTerm(EpochType epoch, bool is_root = false);

  void resetGlobalTerm();
  void freeEpoch(EpochType const& epoch);

public:
  /*
   * Interface for scoped epochs using C++ lexical scopes to encapsulate epoch
   * regimes
   */

  struct Scoped {
    static EpochType rooted(bool small, ActionType closure);
    static EpochType rooted(bool small, ActionType closure, ActionType action);
    static EpochType collective(ActionType closure);
    static EpochType collective(ActionType closure, ActionType action);

    template <typename... Actions>
    static void rootedSeq(bool small, Actions... closures);
  } scope;

public:
  /*
   * Interface for making epochs for termination detection
   */
  EpochType makeEpochRooted(
    bool useDS = false, bool child = true, EpochType parent = no_epoch
  );
  EpochType makeEpochCollective(bool child = true, EpochType parent = no_epoch);
  EpochType makeEpoch(
    bool is_coll, bool useDS = false, bool child = true, EpochType parent = no_epoch
  );
  void activateEpoch(EpochType const& epoch);
  void finishedEpoch(EpochType const& epoch);
  void finishNoActivateEpoch(EpochType const& epoch);

public:
  /*
   * Directly call into a specific type of rooted epoch, can not be overridden
   */
  EpochType makeEpochRootedWave(bool child, EpochType parent);
  EpochType makeEpochRootedDS(bool child, EpochType parent);

private:
  TermStateType& findOrCreateState(EpochType const& epoch, bool is_ready);
  void cleanupEpoch(EpochType const& epoch);
  void produceConsumeState(
    TermStateType& state, TermCounterType const num_units, bool produce,
    NodeType node
  );
  void produceConsume(
    EpochType epoch = any_epoch_sentinel, TermCounterType num_units = 1,
    bool produce = true, NodeType node = uninitialized_destination
  );
  void propagateEpochExternalState(
    TermStateType& state, TermCounterType const& prod, TermCounterType const& cons
  );
  void propagateEpochExternal(
    EpochType const& epoch, TermCounterType const& prod,
    TermCounterType const& cons
  );

  EpochType getArchetype(EpochType const& epoch) const;
  EpochWindow* getWindow(EpochType const& epoch);

private:
  void updateResolvedEpochs(EpochType const& epoch);
  void inquireTerminated(EpochType const& epoch, NodeType const& from_node);
  void replyTerminated(EpochType const& epoch, bool const& is_terminated);

public:
  void setLocalTerminated(bool const terminated, bool const no_local = true);
  void maybePropagate();
  TermCounterType getNumUnits() const;

public:
  // TermTerminated interface
  TermStatusEnum testEpochTerminated(EpochType epoch) override;

private:
  bool propagateEpoch(TermStateType& state);
  void epochTerminated(EpochType const& epoch);
  void epochContinue(EpochType const& epoch, TermWaveType const& wave);
  void setupNewEpoch(EpochType const& epoch);
  void readyNewEpoch(EpochType const& epoch);
  void linkChildEpoch(EpochType epoch, EpochType parent = no_epoch);
  void makeRootedHan(EpochType const& epoch, bool is_root);

  static void makeRootedHandler(TermMsg* msg);
  static void inquireEpochTerminated(TermTerminatedMsg* msg);
  static void replyEpochTerminated(TermTerminatedReplyMsg* msg);
  static void propagateEpochHandler(TermCounterMsg* msg);
  static void epochTerminatedHandler(TermMsg* msg);
  static void epochContinueHandler(TermMsg* msg);

private:
  // global termination state
  TermStateType any_epoch_state_;
  // epoch termination state
  EpochContainerType<TermStateType> epoch_state_        = {};
  // epoch window container for specific archetyped epochs
  std::unordered_map<EpochType,WindowType> epoch_arch_  = {};
  // epoch window for basic collective epochs
  std::unique_ptr<EpochWindow> epoch_coll_              = nullptr;
  // ready epoch list (misnomer: finishedEpoch was invoked)
  std::unordered_set<EpochType> epoch_ready_            = {};
  // list of remote epochs pending status report of finished
  std::unordered_set<EpochType> epoch_wait_status_      = {};
};

}} // end namespace vt::term

#include "vt/termination/termination.impl.h"
#include "vt/termination/term_scope.impl.h"

#endif /*INCLUDED_TERMINATION_TERMINATION_H*/
