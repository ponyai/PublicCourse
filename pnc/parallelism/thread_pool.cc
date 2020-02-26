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

#include "pnc/parallelism/thread_pool.h"

namespace parallelism {

using utils::Mutex;
using utils::MutexLocker;

constexpr int ThreadPool::kSleepTimeInMilliseconds;

ThreadPool::ThreadPool(int num_threads) {
  CHECK_GT(num_threads, 0);
  MutexLocker locker(&mutex_);
  for (int i = 0; i < num_threads; ++i) {
    pool_.emplace_back([this]() { DoWork(); });
  }
}

ThreadPool::~ThreadPool() {
  {
    MutexLocker locker(&mutex_);
    CHECK(work_queue_.empty() && running_work_items_ == 0)
        << "There are still unfinished work items. Please call either WaitForWorkItemsDone() or "
           "Shutdown() before destroying the thread pool!";
  }
  Shutdown();
}

void ThreadPool::Shutdown() {
  {
    MutexLocker locker(&mutex_);
    if (!running_) {
      return;
    }
    running_ = false;
    work_queue_.clear();
  }

  for (std::thread& thread : pool_) {
    thread.join();
  }

  {
    MutexLocker locker(&mutex_);
    running_work_items_ = 0;
  }
}

void ThreadPool::Schedule(std::function<void()> work_item) {
  MutexLocker locker(&mutex_);
  CHECK(running_);
  work_queue_.push_back(std::move(work_item));
  running_work_items_++;
}

int ThreadPool::GetWorkQueueSize() {
  MutexLocker locker(&mutex_);
  return work_queue_.size();
}

int ThreadPool::GetRunningWorkerNum() {
  MutexLocker locker(&mutex_);
  return running_work_items_;
}

void ThreadPool::WaitForWorkItemsDone() {
  MutexLocker locker(&mutex_);
  locker.Await([this]()
                   REQUIRES(mutex_) { return work_queue_.empty() && running_work_items_ == 0; });
}

void ThreadPool::DoWork() {
  for (;;) {
    std::function<void()> work_item;
    {
      MutexLocker locker(&mutex_);
      locker.Await([this]() REQUIRES(mutex_) { return !work_queue_.empty() || !running_; });
      if (!running_) {
        running_work_items_--;
        return;
      }
      if (!work_queue_.empty()) {
        work_item = work_queue_.front();
        work_queue_.pop_front();
      }
    }
    CHECK(work_item);
    work_item();

    {
      MutexLocker locker(&mutex_);
      running_work_items_--;
    }
  }
}

}  // namespace parallelism
