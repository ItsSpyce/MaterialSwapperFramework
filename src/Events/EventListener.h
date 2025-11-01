#pragma once

template <class Event = void>
class EventListener {
 public:
  virtual void OnEvent(const Event& event) = 0;
};

template <class Event = void>
class EventSource {
 public:
  void AddListener(EventListener<Event>* listener) {
    listeners_.push_back(listener);
  }
  void RemoveListener(EventListener<Event>* listener) {
    listeners_.erase(
        std::remove(listeners_.begin(), listeners_.end(), listener),
        listeners_.end());
  }
  void Dispatch(const Event& event) {
    for (auto* listener : listeners_) {
      listener->OnEvent(event);
    }
  }

 private:
  std::vector<EventListener<Event>*> listeners_;
};