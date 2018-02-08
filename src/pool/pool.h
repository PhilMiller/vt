
#if !defined INCLUDED_POOL_POOL_H
#define INCLUDED_POOL_POOL_H

#include "config.h"
#include "pool/static_sized/memory_pool_equal.h"
#include "pool/header/pool_header.h"

#include <vector>
#include <cstdint>
#include <cassert>
#include <memory>

namespace vt { namespace pool {

struct Pool {
  using SizeType = size_t;
  using HeaderType = Header;
  using HeaderManagerType = HeaderManager;
  template <int64_t num_bytes_t>
  using MemoryPoolType = MemoryPoolEqual<num_bytes_t>;
  template <int64_t num_bytes_t>
  using MemoryPoolPtrType = std::unique_ptr<MemoryPoolType<num_bytes_t>>;

  enum struct ePoolSize {
    Small = 1,
    Medium = 2,
    Large = 3,
    Malloc = 4
  };

  Pool();

  void* alloc(size_t const& num_bytes);
  void dealloc(void* const buf);
  ePoolSize getPoolType(size_t const& num_bytes);
  SizeType remainingSize(void* const buf);

  void initWorkerPools(WorkerCountType const& num_workers);
  void destroyWorkerPools();

private:
  using MemPoolSType = MemoryPoolPtrType<memory_size_small>;
  using MemPoolMType = MemoryPoolPtrType<memory_size_medium>;

  static MemPoolSType initSPool();
  static MemPoolMType initMPool();

private:
  MemPoolSType small_msg = nullptr;
  MemPoolMType medium_msg = nullptr;

  std::vector<MemPoolSType> s_msg_worker_;
  std::vector<MemPoolMType> m_msg_worker_;
};

}} //end namespace vt::pool

namespace vt {

extern pool::Pool* thePool();

} //end namespace vt

#endif /*INCLUDED_POOL_POOL_H*/
