
#if !defined __RUNTIME_TRANSPORT_WORKER_GROUP__
#define __RUNTIME_TRANSPORT_WORKER_GROUP__

#include "config.h"
#include "worker/worker_common.h"
#include "worker/worker.h"
#include "worker/stdthread_worker.h"

#include <vector>
#include <memory>

namespace vt { namespace worker {

template <typename WorkerT>
struct WorkerGroupAny {
  using WorkerType = WorkerT;
  using WorkerPtrType = std::unique_ptr<WorkerT>;
  using WorkerContainerType = std::vector<WorkerPtrType>;

  WorkerGroupAny();
  WorkerGroupAny(WorkerCountType const& in_num_workers);

  virtual ~WorkerGroupAny();

  void initialize();
  void spawnWorkers();
  void joinWorkers();

  void enqueueAnyWorker(WorkUnitType const& work_unit);
  void enqueueForWorker(
    WorkerIDType const& worker_id, WorkUnitType const& work_unit
  );
  void enqueueAllWorkers(WorkUnitType const& work_unit);

private:
  bool initialized_ = false;
  WorkerCountType num_workers_ = 0;
  WorkerContainerType workers_;
};

using WorkerGroupStd = WorkerGroupAny<StdThreadWorker>;

}} /* end namespace vt::worker */

#if backend_check_enabled(detector)
  #include "worker/worker_group_traits.h"

  namespace vt { namespace worker {

  static_assert(
    WorkerGroupTraits<WorkerGroupStd>::is_worker,
    "WorkerGroupStd must follow the WorkerGroup concept"
  );

  }} /* end namespace vt::worker */
#endif

#include "worker/worker_group.impl.h"

#endif /*__RUNTIME_TRANSPORT_WORKER_GROUP__*/
