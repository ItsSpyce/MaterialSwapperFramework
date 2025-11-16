// from
// https://github.com/Acro748/MuDynamicNormalMap/blob/master/include/ThreadPool.h
#pragma once

#include <future>

class ThreadPool {
  using Lock = mutex;
  using Locker = unique_lock;

 public:
  ThreadPool() = delete;
  ThreadPool(u32 threadSize) {
    u32 coreCount = max(1u, threadSize);
    for (u32 i = 0; i < coreCount; i++) {
      workers_.emplace_back([this] { run(); });
    }
  }
  ~ThreadPool() {
    stop_.store(true);
    condition_.notify_all();
    for (auto& worker : workers_) {
      if (worker.joinable()) {
        worker.join();
      }
    }
  }

  template <typename F, typename... Args>
  future<invoke_result_t<F, Args...>> enqueue(F&& f, Args&&... args) {
    using return_type = invoke_result_t<F, Args...>;
    auto task = make_shared<packaged_task<return_type()>>(
        bind(forward<F>(f), forward<Args>(args)...));
    auto result = task->get_future();
    {
      Locker lock(queueMutex_);
      tasks_.emplace([task] { (*task)(); });
    }
    condition_.notify_one();
    return result;
  }

  size_t count() {
    Locker lock(queueMutex_);
    return tasks_.size();
  }

 private:
  vector<thread> workers_;
  queue<function<void()>> tasks_;
  Lock queueMutex_;
  condition_variable condition_;
  atomic<bool> stop_{false};

  void run() {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    while (true) {
      function<void()> task;
      {
        unique_lock lock(queueMutex_);
        condition_.wait(lock, [this] {
          return stop_.load() || !tasks_.empty();
        });
        if (stop_.load() && tasks_.empty()) {
          return;
        }
        task = move(tasks_.front());
        tasks_.pop();
      }
      task();
    }
  }
};