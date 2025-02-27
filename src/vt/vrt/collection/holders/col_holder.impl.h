/*
//@HEADER
// *****************************************************************************
//
//                              col_holder.impl.h
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

#if !defined INCLUDED_VRT_COLLECTION_HOLDERS_COL_HOLDER_IMPL_H
#define INCLUDED_VRT_COLLECTION_HOLDERS_COL_HOLDER_IMPL_H

#include "vt/config.h"
#include "vt/vrt/collection/holders/col_holder.h"

namespace vt { namespace vrt { namespace collection {

template <typename ColT, typename IndexT>
CollectionHolder<ColT, IndexT>::CollectionHolder(
  HandlerType const& in_map_fn, IndexT const& idx, bool const in_is_static
) : is_static_(in_is_static), map_fn(in_map_fn), max_idx(idx)
{ }

template <typename ColT, typename IndexT>
void CollectionHolder<ColT, IndexT>::destroy() {
  holder_.destroyAll();
}

template <typename ColT, typename IndexT>
void CollectionHolder<ColT, IndexT>::runLB(PhaseType cur_phase) {
  holder_.foreach([=](IndexT const& idx, CollectionBase<ColT,IndexT>* base){
    auto proxy = base->getCollectionProxy();
    auto phase = cur_phase == no_lb_phase ? base->getStats().getPhase() : cur_phase;
    auto phase_msg = makeSharedMessage<balance::PhaseMsg<ColT>>(phase,proxy,true,true);
    balance::ElementStats::syncNextPhase(phase_msg, static_cast<ColT*>(base));
  });
}

}}} /* end namespace vt::vrt::collection */

#endif /*INCLUDED_VRT_COLLECTION_HOLDERS_COL_HOLDER_IMPL_H*/
