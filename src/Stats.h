#pragma once

#include "Core/RollingList.h"

class Stats : public Singleton<Stats> {
  using Clock = chrono::high_resolution_clock;

  struct TimeStats {
    Core::RollingList<long long, 64> history;
    long long meanTime;
    unsigned int totalCalls;
  };
public:
  void LogTime(const string& identifier, const std::function<void()>& func) {
    auto start = Clock::now();
    func();
    auto end = Clock::now();
    auto duration = (end - start).count();
    lock_guard guard(lock_);
    auto [stats, _] = timeStats_.try_emplace(identifier, TimeStats{});
    stats->second.history.push_back(duration);
    ++stats->second.totalCalls;
    
  }

private:
  mutex lock_;
  map<string, TimeStats> timeStats_;
};