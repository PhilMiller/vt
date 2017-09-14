
#if ! defined __RUNTIME_TRANSPORT_BARRIER_MSGS__
#define __RUNTIME_TRANSPORT_BARRIER_MSGS__

#include "config.h"
#include "message.h"

namespace vt { namespace barrier {

struct BarrierMsg : vt::ShortMessage {
  bool is_named, is_wait, skip_term = false;
  BarrierType barrier;

  BarrierMsg(
    bool const& in_is_named, BarrierType const& in_barrier, bool const& in_is_wait
  )
    : ShortMessage(), is_named(in_is_named), is_wait(in_is_wait),
      skip_term(false), barrier(in_barrier)
  { }
};

struct BarrierState {
  BarrierType barrier;

  int recv_event_count = 0;
  bool is_wait = false;
  bool is_named = false;
  bool released = false;

  ActionType cont_action = nullptr;

  BarrierState(
    bool const& in_is_named, BarrierType const& in_barrier, bool const& in_is_wait
  ) : barrier(in_barrier), is_wait(in_is_wait), is_named(in_is_named)
  { }
};

}} //end namespace vt::barrier

#endif /*__RUNTIME_TRANSPORT_BARRIER_MSGS__*/
