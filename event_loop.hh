#ifndef MINI_MUDUO_EVENT_LOOP
#define MINI_MUDUO_EVENT_LOOP

#include <vector>

#include "./callback.hh"
#include "./channel.hh"
#include "./decl.hh"
#include "./runnable.hh"

class EventLoop : public ChannelCallback {
  public:
    EventLoop();
    ~EventLoop() = default;
    auto run() -> void;
    auto update(Channel* channel) -> void;
    auto queue_loop(Runnable* runnable) -> void;

    virtual auto handle_read() -> void;
    virtual auto handle_write() -> void;

  private:
    auto wakeup() -> void;
    auto create_eventfd() -> void;
    auto invoke_pending_runnables() -> void;

    bool m_terminate;
    Selector* m_selector;

    int m_eventfd;
    Channel* mp_wakeup_channel;

    std::vector<Runnable*> m_runnables;
    std::vector<Channel*> m_channels;
};

#endif