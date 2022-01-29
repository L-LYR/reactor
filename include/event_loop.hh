#ifndef MINI_MUDUO_EVENT_LOOP
#define MINI_MUDUO_EVENT_LOOP

#include <vector>

#include "callback.hh"
#include "channel.hh"
#include "decl.hh"
#include "runnable.hh"
#include "runner.hh"
#include "timer_queue.hh"

class EventLoop : public ChannelCallback {
public:
  EventLoop();
  ~EventLoop();
  auto loop() -> void;
  auto stop() -> void;
  auto update(Channel *p_channel) -> void;
  auto queue_loop(Runnable *p_runnable, void *p_param) -> void;
  auto run_at(Timestamp when, Runnable *p_runnable) -> void *;
  auto run_after(double delay, Runnable *p_runnable) -> void *;
  auto run_every(double interval, Runnable *p_runnable) -> void *;
  auto cancel_timer(void *timer_id) -> void;

  virtual auto handle_read() -> void;
  virtual auto handle_write() -> void;

private:
  auto wakeup() -> void;
  auto create_eventfd() -> void;
  auto invoke_pending_runnables() -> void;

  bool m_terminate;
  Selector *mp_selector; // own

  int m_eventfd;
  Channel *mp_wakeup_channel; // own

  TimerQueue *mp_timer_queue; // own

  std::vector<Runner> m_runnables;
  std::vector<Channel *> m_channels;
};

#endif