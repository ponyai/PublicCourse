/*
 * Copyright 2016 The Cartographer Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>
#include <deque>
#include <functional>
#include <numeric>
#include <thread>
#include <unistd.h>
#include <vector>

#include "glog/logging.h"
#include "common/utils/common/defines.h"
#include "common/utils/common/mutex.h"

namespace parallelism {

// A fixed number of threads working on a work queue of work items. Adding a
// new work item does not block, and will be executed by a background thread
// eventually. The queue must be empty before calling the destructor. The thread
// pool will then wait for the currently executing work items to finish and then
// destroy the threads.
class ThreadPool {
 public:
  explicit ThreadPool(int num_threads);

  ~ThreadPool();

  void Schedule(std::function<void()> work_item);

  int GetWorkQueueSize();

  int GetRunningWorkerNum();

  // Wait for all work items, no matter whether they have already started.
  void WaitForWorkItemsDone();

  // The already-started work items will not be terminated, but the pending ones will be cancelled.
  void Shutdown();

 private:
  static constexpr int kSleepTimeInMilliseconds = 10;

  void DoWork();

  std::vector<std::thread> pool_;

  utils::Mutex mutex_;
  bool running_ GUARDED_BY(mutex_) = true;
  std::deque<std::function<void()>> work_queue_ GUARDED_BY(mutex_);
  int running_work_items_ GUARDED_BY(mutex_) = 0;

  DISALLOW_COPY_MOVE_AND_ASSIGN(ThreadPool);
};

}  // namespace parallelism
