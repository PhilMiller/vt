/*
//@HEADER
// *****************************************************************************
//
//                                     ds.h
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

#if !defined INCLUDED_TERMINATION_DIJKSTRA_SCHOLTEN_DS_H
#define INCLUDED_TERMINATION_DIJKSTRA_SCHOLTEN_DS_H

#include "vt/config.h"
#include "vt/termination/dijkstra-scholten/ack_request.h"
#include "vt/termination/dijkstra-scholten/comm.fwd.h"
#include "vt/termination/term_parent.h"

#include <cstdlib>
#include <map>
#include <list>

namespace vt { namespace term { namespace ds {

/*
 *
 * This implementation is directly based on the Dijkstra-Scholten parental
 * responsibility termination algorithm detailed in this paper based on building
 * an engagement tree limited to the size of nodes engaged. In computations
 * where the fanout is low and well-bounded (and thus theoretically scalable),
 * this algorithm performs optimally wrt the number of nodes that communicate.
 *
 * "Termination Detection for Diffusing Computations"
 *   https://www.cs.mcgill.ca/~lli22/575/termination3.pdf
 *
 * Algorithmic-specific fault tolerance can be attained for this algorithm based
 * on this paper that extends the Dijkstra-Scholten algorithm in two ways---one
 * for independent failures, the other from related process faults:
 *
 * "Adoption Protocols for Fanout-Optimal Fault-Tolerant Termination Detection"
 *    http://jlifflander.com/papers/ppopp13termination.pdf
 */

template <typename CommType>
struct TermDS : EpochRelation {
  using CountType = int64_t;
  using AckReqListType = std::list<AckRequest>;

  TermDS(EpochType in_epoch, bool isRoot_, NodeType self_);
  TermDS(TermDS&&) = default;
  TermDS(TermDS const&) = delete;

  virtual ~TermDS() = default;

  void setRoot(bool isRoot);
  void msgSent(NodeType successor, CountType count);
  void gotAck(CountType count);
  void doneSending();
  void msgProcessed(NodeType predecessor, CountType count);
  void needAck(NodeType const predecessor, CountType const count);
  void tryAck();
  void terminated();
  bool hasParent();

private:
  void tryLast();

protected:
  NodeType parent                   = uninitialized_destination;
  NodeType self                     = uninitialized_destination;
  CountType C                       = 0;
  CountType ackedArbitrary          = 0;
  CountType ackedParent             = 0;
  CountType reqedParent             = 0;
  CountType engagementMessageCount  = 0;
  CountType D                       = 0;
  CountType processedSum            = 0;
  CountType lC                      = 0;
  CountType lD                      = 0;
  AckReqListType outstanding        = {};
};

}}} /* end namespace vt::term::ds */

#endif /*INCLUDED_TERMINATION_DIJKSTRA_SCHOLTEN_DS_H*/
