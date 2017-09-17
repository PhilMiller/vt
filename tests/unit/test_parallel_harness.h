
#if ! defined __VIRTUAL_TRANSPORT_TEST_PARALLEL_HARNESS__
#define __VIRTUAL_TRANSPORT_TEST_PARALLEL_HARNESS__

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <string>

#include "test_config.h"
#include "test_harness.h"

#include "transport.h"

static bool mpi_is_initialized = false;

template <typename TestBase>
struct TestParallelHarnessAny : TestHarnessAny<TestBase> {
  virtual void SetUp() {
    using namespace vt;

    TestHarnessAny<TestBase>::SetUp();

    if (not mpi_is_initialized) {
      CollectiveOps::initializeContext(0, nullptr);
      mpi_is_initialized = true;
    }

    CollectiveOps::initializeSingletons();
    CollectiveOps::initializeRuntime();

    #if DEBUG_TEST_HARNESS_PRINT
      auto const& my_node = theContext->getNode();
      auto const& num_nodes = theContext->getNumNodes();
      printf("my_node=%d, num_nodes=%d\n", my_node, num_nodes);
    #endif
  }

  virtual void TearDown() {
    using namespace vt;

    while (vtIsWorking) {
      runScheduler();
    }

    #if DEBUG_TEST_HARNESS_PRINT
      auto const& my_node = theContext->getNode();
      printf("my_node=%d, tearing down runtime\n", my_node);
    #endif

    CollectiveOps::finalizeRuntime();
    CollectiveOps::finalizeSingletons();

    TestHarnessAny<TestBase>::TearDown();
  }
};

using TestParallelHarness = TestParallelHarnessAny<testing::Test>;

template <typename ParamT>
using TestParallelHarnessParam = TestParallelHarnessAny<
  testing::TestWithParam<ParamT>
>;

using TestParameterHarnessNode = TestParallelHarnessParam<vt::NodeType>;

#endif /* __VIRTUAL_TRANSPORT_TEST_PARALLEL_HARNESS__ */
