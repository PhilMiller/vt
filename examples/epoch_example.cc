/*
//@HEADER
// *****************************************************************************
//
//                               epoch_example.cc
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

#include "vt/transport.h"

#include <cstdlib>
#include <inttypes.h>

using namespace vt;

static NodeType my_node = uninitialized_destination;
static NodeType num_nodes = uninitialized_destination;

int main(int argc, char** argv) {
  CollectiveOps::initialize(argc, argv);

  my_node = theContext()->getNode();
  num_nodes = theContext()->getNumNodes();

  {
    EpochType seq = 0xFFAA;
    auto const rooted = epoch::EpochManip::makeEpoch(seq, true, my_node, false);
    auto const is_rooted = epoch::EpochManip::isRooted(rooted);
    auto const is_user = epoch::EpochManip::isUser(rooted);
    auto const has_category = epoch::EpochManip::hasCategory(rooted);
    auto const get_seq = epoch::EpochManip::seq(rooted);
    auto const ep_node = epoch::EpochManip::node(rooted);
    auto const next = epoch::EpochManip::next(rooted);
    auto const next_seq = epoch::EpochManip::seq(next);

    fmt::print(
      "rooted epoch={}, is_rooted={}, has_cat={}, is_user={}, get_seq={}, "
      "node={}, next={}, next_seq={}, num={}, end={}\n",
      rooted, is_rooted, has_category, is_user, get_seq, ep_node, next,
      next_seq, epoch::epoch_seq_num_bits, epoch::eEpochLayout::EpochSentinelEnd
    );
    fmt::print("epoch={}, seq={}\n", rooted, get_seq);
    fmt::print("epoch {}, {:x} : seq {}, {:x}\n", rooted, rooted, get_seq, get_seq);
    fmt::print("epoch {}, {:x} : seq {}, {:x}\n", next, next, next_seq, next_seq);
  }

  while (!rt->isTerminated()) {
    runScheduler();
  }

  CollectiveOps::finalize();

  return 0;
}
