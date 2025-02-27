/*
//@HEADER
// *****************************************************************************
//
//                                 rdma_info.h
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

#if !defined INCLUDED_RDMA_RDMA_INFO_H
#define INCLUDED_RDMA_RDMA_INFO_H

#include "vt/config.h"
#include "vt/rdma/rdma_common.h"

#include <unordered_map>
#include <vector>

namespace vt { namespace rdma {

struct Info {
  using RDMA_TypeType = Type;

  RDMA_TypeType rdma_type;
  ByteType num_bytes = no_byte;
  TagType tag = no_tag;
  RDMA_PtrType data_ptr = no_rdma_ptr;
  RDMA_ContinuationType cont = no_action;
  ActionType cont_action = no_action;
  ByteType offset = no_byte;
  bool is_local = false;

  Info(
    RDMA_TypeType const& in_rdma_type, ByteType const& in_num_bytes = no_byte,
    ByteType const& in_offset = no_byte, TagType const& in_tag = no_tag,
    RDMA_ContinuationType in_cont = no_action, ActionType in_cont_action = no_action,
    RDMA_PtrType const& in_data_ptr = no_rdma_ptr, bool const in_is_local = false
  ) : rdma_type(in_rdma_type), num_bytes(in_num_bytes), tag(in_tag),
      data_ptr(in_data_ptr), cont(in_cont), cont_action(in_cont_action),
      offset(in_offset), is_local(in_is_local)
  { }
};

}} //end namespace vt::rdma

#endif /*INCLUDED_RDMA_RDMA_INFO_H*/
