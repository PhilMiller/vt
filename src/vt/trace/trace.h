/*
//@HEADER
// *****************************************************************************
//
//                                   trace.h
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

#if !defined INCLUDED_TRACE_TRACE_H
#define INCLUDED_TRACE_TRACE_H

#include "vt/config.h"
#include "vt/context/context.h"
#include "vt/configs/arguments/args.h"
#include "vt/trace/trace_common.h"
#include "vt/trace/trace_registry.h"
#include "vt/trace/trace_constants.h"
#include "vt/trace/trace_event.h"
#include "vt/trace/trace_containers.h"
#include "vt/trace/trace_log.h"
#include "vt/trace/trace_user_event.h"

#include <cstdint>
#include <cassert>
#include <unordered_map>
#include <stack>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <fstream>

#include <mpi.h>
#include <zlib.h>

namespace vt { namespace trace {

struct Trace {
  using LogType             = Log;
  using TraceConstantsType  = eTraceConstants;
  using TraceContainersType = TraceContainers<void>;
  using TimeIntegerType     = int64_t;
  using LogPtrType          = LogType*;
  using TraceContainerType  = std::vector<LogPtrType>;
  using TraceStackType      = std::stack<LogPtrType>;
  using ArgType             = vt::arguments::ArgConfig;

  Trace();
  Trace(std::string const& in_prog_name, std::string const& in_trace_name);

  virtual ~Trace();

  friend struct Log;

  std::string getTraceName() const { return full_trace_name; }
  std::string getSTSName()   const { return full_sts_name;   }
  std::string getDirectory() const { return full_dir_name;   }

  void initialize();
  void setupNames(
    std::string const& in_prog_name, std::string const& in_trace_name,
    std::string const& in_dir_name = ""
  );

  void beginProcessing(
    TraceEntryIDType const ep, TraceMsgLenType const len,
    TraceEventIDType const event, NodeType const from_node,
    double const time = getCurrentTime(),
    uint64_t const idx1 = 0, uint64_t const idx2 = 0, uint64_t const idx3 = 0,
    uint64_t const idx4 = 0
  );
  void endProcessing(
    TraceEntryIDType const ep, TraceMsgLenType const len,
    TraceEventIDType const event, NodeType const from_node,
    double const time = getCurrentTime(),
    uint64_t const idx1 = 0, uint64_t const idx2 = 0, uint64_t const idx3 = 0,
    uint64_t const idx4 = 0
  );

  void beginIdle(double const time = getCurrentTime());
  void endIdle(double const time = getCurrentTime());

  UserEventIDType registerUserEventColl(std::string const& name);
  UserEventIDType registerUserEventRoot(std::string const& name);
  UserEventIDType registerUserEventHash(std::string const& name);
  void registerUserEventManual(std::string const& name, UserSpecEventIDType id);

  void addUserEvent(UserEventIDType event);
  void addUserEventManual(UserSpecEventIDType event);
  void addUserEventBracketed(UserEventIDType event, double begin, double end);
  void addUserEventBracketedManual(
    UserSpecEventIDType event, double begin, double end
  );
  void addUserEventBracketedBegin(UserEventIDType event);
  void addUserEventBracketedEnd(UserEventIDType event);
  void addUserEventBracketedManualBegin(UserSpecEventIDType event);
  void addUserEventBracketedManualEnd(UserSpecEventIDType event);
  void addUserNote(std::string const& note);
  void addUserData(int32_t data);
  void addUserBracketedNote(
    double const begin, double const end, std::string const& note,
    TraceEventIDType const event = no_trace_event
  );

  TraceEventIDType messageCreation(
    TraceEntryIDType const ep, TraceMsgLenType const len,
    double const time = getCurrentTime()
  );
  TraceEventIDType messageCreationBcast(
    TraceEntryIDType const ep, TraceMsgLenType const len,
    double const time = getCurrentTime()
  );
  TraceEventIDType messageRecv(
    TraceEntryIDType const ep, TraceMsgLenType const len,
    NodeType const from_node, double const time = getCurrentTime()
  );
  TraceEventIDType logEvent(LogPtrType log);

  void enableTracing();
  void disableTracing();
  bool checkEnabled();

  void writeTracesFile();
  void writeLogFile(gzFile file, TraceContainerType const& traces);
  bool inIdleEvent() const;

  static double getCurrentTime();
  void outputControlFile(std::ofstream& file);
  static TimeIntegerType timeToInt(double const time);
  static void traceBeginIdleTrigger();
  static void outputHeader(
    NodeType const node, double const start, gzFile file
  );
  static void outputFooter(
    NodeType const node, double const start, gzFile file
  );

  friend void insertNewUserEvent(UserEventIDType event, std::string const& name);

private:
  void editLastEntry(std::function<void(LogPtrType)> fn);

private:
  TraceContainerType traces_;
  TraceStackType open_events_;
  TraceEventIDType cur_event_   = 1;
  std::string dir_name_         = "";
  std::string prog_name_        = "";
  std::string trace_name_       = "";
  bool enabled_                 = true;
  bool idle_begun_              = false;
  double start_time_            = 0.0;
  std::string full_trace_name   = "";
  std::string full_sts_name     = "";
  std::string full_dir_name     = "";
  UserEventRegistry user_event  = {};
};

}} //end namespace vt::trace

namespace vt {

#if backend_check_enabled(trace_enabled)
  extern trace::Trace* theTrace();
#endif

}

#endif /*INCLUDED_TRACE_TRACE_H*/
