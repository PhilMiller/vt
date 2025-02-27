/*
//@HEADER
// *****************************************************************************
//
//                           migrate_handlers.impl.h
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

#if !defined INCLUDED_VRT_COLLECTION_MIGRATE_MIGRATE_HANDLERS_IMPL_H
#define INCLUDED_VRT_COLLECTION_MIGRATE_MIGRATE_HANDLERS_IMPL_H

#include "vt/config.h"
#include "vt/vrt/vrt_common.h"
#include "vt/vrt/collection/migrate/migrate_msg.h"
#include "vt/vrt/collection/migrate/migrate_handlers.h"
#include "vt/vrt/collection/migrate/manager_migrate_attorney.h"
#include "vt/serialization/serialization.h"

#include <memory>
#include <functional>
#include <cassert>

namespace vt { namespace vrt { namespace collection {

template <typename ColT, typename IndexT>
/*static*/ void MigrateHandlers::migrateInHandler(
  MigrateMsg<ColT, IndexT>* msg
) {
  auto const& from_node = msg->getFromNode();
  auto const& full_proxy = msg->getElementProxy();
  auto const& col_proxy = full_proxy.getCollectionProxy();
  auto const& elm_proxy = full_proxy.getElementProxy();
  auto const& idx = elm_proxy.getIndex();

  debug_print(
    vrt_coll, node,
    "migrateInHandler: from_node={}, idx={}\n",
    from_node, idx
  );

  auto const& map_han = msg->getMapHandler();
  auto const& range = msg->getRange();

  // auto buf = reinterpret_cast<SerialByteType*>(msg->getPut());
  // auto const& buf_size = msg->getPutSize();

  // auto vc_elm_ptr = std::make_unique<ColT>();
  // auto vc_raw_ptr = ::serialization::interface::deserialize<ColT>(
  //   buf, buf_size, vc_elm_ptr.get()
  // );
  // ColT* col_t = new ColT();
  // auto vc_raw_ptr = ::serialization::interface::deserialize<ColT>(
  //   buf, buf_size, col_t
  // );
  auto vc_elm_ptr = std::make_unique<ColT>(std::move(*msg->elm_));

  auto const& migrate_status =
    CollectionElmAttorney<ColT,IndexT>::migrateIn(
      col_proxy, idx, from_node, std::move(vc_elm_ptr), range, map_han
    );

  vtAssert(
    migrate_status == MigrateStatus::MigrateInLocal,
    "Should be valid local migration into this memory domain"
  );
}

}}} /* end namespace vt::vrt::collection */


#endif /*INCLUDED_VRT_COLLECTION_MIGRATE_MIGRATE_HANDLERS_IMPL_H*/
