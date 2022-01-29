#ifndef MINI_MUDUO_TIMER
#define MINI_MUDUO_TIMER

#include <chrono>

#include "decl.hh"
#include "runnable.hh"
#include "timestamp.hh"

class Timer {
public:
  Timer(Timestamp ts, Runnable *p_runnable, double seconds)
      : m_ts(ts), m_id(ts), mp_runnable(p_runnable),
        m_second_interval(seconds) {}
  ~Timer() = default;

  auto get_timestamp() -> Timestamp { return m_ts; }

  auto get_id() -> Timestamp { return m_id; }

  auto run() -> void { mp_runnable->run(this); }

  auto is_repeat() -> bool { return m_second_interval > 0.0; }

  auto move_to_next() -> void {
    m_ts = Timestamp::now_after(m_second_interval);
  }

private:
  Timestamp m_ts;
  Timestamp m_id;
  Runnable *mp_runnable;
  double m_second_interval;
};

#endif