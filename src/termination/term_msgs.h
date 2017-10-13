
#if ! defined __RUNTIME_TRANSPORT_TERMINATION_MSGS__
#define __RUNTIME_TRANSPORT_TERMINATION_MSGS__

#include "config.h"
#include "messaging/message.h"
#include "term_state.h"

namespace vt { namespace term {

struct TermMsg : vt::ShortMessage {
  EpochType new_epoch = no_epoch;
  TermCounterType wave = -1;

  explicit TermMsg(EpochType const& in_new_epoch, TermCounterType in_wave = -1)
    : ShortMessage(), new_epoch(in_new_epoch), wave(in_wave)
  { }
};

struct TermCounterMsg : vt::ShortMessage {
  EpochType epoch = no_epoch;
  TermCounterType prod = 0, cons = 0;

  TermCounterMsg(
    EpochType const& in_epoch, TermCounterType const& in_prod,
    TermCounterType const& in_cons
  ) : ShortMessage(), epoch(in_epoch), prod(in_prod), cons(in_cons)
  { }
};

}} //end namespace vt::term

#endif /*__RUNTIME_TRANSPORT_TERMINATION_MSGS__*/
