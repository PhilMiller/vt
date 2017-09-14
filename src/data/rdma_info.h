
#if ! defined __RUNTIME_TRANSPORT_RDMA_INFO__
#define __RUNTIME_TRANSPORT_RDMA_INFO__

#include "config.h"
#include "rdma_common.h"

#include <unordered_map>
#include <vector>

namespace vt { namespace rdma {

struct Info {
  using RDMA_TypeType = Type;

  RDMA_TypeType rdma_type;
  ByteType num_bytes = no_byte;
  TagType tag = no_tag;
  RDMA_PtrType data_ptr = no_rdma_ptr;
  RDMA_ContinuationType cont = no_action;
  ActionType cont_action = no_action;
  ByteType offset = no_byte;

  Info(
    RDMA_TypeType const& in_rdma_type, ByteType const& in_num_bytes = no_byte,
    ByteType const& in_offset = no_byte, TagType const& in_tag = no_tag,
    RDMA_ContinuationType in_cont = no_action, ActionType in_cont_action = no_action,
    RDMA_PtrType const& in_data_ptr = no_rdma_ptr
  ) : rdma_type(in_rdma_type), num_bytes(in_num_bytes), tag(in_tag),
      data_ptr(in_data_ptr), cont(in_cont), cont_action(in_cont_action),
      offset(in_offset)
  { }
};

}} //end namespace vt::rdma

#endif /*__RUNTIME_TRANSPORT_RDMA_INFO__*/
