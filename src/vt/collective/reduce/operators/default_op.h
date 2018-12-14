/*
//@HEADER
// ************************************************************************
//
//                          default_op.h
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

#if !defined INCLUDED_COLLECTIVE_REDUCE_OPERATORS_DEFAULT_OP_H
#define INCLUDED_COLLECTIVE_REDUCE_OPERATORS_DEFAULT_OP_H

#include "vt/config.h"
#include "vt/collective/reduce/reduce_msg.h"
#include "vt/collective/reduce/operators/default_msg.h"
#include "vt/collective/reduce/operators/functors/none_op.h"
#include "vt/collective/reduce/operators/functors/and_op.h"
#include "vt/collective/reduce/operators/functors/or_op.h"
#include "vt/collective/reduce/operators/functors/plus_op.h"
#include "vt/collective/reduce/operators/functors/max_op.h"
#include "vt/collective/reduce/operators/functors/min_op.h"
#include "vt/collective/reduce/operators/functors/bit_and_op.h"
#include "vt/collective/reduce/operators/functors/bit_or_op.h"
#include "vt/collective/reduce/operators/functors/bit_xor_op.h"

#include <algorithm>

namespace vt { namespace collective { namespace reduce { namespace operators {

template <typename T = void>
struct ReduceCallback {
  void operator()(T* t) const { /* do nothing */ }
};

template <typename T = void>
struct ReduceCombine {
  ReduceCombine() = default;
private:
  template <typename MsgT, typename Op, typename ActOp>
  static void combine(MsgT* m1, MsgT* m2) {
    Op()(m1->getVal(), m2->getConstVal());
  }
public:
  template <typename MsgT, typename Op, typename ActOp>
  static void msgHandler(MsgT* msg) {
    if (msg->isRoot()) {
      auto cb = msg->getCallback();
      if (cb.valid()) {
        cb.template send<MsgT>(msg);
      } else {
        ActOp()(msg);
      }
    } else {
      MsgT* fst_msg = msg;
      MsgT* cur_msg = msg->template getNext<MsgT>();
      while (cur_msg != nullptr) {
        ReduceCombine<>::combine<MsgT,Op,ActOp>(fst_msg, cur_msg);
        cur_msg = cur_msg->template getNext<MsgT>();
      }
    }
  }
};

}}}} /* end namespace vt::collective::reduce::operators */

#endif /*INCLUDED_COLLECTIVE_REDUCE_OPERATORS_DEFAULT_OP_H*/
