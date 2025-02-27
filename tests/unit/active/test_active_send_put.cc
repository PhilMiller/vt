/*
//@HEADER
// *****************************************************************************
//
//                           test_active_send_put.cc
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

#include <gtest/gtest.h>

#include "test_parallel_harness.h"
#include "data_message.h"

#include "vt/transport.h"

namespace vt { namespace tests { namespace unit {

using namespace vt;
using namespace vt::tests::unit;

struct PutTestMessage : ::vt::PayloadMessage {
  int num_ints = 0;

  explicit PutTestMessage(int const in_num_ints) : num_ints(in_num_ints) { }
  PutTestMessage() = default;
};

struct TestActiveSendPut : TestParameterHarnessNode {
  static NodeType from_node;
  static NodeType to_node;

  virtual void SetUp() {
    TestParameterHarnessNode::SetUp();
    from_node = 0;
    to_node = 1;
  }

  static void test_handler(PutTestMessage* msg) {
    auto ptr = static_cast<int*>(msg->getPut());
    auto size = msg->getPutSize();
    #if DEBUG_TEST_HARNESS_PRINT
      auto const& this_node = theContext()->getNode();
      fmt::print("{}: test_handler_2: size={}, ptr={}\n", this_node, size, ptr);
    #endif
    EXPECT_EQ(msg->num_ints * sizeof(int), size);
    for (int i = 0; i < msg->num_ints; i++) {
      EXPECT_EQ(ptr[i], i);
    }
  }
};

/*static*/ NodeType TestActiveSendPut::from_node;
/*static*/ NodeType TestActiveSendPut::to_node;

TEST_P(TestActiveSendPut, test_active_fn_send_put_param) {
  auto const& my_node = theContext()->getNode();

  #if DEBUG_TEST_HARNESS_PRINT
    fmt::print("test_type_safe_active_fn_send_large_put: node={}\n", my_node);
  #endif

  auto const& vec_size = GetParam();

  std::vector<int> test_vec_2(vec_size);
  for (int i = 0; i < vec_size; i++) {
    test_vec_2[i] = i;
  }

  if (my_node == from_node) {
    auto msg = makeSharedMessage<PutTestMessage>(
      static_cast<int>(test_vec_2.size())
    );
    msg->setPut(&test_vec_2[0], sizeof(int)*test_vec_2.size());
    #if DEBUG_TEST_HARNESS_PRINT
      fmt::print("{}: sendMsg: (put) i={}\n", my_node, i);
    #endif
    theMsg()->sendMsg<PutTestMessage, test_handler>(1, msg);
  }
}

INSTANTIATE_TEST_CASE_P(
  InstantiationName, TestActiveSendPut,
  ::testing::Range(static_cast<NodeType>(2), static_cast<NodeType>(512), 4),
);

}}} // end namespace vt::tests::unit
