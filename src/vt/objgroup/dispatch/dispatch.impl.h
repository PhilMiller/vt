/*
//@HEADER
// ************************************************************************
//
//                          dispatch.impl.h
//                     vt (Virtual Transport)
//                  Copyright (C) 2018 NTESS, LLC
//
// Under the terms of Contract DE-NA-0003525 with NTESS, LLC,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact darma@sandia.gov
//
// ************************************************************************
//@HEADER
*/

#if !defined INCLUDED_VT_OBJGROUP_DISPATCH_DISPATCH_IMPL_H
#define INCLUDED_VT_OBJGROUP_DISPATCH_DISPATCH_IMPL_H

#include "vt/config.h"
#include "vt/objgroup/common.h"
#include "vt/registry/auto/auto_registry.h"

namespace vt { namespace objgroup { namespace dispatch {

template <typename ObjT>
void Dispatch<ObjT>::run(HandlerType han, BaseMessage* msg) {
  using ActiveFnType = void(ObjT::*)(vt::BaseMessage*);
  vtAssert(obj_ != nullptr, "Must have a valid object");
  // Consume if there is an epoch for this message
  auto tmsg = static_cast<vt::Message*>(msg);
  auto cur_epoch = envelopeGetEpoch(tmsg->env);
  if (cur_epoch != no_epoch) {
    theMsg()->pushEpoch(cur_epoch);
  }
  auto base_func = auto_registry::getAutoHandlerObjGroup(han);
  auto type_func = reinterpret_cast<ActiveFnType>(base_func);
  (obj_->*type_func)(msg);
  if (cur_epoch != no_epoch) {
    theMsg()->popEpoch();
  }
}

}}} /* end namespace vt::objgroup::dispatch */

#endif /*INCLUDED_VT_OBJGROUP_DISPATCH_DISPATCH_IMPL_H*/
