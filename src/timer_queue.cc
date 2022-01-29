#include "timer_queue.hh"

#include <errno.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cstring>

#include "event_loop.hh"
#include "logger.hh"
#include "timer.hh"
#include "timer_wrapper.hh"

TimerQueue::TimerQueue(EventLoop *p_loop)
    : mp_loop(p_loop), mp_add_timer_wrapper(new AddTimerWrapper(this)),
      mp_remove_timer_wrapper(new RemoveTimerWrapper(this)) {
  create_timerfd();
  mp_timerfd_channel = new Channel(p_loop, m_timerfd);
  mp_timerfd_channel->set_callback(this);
  mp_timerfd_channel->enable_read();
}

TimerQueue::~TimerQueue() { ::close(m_timerfd); }

auto TimerQueue::add_timer(void *param) -> void {
  Timer *newp_timer = static_cast<Timer *>(param);
  if (insert(newp_timer)) {
    // earliest timer changed
    reset_timerfd(newp_timer->get_timestamp());
  }
}

auto TimerQueue::remove_timer(void *param) -> void {
  Timer *tarp_timer = static_cast<Timer *>(param);
  TimerListEntry target(tarp_timer->get_id(), tarp_timer);
  remove(target);
}

auto TimerQueue::set_timer(Timer *p_timer) -> void * {
  mp_loop->queue_loop(mp_add_timer_wrapper, p_timer);
  return p_timer;
}

auto TimerQueue::cancel_timer(void *timer_id) -> void {
  mp_loop->queue_loop(mp_remove_timer_wrapper, timer_id);
}

auto TimerQueue::handle_read() -> void {
  Timestamp now(Timestamp::now());
  read_timerfd(now);
  auto expired = get_expired_timers(now);

  for (auto &t : expired) {
    t.second->run();
  }
  reset_repeat_timers(expired, now);
}

auto TimerQueue::handle_write() -> void {}

auto TimerQueue::create_timerfd() -> void {
  m_timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (m_timerfd < 0) {
    error("error in timerfd_create(), errno: %d\n", errno);
  }
}

auto TimerQueue::reset_timerfd(const Timestamp &ts) -> void {
  itimerspec new_timerspec;
  std::memset(&new_timerspec, 0, sizeof(new_timerspec));
  auto microseconds = Timestamp::microseconds_from_now_to(ts);
  if (microseconds.count() < 100) {
    microseconds = std::chrono::microseconds(100);
  }

  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(microseconds);
  auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(
      microseconds - seconds);

  new_timerspec.it_value.tv_sec = seconds.count();
  new_timerspec.it_value.tv_nsec = nanoseconds.count();
  if (::timerfd_settime(m_timerfd, 0, &new_timerspec, NULL) < 0) {
    error("error in timerfd_settime(), errno: %d\n", errno);
  }
}

auto TimerQueue::reset_repeat_timers(const std::vector<TimerListEntry> &expired,
                                     const Timestamp &now) -> void {
  for (const auto &t : expired) {
    if (t.second->is_repeat()) {
      t.second->move_to_next();
      insert(t.second);
    }
  }
  if (!m_timer_list.empty()) {
    auto new_expire_lim = m_timer_list.begin()->second->get_timestamp();
    if (new_expire_lim.is_valid()) {
      reset_timerfd(new_expire_lim);
    }
  }
}

auto TimerQueue::read_timerfd(const Timestamp &now) -> void {
  uint64_t timer_signal;
  ssize_t ret_n = ::read(m_timerfd, &timer_signal, sizeof(timer_signal));
  if (ret_n != sizeof(timer_signal)) {
    error("error in read(), errno: %d\n", errno);
  }
}

static Timer *timer_ptr_for_find = reinterpret_cast<Timer *>(~0);
auto TimerQueue::get_expired_timers(const Timestamp &now)
    -> std::vector<TimerListEntry> {
  std::vector<TimerListEntry> expired_timers;
  auto it = std::lower_bound(m_timer_list.begin(), m_timer_list.end(),
                             TimerListEntry(now, timer_ptr_for_find));
  std::move(m_timer_list.begin(), it, std::back_inserter(expired_timers));
  m_timer_list.erase(m_timer_list.begin(), it);
  return expired_timers;
}

auto TimerQueue::insert(Timer *p_timer) -> bool {
  bool earliest_timer_changed = false;
  Timestamp when = p_timer->get_timestamp();
  if (m_timer_list.empty() || when < m_timer_list.begin()->first) {
    earliest_timer_changed = true;
  }
  auto ret = m_timer_list.insert(TimerListEntry(p_timer->get_id(), p_timer));
  if (!ret.second) {
    error("error in insert()\n");
  }
  return earliest_timer_changed;
}

auto TimerQueue::remove(const TimerListEntry &entry) -> void {
  auto it = m_timer_list.find(entry);
  if (it != m_timer_list.end()) {
    m_timer_list.erase(it);
  } else {
    error("remove an inexistent timer\n");
  }
}