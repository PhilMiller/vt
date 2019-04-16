/*
//@HEADER
// ************************************************************************
//
//                          location_common.h
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

#if !defined INCLUDED_TOPOS_LOCATION_LOCATION_COMMON_H
#define INCLUDED_TOPOS_LOCATION_LOCATION_COMMON_H

#include "vt/config.h"
#include "vt/messaging/message.h"

#include <functional>
#include <cstdint>

namespace vt { namespace location {

using NodeActionType = std::function<void(NodeType)>;
using LocMsgActionType = std::function<void(BaseMessage * )>;
using LocEventID = int64_t;

static constexpr LocEventID const no_location_event_id = -1;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static LocEventID fst_location_event_id = 0;
#pragma GCC diagnostic pop

using LocationSizeType = size_t;
static constexpr LocationSizeType const default_max_cache_size = 4096;

static constexpr ByteType const small_msg_max_size = 256;

using LocInstType = int64_t;

static constexpr LocInstType const no_loc_inst = -1;

}}  // end namespace vt::location

#endif /*INCLUDED_TOPOS_LOCATION_LOCATION_COMMON_H*/
