
#include "config.h"
#include "context/context.h"
#include "group/group_common.h"
#include "group/group_manager.h"
#include "group/id/group_id.h"
#include "group/region/group_region.h"
#include "group/group_info.h"
#include "group/global/group_default.h"
#include "group/global/group_default_msg.h"
#include "scheduler/scheduler.h"
#include "collective/collective_alg.h"

namespace vt { namespace group {

GroupType GroupManager::newGroup(
  RegionPtrType in_region, bool const& is_collective, bool const& is_static,
  ActionGroupType action
) {
  assert(!is_collective && "Must not be collective");
  return newLocalGroup(std::move(in_region), is_static, action);
}

GroupType GroupManager::newGroup(
  RegionPtrType in_region, ActionGroupType action
) {
  bool const is_static = true;
  bool const is_collective = false;
  return newGroup(std::move(in_region), is_collective, is_static, action);
}

GroupType GroupManager::newGroupCollective(
  bool const in_group, ActionGroupType action
) {
  bool const is_static = true;
  return newCollectiveGroup(in_group, is_static, action);
}

GroupType GroupManager::newCollectiveGroup(
  bool const& is_in_group, bool const& is_static, ActionGroupType action
) {
  auto const& this_node = theContext()->getNode();
  auto new_id = next_collective_group_id_++;
  bool const is_collective = true;
  auto const& group = GroupIDBuilder::createGroupID(
    new_id, this_node, is_collective, is_static
  );
  auto group_action = std::bind(action, group);
  initializeLocalGroupCollective(group, is_static, group_action, is_in_group);
  return group;
}

GroupType GroupManager::newLocalGroup(
  RegionPtrType in_region, bool const& is_static, ActionGroupType action
) {
  auto const& this_node = theContext()->getNode();
  auto new_id = next_group_id_++;
  bool const is_collective = false;
  auto const& group = GroupIDBuilder::createGroupID(
    new_id, this_node, is_collective, is_static
  );
  auto const& size = in_region->getSize();
  auto group_action = std::bind(action, group);
  initializeLocalGroup(
    group, std::move(in_region), is_static, group_action, size
  );
  return group;
}

bool GroupManager::inGroup(GroupType const& group) {
  auto iter = local_collective_group_info_.find(group);
  assert(iter != local_collective_group_info_.end() && "Must exist");
  return iter->second->inGroup();
}

GroupManager::ReducePtrType GroupManager::groupReduce(GroupType const& group) {
  auto iter = local_collective_group_info_.find(group);
  assert(iter != local_collective_group_info_.end() && "Must exist");
  auto const& is_default_group = iter->second->isGroupDefault();
  if (!is_default_group) {
    return iter->second->getReduce();
  } else {
    return theCollective();
  }
}

NodeType GroupManager::groupRoot(GroupType const& group) const {
  auto iter = local_collective_group_info_.find(group);
  assert(iter != local_collective_group_info_.end() && "Must exist");
  auto const& root = iter->second->getRoot();
  assert(root != uninitialized_destination && "Must have valid root");
  return root;
}

bool GroupManager::groupDefault(GroupType const& group) const {
  auto iter = local_collective_group_info_.find(group);
  assert(iter != local_collective_group_info_.end() && "Must exist");
  auto const& def = iter->second->isGroupDefault();
  return def;
}

RemoteOperationIDType GroupManager::registerContinuation(ActionType action) {
  RemoteOperationIDType next_id = cur_id_++;
  continuation_actions_.emplace(
    std::piecewise_construct,
    std::forward_as_tuple(next_id),
    std::forward_as_tuple(ActionListType{action})
  );
  return next_id;
}

void GroupManager::registerContinuation(
  RemoteOperationIDType const& op, ActionType action
) {
  continuation_actions_[op].push_back(action);
}

void GroupManager::triggerContinuation(RemoteOperationIDType const& op) {
  auto iter = continuation_actions_.find(op);
  if (iter != continuation_actions_.end()) {
    for (auto&& elm : iter->second) {
      elm();
    }
    continuation_actions_.erase(iter);
  }
}

void GroupManager::initializeRemoteGroup(
  GroupType const& group, RegionPtrType in_region, bool const& is_static,
  RegionType::SizeType const& group_size
) {
  bool const remote = true;
  auto group_info = std::make_unique<GroupInfoType>(
    info_rooted_remote_cons, std::move(in_region), group, group_size
  );
  auto group_ptr = group_info.get();
  remote_group_info_.emplace(
    std::piecewise_construct,
    std::forward_as_tuple(group),
    std::forward_as_tuple(std::move(group_info))
  );
  group_ptr->setup();
}

void GroupManager::initializeLocalGroupCollective(
  GroupType const& group, bool const& is_static, ActionType action,
  bool const in_group
) {
  auto group_info = std::make_unique<GroupInfoType>(
    info_collective_cons, action, group, in_group
  );
  auto group_ptr = group_info.get();
  local_collective_group_info_.emplace(
    std::piecewise_construct,
    std::forward_as_tuple(group),
    std::forward_as_tuple(std::move(group_info))
  );
  group_ptr->setup();
}

void GroupManager::initializeLocalGroup(
  GroupType const& group, RegionPtrType in_region, bool const& is_static,
  ActionType action, RegionType::SizeType const& group_size
) {
  bool const remote = false;
  auto group_info = std::make_unique<GroupInfoType>(
    info_rooted_local_cons, std::move(in_region), action, group, group_size
  );
  auto group_ptr = group_info.get();
  local_group_info_.emplace(
    std::piecewise_construct,
    std::forward_as_tuple(group),
    std::forward_as_tuple(std::move(group_info))
  );
  group_ptr->setup();
}

/*static*/ EventType GroupManager::groupHandler(
  BaseMessage* base, NodeType const& from, MsgSizeType const& size,
  bool const root, ActionType act, bool* const deliver
) {
  auto const& msg = reinterpret_cast<ShortMessage* const>(base);
  auto const& group = envelopeGetGroup(msg->env);
  auto const& is_bcast = envelopeIsBcast(msg->env);
  auto const& dest = envelopeGetDest(msg->env);

  debug_print(
    group, node,
    "GroupManager::groupHandler: size={}, root={}, from={}, group={:x}, "
    "bcast={}, dest={}\n",
    size, root, from, group, is_bcast, dest
  );

  if (is_bcast) {
    // Deliver the message normally if it's not a the root of a broadcast
    *deliver = !root;
    if (group == default_group) {
      return global::DefaultGroup::broadcast(base,from,size,root,act);
    } else {
      auto const& is_collective_group = GroupIDBuilder::isCollective(group);
      if (is_collective_group) {
        return theGroup()->sendGroupCollective(base,from,size,root,act,deliver);
      } else {
        return theGroup()->sendGroup(base,from,size,root,act,deliver);
      }
    }
  } else {
    *deliver = true;
  }
  return no_event;
}

EventType GroupManager::sendGroupCollective(
  BaseMessage* base, NodeType const& from, MsgSizeType const& size,
  bool const is_root, ActionType action, bool* const deliver
) {
  auto const& send_tag = static_cast<messaging::MPI_TagType>(
    messaging::MPITag::ActiveMsgTag
  );
  auto const& msg = reinterpret_cast<ShortMessage* const>(base);
  auto const& group = envelopeGetGroup(msg->env);
  auto iter = local_collective_group_info_.find(group);
  assert(iter != local_collective_group_info_.end() && "Must exist");
  auto const& info = *iter->second;
  auto const& in_group = info.inGroup();
  auto const& group_ready = info.isReady();
  auto const& has_action = action != nullptr;
  auto const& is_shared = isSharedMessage(msg);
  EventRecordType* parent = nullptr;

  if (in_group && group_ready) {
    auto const& this_node = theContext()->getNode();
    auto const& dest = envelopeGetDest(msg->env);
    auto const& is_bcast = envelopeIsBcast(msg->env);
    auto const& is_group_collective = GroupIDBuilder::isCollective(group);
    auto const& root_node = info.getRoot();
    auto const& send_to_root = is_root && this_node != root_node;
    auto const& this_node_dest = dest == this_node;
    auto const& first_send = from == uninitialized_destination;

    assert(is_group_collective && "This must be a collective group");

    debug_print(
      group, node,
      "GroupManager::sendGroupCollective: group={:x}, collective={}, "
      "in_group={}, group root={}, is_root={}, dest={}, from={}, is_bcast={}\n",
      group, is_group_collective, in_group, root_node, is_root, dest, from,
      is_bcast
    );

    EventType event = no_event;
    auto const& tree = info.getTree();
    auto const& num_children = tree->getNumChildren();
    auto const& node = theContext()->getNode();

    debug_print(
      group, node,
      "GroupManager::sendGroupCollective: group={:x}, collective={}, "
      "num_children={}\n",
      group, is_group_collective, num_children
    );

    if ((num_children > 0 || send_to_root) && (!this_node_dest || first_send)) {
      if (has_action) {
        event = theEvent()->createParentEvent(node);
        auto& holder = theEvent()->getEventHolder(event);
        parent = holder.get_event();
      }

      if (is_shared) {
        messageRef(msg);
      }

      info.getTree()->foreachChild([&](NodeType child){
        bool const& send = child != dest;

        debug_print(
          broadcast, node,
          "GroupManager::sendGroupCollective *send* size={}, from={}, child={}, "
          "send={}, msg={}\n",
          size, from, child, send, print_ptr(msg)
        );

        if (send) {
          messageRef(msg);
          auto const put_event = theMsg()->sendMsgBytesWithPut(
            child, base, size, send_tag, action
          );
          if (has_action) {
            parent->addEventToList(put_event);
          }
        }
      });

      /*
       *  Send message to the root node of the group
       */
      if (send_to_root) {
        messageRef(msg);
        auto const put_event = theMsg()->sendMsgBytesWithPut(
          root_node, base, size, send_tag, action
        );
        if (has_action) {
          parent->addEventToList(put_event);
        }
      }

      if (is_shared) {
        messageDeref(msg);
      }

      if (!first_send && this_node_dest) {
        *deliver = false;
      } else {
        *deliver = true;
      }

      return event;
    } else {
      *deliver = true;
      return no_event;
    }
  } else if (in_group && !group_ready) {
    messageRef(msg);
    local_collective_group_info_.find(group)->second->readyAction([=]{
      theGroup()->sendGroupCollective(base,from,size,is_root,action,deliver);
    });
    *deliver = true;
    return no_event;
  } else {
    auto const& root_node = info.getRoot();
    assert(!in_group && "Must not be in this group");
    /*
     *  Forward message to the root node of the group; currently, only nodes
     *  that are part of the group can be in the spanning tree. Thus, this node
     *  must forward.
     */
    messageRef(msg);
    auto const put_event = theMsg()->sendMsgBytesWithPut(
      root_node, base, size, send_tag, action
    );
    if (has_action) {
      parent->addEventToList(put_event);
    }
    if (is_shared) {
      messageDeref(msg);
    }
    /*
     *  Do not deliver on this node since it is not part of the group and will
     *  just forward to the root node.
     */
    *deliver = false;
    return put_event;
  }
}

EventType GroupManager::sendGroup(
  BaseMessage* base, NodeType const& from, MsgSizeType const& size,
  bool const is_root, ActionType action, bool* const deliver
) {
  auto const& this_node = theContext()->getNode();
  auto const& msg = reinterpret_cast<ShortMessage* const>(base);
  auto const& group = envelopeGetGroup(msg->env);
  auto const& dest = envelopeGetDest(msg->env);
  auto const& is_shared = isSharedMessage(msg);
  auto const& this_node_dest = dest == this_node;
  auto const& first_send = from == uninitialized_destination;

  debug_print(
    group, node,
    "GroupManager::sendGroup: group={}, is_root={}\n",
    group, is_root
  );

  auto const& group_node = GroupIDBuilder::getNode(group);
  auto const& group_static = GroupIDBuilder::isStatic(group);
  auto const& group_collective = GroupIDBuilder::isCollective(group);
  auto const& send_tag = static_cast<messaging::MPI_TagType>(
    messaging::MPITag::ActiveMsgTag
  );

  assert(
    !group_collective && "Collective groups are not supported"
  );

  auto send_to_node = [&](NodeType node) -> EventType {
    EventType event = no_event;
    bool const& has_action = action != nullptr;
    EventRecordType* parent = nullptr;
    auto const& send_tag = static_cast<messaging::MPI_TagType>(
      messaging::MPITag::ActiveMsgTag
    );

    if (has_action) {
      event = theEvent()->createParentEvent(this_node);
      auto& holder = theEvent()->getEventHolder(event);
      parent = holder.get_event();
    }

    return theMsg()->sendMsgBytesWithPut(node, base, size, send_tag, action);
  };

  EventType ret_event = no_event;

  if (is_root && group_node != this_node) {
    *deliver = false;
    return send_to_node(group_node);
  } else {
    auto iter = local_group_info_.find(group);
    bool is_at_root = iter != local_group_info_.end();
    if (is_at_root && iter->second->forward_node_ != this_node) {
      if (iter->second->forward_node_ != this_node) {
        auto& info = *iter->second;
        assert(info.is_forward_ && "Must be a forward");
        auto const& node = info.forward_node_;
        *deliver = false;
        return send_to_node(node);
      } else {
        *deliver = true;
        return no_event;
      }
    } else {
      auto iter = remote_group_info_.find(group);

      debug_print(
        broadcast, node,
        "GroupManager::sendGroup: *send* remote size={}, from={}, found={}, "
        "dest={}, group={:x}, is_root={} \n",
        size, from, iter != remote_group_info_.end(), dest, group,
        is_root
      );

      if (iter != remote_group_info_.end() && (!this_node_dest || first_send)) {
        auto& info = *iter->second;
        assert(!info.is_forward_ && "Must not be a forward");
        assert(
          info.default_spanning_tree_ != nullptr && "Must have spanning tree"
        );

        bool const& has_action = action != nullptr;
        EventRecordType* parent = nullptr;
        auto const& send_tag = static_cast<messaging::MPI_TagType>(
          messaging::MPITag::ActiveMsgTag
        );
        auto const& num_children = info.default_spanning_tree_->getNumChildren();

        if (has_action) {
          ret_event = theEvent()->createParentEvent(this_node);
          auto& holder = theEvent()->getEventHolder(ret_event);
          parent = holder.get_event();
        }

        if (is_shared) {
          messageRef(msg);
        }

        // Send to child nodes in the group's spanning tree
        if (num_children > 0) {
          info.default_spanning_tree_->foreachChild([&](NodeType child) {
            debug_print(
              broadcast, node,
              "GroupManager::sendGroup: *send* size={}, from={}, child={}\n",
              size, from, child
            );

            if (child != this_node) {
              messageRef(msg);
              auto const put_event = theMsg()->sendMsgBytesWithPut(
                child, base, size, send_tag, action
              );
              if (has_action) {
                parent->addEventToList(put_event);
              }
            }
          });
        }

        if (is_shared) {
          messageDeref(msg);
        }

        if (is_root && from == uninitialized_destination) {
          *deliver = true;
        } else if (!is_root && dest == this_node) {
          *deliver = false;
        } else {
          *deliver = true;
        }
      }
    }
  }

  return ret_event;
}

GroupManager::GroupManager() {
  global::DefaultGroup::setupDefaultTree();
}

}} /* end namespace vt::group */
