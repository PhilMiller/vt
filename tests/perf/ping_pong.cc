/*
//@HEADER
// *****************************************************************************
//
//                                 ping_pong.cc
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

#include <cassert>
#include <cstdint>

#include <fmt/format.h>

#include "vt/transport.h"

#define DEBUG_PING_PONG 0
#define REUSE_MESSAGE_PING_PONG 0

using namespace vt;

static constexpr int64_t const min_bytes = 1;
static constexpr int64_t const max_bytes = 16384;

static int64_t num_pings = 1024;

static constexpr NodeType const ping_node = 0;
static constexpr NodeType const pong_node = 1;

static double startTime = 0.0;

template <int64_t num_bytes>
struct PingMsg : ShortMessage {
  int64_t count = 0;
  std::array<char, num_bytes> payload;

  PingMsg() : ShortMessage() { }
  PingMsg(int64_t const in_count) : ShortMessage(), count(in_count) { }
};

template <int64_t num_bytes>
struct FinishedPingMsg : ShortMessage {
  int64_t prev_bytes = 0;

  FinishedPingMsg(int64_t const in_prev_bytes)
    : ShortMessage(), prev_bytes(in_prev_bytes)
  { }
};

template <int64_t num_bytes>
static void pingPong(PingMsg<num_bytes>* in_msg);

template <int64_t num_bytes>
static void finishedPing(FinishedPingMsg<num_bytes>* msg);

static void printTiming(int64_t const& num_bytes) {
  double const total = MPI_Wtime() - startTime;

  startTime = MPI_Wtime();

  fmt::print(
    "{}: Finished num_pings={}, bytes={}, total time={}, time/msg={}\n",
    theContext()->getNode(), num_pings, num_bytes, total, total/num_pings
  );
}

template <int64_t num_bytes>
static void finishedPing(FinishedPingMsg<num_bytes>* msg) {
  printTiming(num_bytes);

  if (num_bytes != max_bytes) {
    auto pmsg = makeSharedMessage<PingMsg<num_bytes * 2>>();
    theMsg()->sendMsg<PingMsg<num_bytes * 2>, pingPong>(
      pong_node, pmsg
    );
  }
}

template <>
void finishedPing<max_bytes>(FinishedPingMsg<max_bytes>* msg) {
  printTiming(max_bytes);
}

template <int64_t num_bytes>
static void pingPong(PingMsg<num_bytes>* in_msg) {
  auto const& cnt = in_msg->count;

  #if DEBUG_PING_PONG
    fmt::print(
      "{}: pingPong: cnt={}, bytes={}\n",
      theContext()->getNode(), cnt, num_bytes
    );
  #endif

  #if REUSE_MESSAGE_PING_PONG
    in_msg->count++;
  #endif

  if (cnt >= num_pings) {
    auto msg = makeSharedMessage<FinishedPingMsg<num_bytes>>(num_bytes);
    theMsg()->sendMsg<FinishedPingMsg<num_bytes>, finishedPing>(
      0, msg
    );
  } else {
    NodeType const next =
      theContext()->getNode() == ping_node ? pong_node : ping_node;
    #if REUSE_MESSAGE_PING_PONG
      // @todo: fix this memory allocation problem
      theMsg()->sendMsg<PingMsg<num_bytes>, pingPong>(
        next, in_msg, [=]{ /*delete in_msg;*/ }
      );
    #else
      auto m = makeSharedMessage<PingMsg<num_bytes>>(cnt + 1);
      theMsg()->sendMsg<PingMsg<num_bytes>, pingPong>(next, m);
    #endif
  }
}

int main(int argc, char** argv) {
  CollectiveOps::initialize(argc, argv);

  auto const& my_node = theContext()->getNode();
  auto const& num_nodes = theContext()->getNumNodes();

  if (num_nodes == 1) {
    CollectiveOps::abort("At least 2 ranks required");
  }

  if (argc > 1) {
    num_pings = atoi(argv[1]);
  }

  startTime = MPI_Wtime();

  if (my_node == 0) {
    auto m = makeSharedMessage<PingMsg<min_bytes>>();
    theMsg()->sendMsg<PingMsg<min_bytes>, pingPong>(pong_node, m);
  }

  while (!rt->isTerminated()) {
    runScheduler();
  }

  CollectiveOps::finalize();

  return 0;
}
