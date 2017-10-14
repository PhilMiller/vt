
#if !defined __RUNTIME_TRANSPORT_SERIALIZED_DATA_MSG__
#define __RUNTIME_TRANSPORT_SERIALIZED_DATA_MSG__

#include "config.h"
#include "messaging/message.h"
#include "serialization/serialization.h"

using namespace ::serialization::interface;

namespace vt { namespace serialization {

static constexpr SizeType const serialized_msg_eager_size = 128;

struct NoneVrt { };

template <typename T>
struct SerializedDataMsg : ShortMessage {
  SerializedDataMsg() : ShortMessage() { }

  HandlerType handler = uninitialized_handler;
  TagType data_recv_tag = no_tag;
  NodeType from_node = uninitialized_destination;
};

using NumBytesType = int64_t;

template <typename UserMsgT, typename MessageT, NumBytesType num_bytes>
struct SerialPayloadMsg : MessageT {
  std::array<SerialByteType, num_bytes> payload{};
  NumBytesType bytes = 0;

  SerialPayloadMsg() : MessageT() { }

  explicit SerialPayloadMsg(NumBytesType const& in_bytes)
    : MessageT(), bytes(in_bytes)
  { }

  explicit SerialPayloadMsg(
    NumBytesType const& in_bytes, std::array<ByteType, num_bytes>&& arr
  ) : MessageT(), payload(std::forward<std::array<ByteType, num_bytes>>(arr)),
      bytes(in_bytes)
  { }
};

template <typename UserMsgT>
using SerialEagerPayloadMsg = SerialPayloadMsg<
  UserMsgT, SerializedDataMsg<UserMsgT>, serialized_msg_eager_size
>;

}} /* end namespace vt::serialization */

#endif /*__RUNTIME_TRANSPORT_SERIALIZED_DATA_MSG__*/
