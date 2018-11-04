
#include "vt/config.h"
#include "vt/group/group_common.h"
#include "vt/group/collective/group_collective_msg.h"
#include "vt/group/collective/group_collective_util.h"

namespace vt { namespace group {

bool
GroupCollSort::operator()(GroupCollectiveMsg* a, GroupCollectiveMsg* b) const {
  return a->getSubtreeSize() < b->getSubtreeSize();
}

}} /* end namespace vt::group */
