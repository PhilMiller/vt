
#if !defined INCLUDED_GROUP_GROUP_INFO_BASE_H
#define INCLUDED_GROUP_GROUP_INFO_BASE_H

#include "config.h"
#include "group/group_common.h"
#include "group/group_info.fwd.h"
#include "collective/tree/tree.h"

#include <memory>
#include <cstdlib>

namespace vt { namespace group {

struct InfoBase {
  using WaitCountType = int32_t;
  using TreeType = collective::tree::Tree;
  using TreePtrType = std::unique_ptr<TreeType>;

protected:
  virtual GroupType getGroupID() const = 0;
  virtual ActionType getAction() const = 0;
};

}} /* end namespace vt::group */

#endif /*INCLUDED_GROUP_GROUP_INFO_BASE_H*/
