
#if !defined INCLUDED_BITS_COUNTER
#define INCLUDED_BITS_COUNTER

#include "vt/config.h"

namespace vt { namespace utils {

template <typename BitField>
struct BitCounter {
  static constexpr BitCountType const value = sizeof(BitField) * 8;
};

}}  // end namespace vt::utils

#endif  /*INCLUDED_BITS_COUNTER*/
