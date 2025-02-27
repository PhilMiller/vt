/*
//@HEADER
// *****************************************************************************
//
//                             envelope_set.impl.h
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

#if !defined INCLUDED_MESSAGING_ENVELOPE_ENVELOPE_SET_IMPL_H
#define INCLUDED_MESSAGING_ENVELOPE_ENVELOPE_SET_IMPL_H

#include "vt/config.h"
#include "vt/messaging/envelope/envelope_set.h"

namespace vt {

// Set the type of Envelope
template <typename Env>
inline void setNormalType(Env& env) {
  reinterpret_cast<Envelope*>(&env)->type = 0;
}

template <typename Env>
inline void setPipeType(Env& env) {
  reinterpret_cast<Envelope*>(&env)->type |= 1 << eEnvType::EnvPipe;
}

template <typename Env>
inline void setPutType(Env& env) {
  reinterpret_cast<Envelope*>(&env)->type |= 1 << eEnvType::EnvPut;
}

template <typename Env>
inline void setTermType(Env& env) {
  reinterpret_cast<Envelope*>(&env)->type |= 1 << eEnvType::EnvTerm;
}

template <typename Env>
inline void setBroadcastType(Env& env) {
  reinterpret_cast<Envelope*>(&env)->type |= 1 << eEnvType::EnvBroadcast;
}

template <typename Env>
inline void setEpochType(Env& env) {
  reinterpret_cast<Envelope*>(&env)->type |= 1 << eEnvType::EnvEpochType;
}

template <typename Env>
inline void setTagType(Env& env) {
  reinterpret_cast<Envelope*>(&env)->type |= 1 << eEnvType::EnvTagType;
}

template <typename Env>
inline void envelopeSetHandler(Env& env, HandlerType const& handler) {
  reinterpret_cast<Envelope*>(&env)->han = handler;
}

template <typename Env>
inline void envelopeSetDest(Env& env, NodeType const& dest) {
  reinterpret_cast<Envelope*>(&env)->dest = dest;
}

template <typename Env>
inline void envelopeSetRef(Env& env, RefType const& ref) {
  reinterpret_cast<Envelope*>(&env)->ref = ref;
}

template <typename Env>
inline void envelopeSetGroup(Env& env, GroupType const& group) {
  reinterpret_cast<Envelope*>(&env)->group = group;
}

#if backend_check_enabled(trace_enabled)
template <typename Env>
inline void envelopeSetTraceEvent(Env& env, trace::TraceEventIDType const& evt) {
  reinterpret_cast<Envelope*>(&env)->trace_event = evt;
}
#endif

} /* end namespace vt */

#endif /*INCLUDED_MESSAGING_ENVELOPE_ENVELOPE_SET_IMPL_H*/
