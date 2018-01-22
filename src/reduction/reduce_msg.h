
#if !defined INCLUDED_REDUCTION_REDUCE_MSG_H
#define INCLUDED_REDUCTION_REDUCE_MSG_H

#include "config.h"
#include "messaging/message.h"

#include <cstdlib>

namespace vt { namespace reduction {

struct ReduceMsg;

struct ReduceLink {
  using MsgCountType = uint16_t;

  bool is_root = false;
  ReduceMsg* next = nullptr;
  MsgCountType count = 0;
};

struct ReduceMsg : ::vt::Message, ReduceLink {
  NodeType reduce_root_ = uninitialized_destination;
  TagType reduce_tag_ = no_tag;
  EpochType reduce_epoch_ = no_epoch;
  HandlerType combine_handler_ = uninitialized_handler;
};

}} /* end namespace vt::reduction */

#endif /*INCLUDED_REDUCTION_REDUCE_MSG_H*/
