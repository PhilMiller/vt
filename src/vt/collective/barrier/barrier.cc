
#include "vt/collective/barrier/barrier.h"
#include "vt/collective/collective_alg.h"
#include "vt/messaging/active.h"

namespace vt { namespace collective { namespace barrier {

Barrier::Barrier() :
  tree::Tree(tree::tree_cons_tag_t)
{ }

/*static*/ void Barrier::barrierUp(BarrierMsg* msg) {
  theCollective()->barrierUp(
    msg->is_named, msg->is_wait, msg->barrier, msg->skip_term
  );
}

/*static*/ void Barrier::barrierDown(BarrierMsg* msg) {
  theCollective()->barrierDown(msg->is_named, msg->is_wait, msg->barrier);
}

Barrier::BarrierStateType& Barrier::insertFindBarrier(
  bool const& is_named, bool const& is_wait, BarrierType const& barrier,
  ActionType cont_action
) {
  auto& state = is_named ? named_barrier_state_ : unnamed_barrier_state_;

  auto iter = state.find(barrier);
  if (iter == state.end()) {
    state.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(barrier),
      std::forward_as_tuple(BarrierStateType{is_named, barrier, is_wait})
    );
    iter = state.find(barrier);
  }

  if (cont_action != nullptr) {
    assert(
      (not is_wait) and
      "Barrier must not be waiting kind if action is associated"
    );

    iter->second.cont_action = cont_action;
  }

  return iter->second;
}

void Barrier::removeBarrier(
  bool const& is_named, bool const& is_wait, BarrierType const& barrier
) {
  auto& state = is_named ? named_barrier_state_ : unnamed_barrier_state_;

  auto iter = state.find(barrier);
  assert(
    iter != state.end() and "Barrier must exist at this point"
  );

  state.erase(iter);
}

BarrierType Barrier::newNamedCollectiveBarrier() {
  return cur_named_coll_barrier_++;
}

BarrierType Barrier::newNamedBarrier() {
  BarrierType const next_barrier = cur_named_barrier_++;
  NodeType const cur_node = theContext()->getNode();
  BarrierType const cur_node_shift = static_cast<BarrierType>(cur_node) << 32;
  BarrierType const barrier_name = next_barrier | cur_node_shift;
  return barrier_name;
}

void Barrier::waitBarrier(
  ActionType poll_action, BarrierType const& named, bool const skip_term
) {
  bool const is_wait = true;
  bool const is_named = named != no_barrier;

  BarrierType const barrier = is_named ? named : cur_unnamed_barrier_++;

  auto& barrier_state = insertFindBarrier(is_named, is_wait, barrier);

  debug_print(
    barrier, node,
    "waitBarrier: named={}, barrier={}\n", is_named, barrier
  );

  barrierUp(is_named, is_wait, barrier, skip_term);

  while (not barrier_state.released) {
    theMsg()->scheduler();
    if (poll_action) {
      poll_action();
    }
  }

  debug_print(
    barrier, node,
    "waitBarrier: released: named={}, barrier={}\n", is_named, barrier
  );

  removeBarrier(is_named, is_wait, barrier);
}

void Barrier::contBarrier(
  ActionType fn, BarrierType const& named, bool const skip_term
) {
  debug_print(
    barrier, node,
    "contBarrier: named_barrier={}, skip_term={}\n", named, skip_term
  );

  bool const is_wait = false;
  bool const is_named = named != no_barrier;

  BarrierType const barrier = is_named ? named : cur_unnamed_barrier_++;

  auto& barrier_state = insertFindBarrier(is_named, is_wait, barrier, fn);

  debug_print(
    barrier, node,
    "contBarrier: named={}, barrier={}\n", is_named, barrier
  );

  barrierUp(is_named, is_wait, barrier, skip_term);
}

void Barrier::barrierDown(
  bool const& is_named, bool const& is_wait, BarrierType const& barrier
) {
  auto& barrier_state = insertFindBarrier(is_named, is_wait, barrier);

  debug_print(
    barrier, node,
    "barrierUp: invoking: named={}, wait={}, barrier={}\n",
    is_named, is_wait, barrier
  );

  barrier_state.released = true;

  if (not is_wait and barrier_state.cont_action != nullptr) {
    barrier_state.cont_action();
  }
}

void Barrier::barrierUp(
  bool const& is_named, bool const& is_wait, BarrierType const& barrier,
  bool const& skip_term
) {
  auto const& num_children_ = getNumChildren();
  bool const& is_root_ = isRoot();
  auto const& parent_ = getParent();

  // ToDo: Why we call this function again? (if you come from contBarrier,
  // ToDo: this is already called)
  auto& barrier_state = insertFindBarrier(is_named, is_wait, barrier);

  barrier_state.recv_event_count += 1;

  bool const is_ready = barrier_state.recv_event_count == num_children_ + 1;

  debug_print(
    barrier, node,
    "barrierUp: invoking: named={}, wait={}, ready={}, events={}, barrier={}\n",
    is_named, is_wait, is_ready, barrier_state.recv_event_count, barrier
  );

  if (is_ready) {
    if (not is_root_) {
      auto msg = makeSharedMessage<BarrierMsg>(is_named, barrier, is_wait);
      // system-level barriers can choose to skip the termination protocol
      if (skip_term) {
        theMsg()->setTermMessage(msg);
      }
      debug_print(
        barrier, node,
        "barrierUp: barrier={}\n", barrier
      );
      theMsg()->sendMsg<BarrierMsg, barrierUp>(parent_, msg);
    } else {
      auto msg = makeSharedMessage<BarrierMsg>(is_named, barrier, is_wait);
      // system-level barriers can choose to skip the termination protocol
      if (skip_term) {
        theMsg()->setTermMessage(msg);
      }
      debug_print(
        barrier, node,
        "barrierDown: barrier={}\n", barrier
      );
      theMsg()->broadcastMsg<BarrierMsg, barrierDown>(msg);
      barrierDown(is_named, is_wait, barrier);
    }
  }
}

}}}  // end namespace vt::collective::barrier
