
#include "config.h"
#include "pool/pool.h"
#include "worker/worker_headers.h"
#include "pool/static_sized/memory_pool_equal.h"

#include <cstdlib>
#include <cstdint>
#include <cassert>

namespace vt { namespace pool {

Pool::Pool()
  : small_msg(initSPool()), medium_msg(initMPool())
{ }

/*static*/Pool::MemPoolSType Pool::initSPool() {
  return std::make_unique<MemoryPoolType<memory_size_small>>();
}

/*static*/ Pool::MemPoolMType Pool::initMPool() {
  return std::make_unique<MemoryPoolType<memory_size_medium>>(64);
}

Pool::ePoolSize Pool::getPoolType(size_t const& num_bytes) {
  if (num_bytes <= small_msg->getNumBytes()) {
    return ePoolSize::Small;
  } else if (num_bytes <= medium_msg->getNumBytes()) {
    return ePoolSize::Medium;
  } else {
    return ePoolSize::Malloc;
  }
}

void* Pool::try_pooled_alloc(size_t const& num_bytes) {
  ePoolSize const pool_type = getPoolType(num_bytes);

  if (pool_type != ePoolSize::Malloc) {
    return pooled_alloc(num_bytes, pool_type);
  } else {
    return nullptr;
  }
}

bool Pool::try_pooled_dealloc(void* const buf) {
  auto buf_char = static_cast<char*>(buf);
  auto const& actual_alloc_size = HeaderManagerType::getHeaderBytes(buf_char);
  ePoolSize const pool_type = getPoolType(actual_alloc_size);

  if (pool_type != ePoolSize::Malloc) {
    pooled_dealloc(buf, pool_type);
    return true;
  } else {
    return false;
  }
}

void* Pool::pooled_alloc(size_t const& num_bytes, ePoolSize const pool_type) {
  auto const worker = theContext()->getWorker();
  bool const comm_thread = worker == worker_id_comm_thread;
  void* ret = nullptr;

  debug_print(
    pool, node,
    "Pool::pooled_alloc of size={}, type={}, ret={}, worker={}\n",
    num_bytes, print_pool_type(pool_type), ret, worker
  );

  if (pool_type == ePoolSize::Small) {
    auto pool = comm_thread ? small_msg.get() : s_msg_worker_[worker].get();
    assert(
      (comm_thread || s_msg_worker_.size() > worker) && "Must have worker pool"
    );
    ret = pool->alloc(num_bytes);
  } else if (pool_type == ePoolSize::Medium) {
    auto pool = comm_thread ? medium_msg.get() : m_msg_worker_[worker].get();
    assert(
      (comm_thread || m_msg_worker_.size() > worker) && "Must have worker pool"
    );
    ret = pool->alloc(num_bytes);
  } else {
    assert(0 && "Pool must be valid");
    ret = nullptr;
  }

  return ret;
}

void Pool::pooled_dealloc(void* const buf, ePoolSize const pool_type) {
  debug_print(
    pool, node,
    "Pool::pooled_dealloc of ptr={}, type={}\n",
    print_ptr(buf), print_pool_type(pool_type)
  );

  if (pool_type == ePoolSize::Small) {
    small_msg->dealloc(buf);
  } else if (pool_type == ePoolSize::Medium) {
    medium_msg->dealloc(buf);
  } else {
    assert(0 && "Pool must be valid");
  }
}

void* Pool::default_alloc(size_t const& num_bytes) {
  auto alloc_buf = std::malloc(num_bytes + sizeof(HeaderType));
  return HeaderManagerType::setHeader(num_bytes, static_cast<char*>(alloc_buf));
}

void Pool::default_dealloc(void* const ptr) {
  std::free(ptr);
}

void* Pool::alloc(size_t const& num_bytes) {
  void* ret = nullptr;

  #if backend_check_enabled(memory_pool)
    ret = try_pooled_alloc(num_bytes);
  #endif

  // Fall back to the default allocated if the pooled allocated fails to return
  // a valid pointer
  if (ret == nullptr) {
    ret = default_alloc(num_bytes);
  }

  debug_print(
    pool, node,
    "Pool::alloc of size={}, type={}, ret={}\n",
    num_bytes, print_pool_type(pool_type), ret
  );

  return ret;
}

void Pool::dealloc(void* const buf) {
  auto buf_char = static_cast<char*>(buf);
  auto const& actual_alloc_size = HeaderManagerType::getHeaderBytes(buf_char);
  auto const& alloc_worker = HeaderManagerType::getHeaderWorker(buf_char);
  auto const& ptr_actual = HeaderManagerType::getHeaderPtr(buf_char);
  auto const worker = theContext()->getWorker();

  ePoolSize const pool_type = getPoolType(actual_alloc_size);

  debug_print(
    pool, node,
    "Pool::dealloc of buf={}, type={}, alloc_size={}, worker={}, ptr={}\n",
    buf, print_pool_type(pool_type), actual_alloc_size, alloc_worker,
    print_ptr(ptr_actual)
  );

  if (pool_type != ePoolSize::Malloc && alloc_worker != worker) {
    theWorkerGrp()->enqueueForWorker(worker, [buf]{
      thePool()->dealloc(buf);
    });
    return;
  }

  bool success = false;

  #if backend_check_enabled(memory_pool)
    success = try_pooled_dealloc(buf);
  #endif

  if (!success) {
    default_dealloc(ptr_actual);
  }
};

Pool::SizeType Pool::remainingSize(void* const buf) {
  #if backend_check_enabled(memory_pool)
    auto buf_char = static_cast<char*>(buf);
    auto const& actual_alloc_size = HeaderManagerType::getHeaderBytes(buf_char);

    ePoolSize const pool_type = getPoolType(actual_alloc_size);

    if (pool_type == ePoolSize::Small) {
      return small_msg->getNumBytes() - actual_alloc_size;
    } else if (pool_type == ePoolSize::Medium) {
      return medium_msg->getNumBytes() - actual_alloc_size;
    } else {
      return 0;
    }
  #else
    return 0;
  #endif
}

void Pool::initWorkerPools(WorkerCountType const& num_workers) {
  #if backend_check_enabled(memory_pool)
    for (auto i = 0; i < num_workers; i++) {
      s_msg_worker_.emplace_back(initSPool());
      m_msg_worker_.emplace_back(initMPool());
    }
  #endif
}

void Pool::destroyWorkerPools() {
  #if backend_check_enabled(memory_pool)
    s_msg_worker_.clear();
    m_msg_worker_.clear();
  #endif
}

bool Pool::active() const {
  return backend_check_enabled(memory_pool);
}

 bool Pool::active_env() const {
  return backend_check_enabled(memory_pool) &&
    !backend_check_enabled(no_pool_alloc_env);
}

}} //end namespace vt::pool
