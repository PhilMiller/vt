/*
//@HEADER
// *****************************************************************************
//
//                                   event.h
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

#if !defined INCLUDED_EVENT_EVENT_H
#define INCLUDED_EVENT_EVENT_H

#include "vt/config.h"
#include "vt/context/context.h"
#include "vt/event/event_record.h"
#include "vt/event/event_id.h"
#include "vt/event/event_holder.h"
#include "vt/event/event_msgs.h"

#include <memory>
#include <vector>
#include <list>
#include <functional>
#include <unordered_map>

#include <mpi.h>

namespace vt { namespace event {

enum class EventState : int8_t {
  EventReady = 1,
  EventWaiting = 2,
  EventRemote = 3
};

struct AsyncEvent {
  using EventRecordTypeType = eEventRecord;
  using EventManagerType = EventIDManager;
  using EventStateType = EventState;
  using EventRecordType = EventRecord;
  using EventRecordPtrType = std::unique_ptr<EventRecordType>;
  using EventHolderType = EventHolder;
  using EventHolderPtrType = EventHolder*;
  using TypedEventContainerType = std::list<EventHolderType>;
  using EventContIter = typename TypedEventContainerType::iterator;
  using EventContainerType = std::unordered_map<EventType, EventContIter>;

  AsyncEvent() = default;

  virtual ~AsyncEvent();

  void cleanup();
  EventType createEvent(EventRecordTypeType const& type, NodeType const& node);
  EventRecordType& getEvent(EventType const& event);
  NodeType getOwningNode(EventType const& event);
  EventType createMPIEvent(NodeType const& node);
  EventType createNormalEvent(NodeType const& node);
  EventType createParentEvent(NodeType const& node);
  EventHolderType& getEventHolder(EventType const& event);
  bool holderExists(EventType const& event);
  bool needsPolling(EventRecordTypeType const& type);
  void removeEventID(EventType const& event);
  EventStateType testEventComplete(EventType const& event);
  EventType attachAction(EventType const& event, ActionType callable);
  void testEventsTrigger(int const& num_events = num_check_actions);
  bool scheduler();
  bool isLocalTerm();

  static void eventFinished(EventFinishedMsg* msg);
  static void checkEventFinished(EventCheckFinishedMsg* msg);

private:
  // next event id
  EventType cur_event_ = 0;

  // std::list of events
  TypedEventContainerType event_container_;

  // list of events that need polling for progress
  TypedEventContainerType polling_event_container_;

  // container to lookup events by EventType
  EventContainerType lookup_container_;
};

}} //end namespace vt::event

namespace vt {

using EventRecordType = event::EventRecord;

extern event::AsyncEvent* theEvent();

} //end namespace vt

#endif /*INCLUDED_EVENT_EVENT_H*/
