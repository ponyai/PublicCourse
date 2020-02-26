// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <functional>
#include <unistd.h>

#include "common/utils/containers/fixed_array.h"

namespace parallelism {

// Parallelizes a number of tasks with multiple worker processes. The total number of workers would
// be same as the number of tasks. However, at most 'max_process_num' workers would be alive at the
// same time. When there are already 'max_process_num' workers running, if StartWorker() is called,
// it will wait until a worker ends.
class MultiProcessParallelizer {
 public:
  explicit MultiProcessParallelizer(int max_process_num);
  ~MultiProcessParallelizer() = default;

  void StartWorker(std::function<void()> func, const std::string& name = std::string());
  void WaitForAllWorkers();
  void KillAllWorkers();

 private:
  void WaitForOneWorker();

  struct Worker {
    pid_t pid = 0;
    std::string name;
  };

  utils::FixedArray<Worker> workers_;
  int num_workers_ = 0;
};

}  // namespace parallelism
