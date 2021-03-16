#ifndef MINI_MUDUO_EVENT_LOOP
#define MINI_MUDUO_EVENT_LOOP

#include "./decl.hh"

class EventLoop {
  public:
    EventLoop();
    ~EventLoop() = default;
    auto run() -> void;
    auto update(Channel* channel) -> void;

  private:
    bool m_terminate;
    Selector* m_selector;
};

#endif