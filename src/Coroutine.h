#pragma once

#include "Events/EventListener.h"
#include "Options.h"

class CoroutineManager : public Singleton<CoroutineManager>,
                         public EventListener<FrameEvent> {
 public:
  void Initialize() {
    maxOpsPerFrame_ = Options::GetSingleton()->GetMaxCoroutineOpsPerFrame();
  }

  void OnEvent(const FrameEvent& event) override;
  void RunCoroutine(Coroutine& coroutine);

 private:
  i32 maxOpsPerFrame_{0};
};

class Coroutine {
  using CoroutineFn = Visitor<>;
  explicit Coroutine(CoroutineFn& fn) : fn_(fn) {}

 public:
  Coroutine() = delete;
  Coroutine(Coroutine&&) = delete;

  static void Run(CoroutineFn& fn) {
    Coroutine coroutine(fn);
    CoroutineManager::GetSingleton()->RunCoroutine(coroutine);
  }

 private:
  CoroutineFn& fn_;
};

inline void CoroutineManager::OnEvent(const FrameEvent& event) {}