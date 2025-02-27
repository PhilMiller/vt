/*
//@HEADER
// *****************************************************************************
//
//                                  lb_comm.h
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

#if !defined INCLUDED_VT_VRT_COLLECTION_BALANCE_LB_COMM_H
#define INCLUDED_VT_VRT_COLLECTION_BALANCE_LB_COMM_H

#include "vt/config.h"
#include "vt/vrt/collection/balance/lb_common.h"

#include <unordered_map>

namespace vt { namespace vrt { namespace collection { namespace balance {

enum struct CommCategory : int8_t {
  SendRecv = 1,
  CollectionToNode = 2,
  NodeToCollection = 3,
  Broadcast = 4,
  CollectionToNodeBcast = 5,
  NodeToCollectionBcast = 6,
};

inline NodeType objGetNode(ElementIDType const id) {
  return id & 0x0000000FFFFFFFF;
}

struct LBCommKey {

  struct CollectionTag { };
  struct CollectionToNodeTag { };
  struct NodeToCollectionTag { };

  LBCommKey() = default;
  LBCommKey(LBCommKey const&) = default;
  LBCommKey(LBCommKey&&) = default;
  LBCommKey& operator=(LBCommKey const&) = default;

  LBCommKey(
    CollectionTag,
    ElementIDType from, ElementIDType from_temp,
    ElementIDType to,   ElementIDType to_temp,
    bool bcast
  ) : from_(from), from_temp_(from_temp), to_(to), to_temp_(to_temp),
      cat_(bcast ? CommCategory::Broadcast : CommCategory::SendRecv)
  { }
  LBCommKey(
    CollectionToNodeTag,
    ElementIDType from, ElementIDType from_temp, NodeType to,
    bool bcast
  ) : from_(from), from_temp_(from_temp), nto_(to),
      cat_(bcast ? CommCategory::CollectionToNodeBcast : CommCategory::CollectionToNode)
  { }
  LBCommKey(
    NodeToCollectionTag,
    NodeType from, ElementIDType to, ElementIDType to_temp,
    bool bcast
  ) : to_(to), to_temp_(to_temp), nfrom_(from),
      cat_(bcast ? CommCategory::NodeToCollectionBcast : CommCategory::NodeToCollection)
  { }

  ElementIDType from_      = no_element_id;
  ElementIDType from_temp_ = no_element_id;
  ElementIDType to_        = no_element_id;
  ElementIDType to_temp_   = no_element_id;
  NodeType nfrom_          = uninitialized_destination;
  NodeType nto_            = uninitialized_destination;
  CommCategory  cat_       = CommCategory::SendRecv;

  ElementIDType fromObj()      const { return from_; }
  ElementIDType toObj()        const { return to_; }
  ElementIDType fromObjTemp()  const { return from_temp_; }
  ElementIDType toObjTemp()    const { return to_temp_; }
  ElementIDType fromNode()     const { return nfrom_; }
  ElementIDType toNode()       const { return nto_; }

  bool selfEdge() const { return cat_ == CommCategory::SendRecv and from_ == to_; }
  bool offNode() const {
    if (cat_ == CommCategory::SendRecv) {
      return objGetNode(from_temp_) != objGetNode(to_temp_);
    } else if (cat_ == CommCategory::CollectionToNode) {
      return objGetNode(from_temp_) != nto_;
    } else if (cat_ == CommCategory::NodeToCollection) {
      return objGetNode(to_temp_) != nfrom_;
    } else {
      return true;
    }
  }
  bool onNode() const { return !offNode(); }

  bool operator==(LBCommKey const& k) const {
    return
      k.from_  ==  from_ and k.to_  ==  to_ and
      k.nfrom_ == nfrom_ and k.nto_ == nto_ and
      k.cat_   == cat_;
  }

  template <typename SerializerT>
  void serialize(SerializerT& s) {
    s | from_ | to_ | from_temp_ | to_temp_ | nfrom_ | nto_ | cat_;
  }
};

// Set the types for the communication graph
using CommKeyType   = LBCommKey;
using CommBytesType = double;
using CommMapType   = std::unordered_map<CommKeyType,CommBytesType>;

}}}} /* end namespace vt::vrt::collection::balance */

namespace std {

using CommCategoryType = vt::vrt::collection::balance::CommCategory;
using LBCommKeyType    = vt::vrt::collection::balance::LBCommKey;

template <>
struct hash<CommCategoryType> {
  size_t operator()(CommCategoryType const& in) const {
    using LBUnderType = typename std::underlying_type<CommCategoryType>::type;
    auto const val = static_cast<LBUnderType>(in);
    return std::hash<LBUnderType>()(val);
  }
};

template <>
struct hash<LBCommKeyType> {
  size_t operator()(LBCommKeyType const& in) const {
    return std::hash<uint64_t>()(in.from_ ^ in.to_ ^ in.nfrom_ ^ in.nto_);
  }
};

} /* end namespace std */

#endif /*INCLUDED_VT_VRT_COLLECTION_BALANCE_LB_COMM_H*/
