#include "event_loop.hh"

#include <sys/eventfd.h>
#include <unistd.h>

#include <cerrno>
#include <vector>

#include "logger.hh"
#include "selector.hh"
#include "timer.hh"
#include "timer_queue.hh"
#include "timestamp.hh"

EventLoop::EventLoop()
    : m_terminate(false), mp_selector(new Selector),
      mp_timer_queue(new TimerQueue(this)) {
  create_eventfd();
  mp_wakeup_channel = new Channel(this, m_eventfd);
  mp_wakeup_channel->set_callback(this);
  mp_wakeup_channel->enable_read();
}

auto EventLoop::loop() -> void {
  while (!m_terminate) {
    mp_selector->poll(m_channels);
    for (auto p_channel : m_channels) {
      p_channel->handle_event();
    }
    m_channels.clear();
    invoke_pending_runnables();
  }
}

auto EventLoop::queue_loop(Runnable *p_runnable, void *p_param) -> void {
  m_runnables.emplace_back(p_runnable, p_param);
  wakeup();
}

auto EventLoop::handle_read() -> void {
  uint64_t one = 1;
  ssize_t n = ::read(m_eventfd, &one, sizeof(one));
  if (n != sizeof(one)) {
    error("Fail to read signal...\n");
  }
}

auto EventLoop::handle_write() -> void {}

auto EventLoop::update(Channel *channel) -> void {
  mp_selector->update(channel);
}

auto EventLoop::wakeup() -> void {
  uint64_t one = 1;
  ssize_t n = ::write(m_eventfd, &one, sizeof(one));
  if (n != sizeof(one)) {
    error("Fail to send wakeup signal...\n");
  }
}

auto EventLoop::create_eventfd() -> void {
  m_eventfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (m_eventfd < 0) {
    error("error in eventfd(), errno: %d\n", errno);
  }
}

auto EventLoop::invoke_pending_runnables() -> void {
  std::vector<Runner> p_runnables_tmp;
  p_runnables_tmp.swap(m_runnables);
  for (auto &p_runnable : p_runnables_tmp) {
    p_runnable.run();
  }
}

auto EventLoop::run_at(Timestamp when, Runnable *p_runnable) -> void * {
  return mp_timer_queue->set_timer(new Timer(when, p_runnable, 0.0));
}

auto EventLoop::run_after(double delay, Runnable *p_runnable) -> void * {
  return mp_timer_queue->set_timer(
      new Timer(Timestamp::now_after(delay), p_runnable, 0.0));
}

auto EventLoop::run_every(double interval, Runnable *p_runnable) -> void * {
  return mp_timer_queue->set_timer(
      new Timer(Timestamp::now_after(interval), p_runnable, interval));
}

auto EventLoop::cancel_timer(void *timer_id) -> void {
  mp_timer_queue->cancel_timer(timer_id);
}