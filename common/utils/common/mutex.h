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

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>

namespace utils {

// Enable thread safety attributes only with clang.
// The attributes can be safely erased when compiling with other compilers.
#if defined(__SUPPORT_TS_ANNOTATION__) || defined(__clang__)
#define THREAD_ANNOTATION_ATTRIBUTE__(x) __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x)  // no-op
#endif

#define CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(capability(x))

#define SCOPED_CAPABILITY THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)

#define GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

#define PT_GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))

#define REQUIRES(...) THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))

#define ACQUIRE(...) THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))

#define RELEASE(...) THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

#define EXCLUDES(...) THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

#define NO_THREAD_SAFETY_ANALYSIS THREAD_ANNOTATION_ATTRIBUTE__(no_thread_safety_analysis)

// Defines an annotated mutex that can only be locked through its scoped locker
// implementation.
class CAPABILITY("mutex") Mutex {
 public:
#ifdef PONY_MUTEX_BENCHMARK
  Mutex(std::string name = std::string()) : name_(std::move(name)) {}
  ~Mutex() = default;

  void set_name(std::string name) { name_ = std::move(name); }

#else
  Mutex(std::string /*name*/ = std::string()) {}
  void set_name(std::string /*name*/) {}
#endif  // PONY_MUTEX_BENCHMARK

  // An RAII class that acquires a mutex in its constructor, and
  // releases it in its destructor. It also implements waiting functionality on
  // conditions that get checked whenever the mutex is released.
  class SCOPED_CAPABILITY Locker {
   public:
#ifdef PONY_MUTEX_BENCHMARK
    explicit Locker(Mutex* mutex) ACQUIRE(mutex) : mutex_(mutex) {
      const auto start_time = std::chrono::system_clock::now();
      lock_ = std::unique_lock<std::mutex>(mutex->mutex_);
      const auto end_time = std::chrono::system_clock::now();
      const double time_cost = std::chrono::duration<double>(end_time - start_time).count();
      if (time_cost > 0.01) {
        LOG(ERROR) << "Mutex [" << mutex_->name_ << "] took " << time_cost << " seconds!";
      }

      mutex_->max_time_cost_ = std::max(mutex_->max_time_cost_, time_cost);
      LOG_EVERY_N(ERROR, 10000) << "Max mutex wait cost [" << mutex_->name_
                                << "]: " << mutex_->max_time_cost_ << " seconds.";
    }
#else
    explicit Locker(Mutex* mutex) ACQUIRE(mutex) : mutex_(mutex), lock_(mutex->mutex_) {}
#endif  // PONY_MUTEX_BENCHMARK

    ~Locker() RELEASE() {
      lock_.unlock();
      mutex_->condition_.notify_all();
    }

    // clang-3.8 does not support pointer alias analysis, so REQUIRES() attribute
    // on this function is removed.
    template <typename Predicate>
    void Await(Predicate predicate) {
      mutex_->condition_.wait(lock_, predicate);
    }

    // clang-3.8 does not support pointer alias analysis, so REQUIRES() attribute
    // on this function is removed.
    template <typename Predicate>
    bool AwaitWithTimeout(Predicate predicate,
                          std::chrono::duration<int64_t, std::ratio<1, 10000000>> timeout) {
      return mutex_->condition_.wait_for(lock_, timeout, predicate);
    }

   private:
    Mutex* mutex_;
    std::unique_lock<std::mutex> lock_;
  };

 private:
  std::condition_variable condition_;
  std::mutex mutex_;

#ifdef PONY_MUTEX_BENCHMARK
  std::string name_;
  double max_time_cost_ = 0.0;
#endif  // PONY_MUTEX_BENCHMARK
};

using MutexLocker = Mutex::Locker;

}  // namespace utils
