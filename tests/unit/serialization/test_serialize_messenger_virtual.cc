/*
//@HEADER
// ************************************************************************
//
//                          test_serialize_messenger_virtual.cc
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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "test_parallel_harness.h"
#include "data_message.h"

#include "vt/transport.h"

namespace vt { namespace tests { namespace unit {

using namespace vt;
using namespace vt::tests::unit;

static constexpr int const val1 = 20;
static constexpr int const val2 = 21;
static constexpr int const val3 = 22;
static constexpr int const test_val = 129;

struct TestCtx : ::vt::vrt::VirtualContext {
  TestCtx() = default;
};

struct DataMsg : vt::vrt::VirtualMessage {
  int test = 0;
  std::vector<int> vec;

  void init() {
    vec = std::vector<int>{val1,val2};
    test = test_val;
  }

  void check() {
    EXPECT_EQ(test, test_val);
    EXPECT_EQ(vec[0], val1);
    EXPECT_EQ(vec[1], val2);

    // for (auto&& elm: vec) {
    //   fmt::print("elm={}\n",elm);
    // }
  }

  template <typename SerializerT>
  void serialize(SerializerT& s) {
    s | vec;
    s | test;
  }
};

struct TestSerialMessengerVirtual : TestParallelHarness {
  using TestMsg = TestStaticBytesShortMsg<4>;

  static void testHandler(DataMsg* msg, TestCtx* ctx) {
    msg->check();
  }
};

#if HAS_SERIALIZATION_LIBRARY
TEST_F(TestSerialMessengerVirtual, test_serial_messenger_1) {
  auto const& my_node = theContext()->getNode();

  if (my_node == 0) {
    using TupleType = std::tuple<int, int>;

    auto proxy = theVirtualManager()->makeVirtual<TestCtx>();
    auto msg = makeSharedMessage<DataMsg>();
    msg->init();
    theVirtualManager()->sendSerialMsg<TestCtx, DataMsg, testHandler>(proxy, msg);
  }
}
#endif

}}} // end namespace vt::tests::unit
