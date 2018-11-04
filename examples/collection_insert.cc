
#include "vt/transport.h"

#include <cstdlib>

using namespace ::vt;
using namespace ::vt::collective;
using namespace ::vt::index;
using namespace ::vt::mapping;

static constexpr int32_t const default_num_elms = 64;

struct InsertCol : InsertableCollection<InsertCol,Index1D> {
  InsertCol()
    : InsertableCollection<InsertCol,Index1D>()
  {
    auto const& this_node = theContext()->getNode();
    ::fmt::print("{}: constructing: idx={}\n", this_node, getIndex().x());
 }
};

struct TestMsg : CollectionMessage<InsertCol> { };

static void work(TestMsg* msg, InsertCol* col) {
  auto const& this_node = theContext()->getNode();
  ::fmt::print("work: node={}, idx={}\n", this_node, col->getIndex().x());
}

int main(int argc, char** argv) {
  CollectiveOps::initialize(argc, argv);

  auto const& this_node = theContext()->getNode();
  auto const& num_nodes = theContext()->getNumNodes();

  int32_t num_elms = default_num_elms;

  if (argc > 1) {
    num_elms = atoi(argv[1]);
  }

  auto const epoch = theTerm()->newEpoch();
  theMsg()->setGlobalEpoch(epoch);

  if (this_node == 0) {
    auto const& range = Index1D(num_elms);
    auto proxy = theCollection()->construct<InsertCol>(range);

    theTerm()->addActionEpoch(epoch,[=]{
      ::fmt::print("broadcasting\n");
      auto msg = makeSharedMessage<TestMsg>();
      proxy.broadcast<TestMsg,work>(msg);
    });
    for (int i = 0; i < range.x() / 2; i++) {
      proxy[i].insert(i % 2);
      /*
       * Alternative syntax:
       *   theCollection()->insert<InsertCol>(proxy, Index1D(i));
       */
    }
    theTerm()->finishedEpoch(epoch);
    ::fmt::print("calling finished insert\n");
    proxy.finishedInserting([=]{
      ::fmt::print("insertions are finished\n");
      for (int i = range.x()/2; i < range.x(); i++) {
        proxy[i].insert(i % 2);
      }
      proxy.finishedInserting([proxy]{
        ::fmt::print("insertions are finished2\n");
      });
    });
  } else {
    theTerm()->finishedEpoch(epoch);
  }

  while (!rt->isTerminated()) {
    runScheduler();
  }

  CollectiveOps::finalize();

  return 0;
}
