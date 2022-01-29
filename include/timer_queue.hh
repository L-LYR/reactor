#ifndef MINI_MUDUO_TIME_QUEUE
#define MINI_MUDUO_TIME_QUEUE

#include <set>
#include <utility>
#include <vector>

#include "callback.hh"
#include "decl.hh"
#include "runnable.hh"
#include "timestamp.hh"

class TimerQueue : public ChannelCallback {
public:
  TimerQueue(EventLoop *p_loop);
  ~TimerQueue();

  auto add_timer(void *param) -> void;
  auto remove_timer(void *param) -> void;

  auto set_timer(Timer *p_timer) -> void *;
  auto cancel_timer(void *timer_id) -> void;

  virtual auto handle_read() -> void;
  virtual auto handle_write() -> void;

private:
  using TimerListEntry = std::pair<Timestamp, Timer *>;
  using TimerList = std::set<TimerListEntry>; // sorted and 1:n one time point
                                              // for many timers.

  auto create_timerfd() -> void;
  auto reset_timerfd(const Timestamp &ts) -> void;
  auto read_timerfd(const Timestamp &now) -> void;

  auto get_expired_timers(const Timestamp &now) -> std::vector<TimerListEntry>;
  auto reset_repeat_timers(const std::vector<TimerListEntry> &expired,
                           const Timestamp &now) -> void;

  auto insert(Timer *p_timer) -> bool;
  auto remove(const TimerListEntry &entry) -> void;

  int m_timerfd;
  TimerList m_timer_list;

  EventLoop *mp_loop;
  Channel *mp_timerfd_channel;

  AddTimerWrapper *mp_add_timer_wrapper;
  RemoveTimerWrapper *mp_remove_timer_wrapper;
};

#endif