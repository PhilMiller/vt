
#if !defined INCLUDED_TYPES_SENTINELS
#define INCLUDED_TYPES_SENTINELS

#include "configs/debug/debug_masterconfig.h"
#include "configs/types/types_type.h"
#include "configs/types/types_rdma.h"

namespace vt {

// Physical identifier sentinel values (nodes, cores, workers, etc.)
static constexpr NodeType const uninitialized_destination          = 0xFFFF;
static constexpr WorkerCountType const no_workers                  = 0xFFFF;
static constexpr WorkerIDType const no_worker_id                   = 0xFFFE;
static constexpr WorkerIDType const worker_id_comm_thread          = 0xFEED;
static constexpr WorkerIDType const comm_debug_print               = -1;

// Runtime default `empty' sentinel value
static constexpr uint64_t const u64empty = 0xFFFFFFFFFFFFFFFF;
static constexpr uint32_t const u32empty = 0xFEEDFEED;
static constexpr int64_t  const s64empty = 0xFFFFFFFFFFFFFFFF;
static constexpr int32_t  const s32empty = 0xFEEDFEED;

// Runtime identifier sentinel values
static constexpr int const num_check_actions                       = 8;
static constexpr EpochType const no_epoch                          = -1;
static constexpr TagType const no_tag                              = -1;
static constexpr EventType const no_event                          = -1;
static constexpr BarrierType const no_barrier                      = -1;
static constexpr RDMA_HandleType const no_rdma_handle              = -1;
static constexpr ByteType const no_byte                            = -1;
static constexpr ByteType const no_offset                          = -1;
static constexpr auto no_action                                    = nullptr;
static constexpr RDMA_PtrType const no_rdma_ptr                    = nullptr;
static constexpr VirtualProxyType const no_vrt_proxy               = u64empty;
static constexpr HandlerType const uninitialized_handler           = -1;
static constexpr RDMA_HandlerType const uninitialized_rdma_handler = -1;
static constexpr RefType const not_shared_message                  = -1000;
static constexpr RDMA_ElmType const no_rdma_elm                    = -1;
static constexpr RDMA_BlockType const no_rdma_block                = -1;
static constexpr SeedType const no_seed                            = -1;
static constexpr VirtualElmCountType const no_elms                 = -1;
static constexpr TagType const local_rdma_op_tag                   = s32empty;
static constexpr GroupType const no_group                          = u64empty;
static constexpr GroupType const default_group                     = 0xFFFFFFFF;
static constexpr PhaseType const fst_lb_phase                      = 0;
static constexpr PipeType const no_pipe                            = u64empty;

}  // end namespace vt

#endif  /*INCLUDED_TYPES_SENTINELS*/
