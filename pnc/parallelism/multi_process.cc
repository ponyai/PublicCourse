// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/parallelism/multi_process.h"

#include <sys/wait.h>

#include <algorithm>
#include <glog/logging.h>

namespace parallelism {

MultiProcessParallelizer::MultiProcessParallelizer(int max_process_num)
    : workers_(max_process_num) {
  CHECK_GT(max_process_num, 0);
}

void MultiProcessParallelizer::StartWorker(std::function<void()> func, const std::string& name) {
  if (num_workers_ == workers_.size()) {
    WaitForOneWorker();
  }

  pid_t pid = fork();
  if (pid == 0) {
    // Child.
    func();
    exit(0);
  }

  // Parent.
  Worker& worker = workers_[num_workers_];
  worker.pid = pid;
  worker.name = name;
  ++num_workers_;
}

void MultiProcessParallelizer::WaitForOneWorker() {
  CHECK_GT(num_workers_, 0);
  int status = 0;
  pid_t finished_child_pid = ::wait(&status);
  // Make sure the child process exits normally.
  for (int i = 0; i < num_workers_; ++i) {
    if (finished_child_pid == workers_[i].pid) {
      if (i + 1 < num_workers_) {
        using std::swap;
        swap(workers_[i], workers_[num_workers_ - 1]);
      }

      if (!WIFEXITED(status)) {
        const std::string& worker_name = workers_[num_workers_ - 1].name;
        --num_workers_;
        LOG(ERROR) << "Worker named '" << worker_name << "' terminated abnormally.";
      }
      break;
    }
  }

  --num_workers_;
}

void MultiProcessParallelizer::WaitForAllWorkers() {
  while (num_workers_ > 0) {
    WaitForOneWorker();
  }
}

void MultiProcessParallelizer::KillAllWorkers() {
  for (int i = 0; i < num_workers_; ++i) {
    const int ret = ::kill(workers_[i].pid, SIGKILL);

    // If the worker has already terminates, kill() will fail. In this case, we should wait() on it.
    if (ret == ESRCH) {
      int status;
      ::waitpid(workers_[i].pid, &status, 0);
    }
  }
}

}  // namespace parallelism
