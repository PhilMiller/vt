/*
//@HEADER
// *****************************************************************************
//
//                                 test_seq.cc
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

using namespace vt;

static NodeType my_node = uninitialized_destination;
static NodeType num_nodes = uninitialized_destination;

struct EmptyMsg : vt::Message {
  EmptyMsg() : Message() { }
};

#define PRINT_SEQUENCE_ON 1

#if PRINT_SEQUENCE_ON
#define PRINT_SEQUENCE(...)                                             \
  do {                                                                  \
    vt_print(                                                           \
      sequence, __VA_ARGS__                                             \
    );                                                                  \
  } while (0);
#else
#define PRINT_SEQUENCE(...)
#endif

SEQUENCE_REGISTER_HANDLER(EmptyMsg, action1)

#pragma GCC diagnostic ignored "-Wunused-function"
static void mySeq(SeqType const& seq_id) {
  PRINT_SEQUENCE("mySeq: executing sequence\n");

  theSeq()->wait<EmptyMsg, action1>(10, [](EmptyMsg* msg){
    PRINT_SEQUENCE("action1 WAIT-1 triggered\n");
  });

  theSeq()->sequenced([]{
    PRINT_SEQUENCE("action1 sequenced_block triggered\n");
    auto const& this_node = theContext()->getNode();
    theMsg()->sendMsg<EmptyMsg, action1>(
      this_node, makeSharedMessage<EmptyMsg>(), 20
    );
  });

  theSeq()->sequenced(seq_id, [=]{
    PRINT_SEQUENCE("SEQUENCED: (begin) unraveling child closure triggered\n\n");

    theSeq()->wait<EmptyMsg, action1>(20, [](EmptyMsg* msg){
      PRINT_SEQUENCE("action1 WAIT-2 triggered\n");
    });

    theSeq()->wait<EmptyMsg, action1>(30, [](EmptyMsg* msg){
      PRINT_SEQUENCE("action1 WAIT-3 triggered\n");

      auto const& nd = my_node == 0 ? 1 : 0;
      // send messages for the parallel region following
      //theMsg()->sendMsg<EmptyMsg, action1>(nd, makeSharedMessage<EmptyMsg>(), 229);
      //theMsg()->sendMsg<EmptyMsg, action1>(nd, makeSharedMessage<EmptyMsg>(), 129);

      theMsg()->sendMsg<EmptyMsg, action1>(nd, makeSharedMessage<EmptyMsg>(), 900);
    });

    PRINT_SEQUENCE("SEQUENCED: (end) unraveling child closure triggered\n\n");
  });

  theSeq()->sequenced([]{
    theSeq()->wait<EmptyMsg, action1>(900, [](EmptyMsg* msg){
      PRINT_SEQUENCE("action1 WAIT-6 FINAL triggered\n");
    });
  });
}

#pragma GCC diagnostic ignored "-Wunused-function"
static void mySeqParallel(SeqType const& seq_id) {
  PRINT_SEQUENCE("mySeqParallel: executing sequence\n");

  theSeq()->wait<EmptyMsg, action1>(10, [](EmptyMsg* msg){
    PRINT_SEQUENCE("action1 w1 triggered\n");
    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 20);
    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 30);
    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 40);
  });

  theSeq()->parallel(seq_id, []{
    theSeq()->wait<EmptyMsg, action1>(20, [](EmptyMsg* msg){
      PRINT_SEQUENCE("action1 w2-par-1 parallel triggered\n");
    });
  }, []{
    theSeq()->wait<EmptyMsg, action1>(30, [](EmptyMsg* msg){
      PRINT_SEQUENCE("action1 w2-par-2 parallel triggered\n");
    });
  });

  theSeq()->sequenced([]{
    theSeq()->wait<EmptyMsg, action1>(40, [](EmptyMsg* msg){
      PRINT_SEQUENCE("action1 w3 FINAL triggered\n");
    });
  });
}

static void mySeqFor(SeqType const& seq_id) {
  PRINT_SEQUENCE("mySeqParallel: executing sequence\n");

  theSeq()->for_loop(0, 10, 1, [](vt::seq::ForIndex i) {
    PRINT_SEQUENCE("for loop: i={}\n", i);
    theSeq()->wait<EmptyMsg, action1>([](EmptyMsg* msg){
      PRINT_SEQUENCE("action1 triggered\n");
      theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>());
    });
  });
}

#pragma GCC diagnostic ignored "-Wunused-function"
static void mySeqSingleNode(SeqType const& seq_id) {
  PRINT_SEQUENCE("mySeqSingleNode: executing sequence\n");

  theSeq()->wait<EmptyMsg, action1>(10, [](EmptyMsg* msg){
    PRINT_SEQUENCE("action1 WAIT-1 triggered\n");
  });

  theSeq()->sequenced([]{
    PRINT_SEQUENCE("action1 sequenced_block triggered\n");
    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 20);
  });

  theSeq()->sequenced(seq_id, [=]{
    PRINT_SEQUENCE("SEQUENCED: (begin) unraveling child closure triggered\n");

    theSeq()->wait<EmptyMsg, action1>(20, [](EmptyMsg* msg){
      PRINT_SEQUENCE("action1 WAIT-2 triggered\n");
    });

    theSeq()->wait<EmptyMsg, action1>(30, [](EmptyMsg* msg){
      PRINT_SEQUENCE("action1 WAIT-3 triggered\n");
      theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 900);
    });

    PRINT_SEQUENCE("SEQUENCED: (end) unraveling child closure triggered\n");
  });

  theSeq()->sequenced([]{
    PRINT_SEQUENCE("SEQUENCED: (begin) unraveling FINAL child closure triggered\n");

    theSeq()->wait<EmptyMsg, action1>(900, [](EmptyMsg* msg){
      PRINT_SEQUENCE("action1 WAIT-6 FINAL triggered\n");
    });
  });
}

#pragma GCC diagnostic ignored "-Wunused-function"
static void simpleSeq(SeqType const& seq_id) {
  PRINT_SEQUENCE("simpleSeq: executing sequence: seq_id={}\n", seq_id);

  theSeq()->wait<EmptyMsg, action1>(10, [](EmptyMsg* msg){
    auto const& cur_seq_id = theSeq()->getCurrentSeq();
    PRINT_SEQUENCE(
      "simpleSeq: seq={}: w1 triggered: msg={}\n", cur_seq_id, print_ptr(msg)
    );
  });

  theSeq()->sequenced([]{
    PRINT_SEQUENCE("SEQUENCED: (begin) unraveling child closure\n");
    theSeq()->wait<EmptyMsg, action1>(100, [](EmptyMsg* msg){
      PRINT_SEQUENCE("simpleSeq: w100 inner triggered\n");
    });
    theSeq()->wait<EmptyMsg, action1>(200, [](EmptyMsg* msg){
      PRINT_SEQUENCE("simpleSeq: w200 inner triggered\n");
    });
    PRINT_SEQUENCE("SEQUENCED: (end) unraveling child closure\n");
  });

  theSeq()->wait<EmptyMsg, action1>(20, [](EmptyMsg* msg){
    auto const& cur_seq_id = theSeq()->getCurrentSeq();
    PRINT_SEQUENCE(
      "simpleSeq: seq={}: w2 triggered: msg={}\n", cur_seq_id, print_ptr(msg)
    );
    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 30);
  });

  theSeq()->wait<EmptyMsg, action1>(30, [](EmptyMsg* msg){
    auto const& cur_seq_id = theSeq()->getCurrentSeq();
    PRINT_SEQUENCE("simpleSeq: seq={}: w3 triggered\n", cur_seq_id);
  });

  theSeq()->sequenced([]{
    PRINT_SEQUENCE("SEQUENCED: (begin) unraveling child closure 2\n");
    theSeq()->wait<EmptyMsg, action1>(1000, [](EmptyMsg* msg){
      PRINT_SEQUENCE("simpleSeq: w1000 inner triggered\n");
      theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 40);
    });

    theSeq()->sequenced([]{
      PRINT_SEQUENCE("SEQUENCED: (begin) unraveling child closure 2a\n");
      theSeq()->wait<EmptyMsg, action1>(10000, [](EmptyMsg* msg){
        PRINT_SEQUENCE("simpleSeq: w10000 inner-inner triggered\n");
      });
      theSeq()->wait<EmptyMsg, action1>(20000, [](EmptyMsg* msg){
        PRINT_SEQUENCE("simpleSeq: w20000 inner-inner triggered\n");
      });
      PRINT_SEQUENCE("SEQUENCED: (end) unraveling child closure 2a\n");
    });

    theSeq()->wait<EmptyMsg, action1>(2000, [](EmptyMsg* msg){
      PRINT_SEQUENCE("simpleSeq: w2000 inner triggered\n");
    });
    PRINT_SEQUENCE("SEQUENCED: (end) unraveling child closure 2\n");
  });

  theSeq()->wait<EmptyMsg, action1>(40, [](EmptyMsg* msg){
    auto const& cur_seq_id = theSeq()->getCurrentSeq();
    PRINT_SEQUENCE("simpleSeq: seq={}: w4 triggered\n", cur_seq_id);
  });
}

int main(int argc, char** argv) {
  CollectiveOps::initialize(argc, argv);

  my_node = theContext()->getNode();
  num_nodes = theContext()->getNumNodes();

  //#define SIMPLE_SEQ_MULTI_NODE 1
  //#define SIMPLE_SEQ_PARALLEL 1
  #define SIMPLE_SEQ_FOR 1

  #if SIMPLE_SEQ_MULTI_NODE
  if (num_nodes == 1) {
    CollectiveOps::output("requires at least 2 nodes");
    CollectiveOps::finalize();
    return 0;
  }
  #endif

  #if SIMPLE_SEQ
  if (my_node == 0) {
    SeqType const& seq_id = theSeq()->nextSeq();
    theSeq()->sequenced(seq_id, simpleSeq);

    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 10);
    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 20);

    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 100);
    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 200);

    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 1000);
    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 2000);

    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 10000);
    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 20000);
  }
  #endif

  #if SIMPLE_SEQ_MULTI_NODE
  if (my_node == 0) {
    SeqType const& seq_id = theSeq()->nextSeq();
    theSeq()->sequenced(seq_id, mySeq);

    theMsg()->sendMsg<EmptyMsg, action1>(1, makeSharedMessage<EmptyMsg>(), 30);
    theMsg()->sendMsg<EmptyMsg, action1>(1, makeSharedMessage<EmptyMsg>(), 10);

    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 30);
    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 10);
  } else if (my_node == 1) {
    SeqType const& seq_id2 = theSeq()->nextSeq();
    theSeq()->sequenced(seq_id2, mySeq);
  }
  #endif

  #if SIMPLE_SEQ_SINGLE_NODE
  if (my_node == 0) {
    SeqType const& seq_id = theSeq()->nextSeq();
    theSeq()->sequenced(seq_id, mySeqSingleNode);
    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 30);
    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 10);
  }
  #endif

  #if SIMPLE_SEQ_PARALLEL
  if (my_node == 0) {
    SeqType const& seq_id = theSeq()->nextSeq();
    theSeq()->sequenced(seq_id, mySeqParallel);
    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>(), 10);
  }
  #endif

  #if SIMPLE_SEQ_FOR
  if (my_node == 0) {
    SeqType const& seq_id = theSeq()->nextSeq();
    theSeq()->sequenced(seq_id, mySeqFor);
    theMsg()->sendMsg<EmptyMsg, action1>(0, makeSharedMessage<EmptyMsg>());
  }
  #endif

  while (!rt->isTerminated()) {
    runScheduler();
  }

  CollectiveOps::finalize();

  return 0;
}
