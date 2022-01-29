#ifndef MINI_MUDUO_TIMESTAMP
#define MINI_MUDUO_TIMESTAMP

#include <inttypes.h>

#include <chrono>
#include <cstdio>
#include <string>

class Timestamp {
public:
  using clock = std::chrono::steady_clock;
  using timestamp = std::chrono::time_point<clock>;
  using duration = timestamp::duration;
  using microseconds = std::chrono::microseconds;
  using seconds = std::chrono::seconds;

  Timestamp() = default;
  ~Timestamp() = default;
  Timestamp(const timestamp &ts) : m_ts(ts) {}

  auto is_valid() -> bool { return m_ts.time_since_epoch() > duration::zero(); }

  auto seconds_since_epoch() const -> seconds {
    return std::chrono::duration_cast<seconds>(m_ts.time_since_epoch());
  }

  auto microseconds_since_epoch() const -> microseconds {
    return std::chrono::duration_cast<microseconds>(m_ts.time_since_epoch());
  }

  auto operator<(const Timestamp &r) const -> bool {
    return this->microseconds_since_epoch() < r.microseconds_since_epoch();
  }

  auto operator>(const Timestamp &r) const -> bool {
    return this->microseconds_since_epoch() > r.microseconds_since_epoch();
  }

  auto operator==(const Timestamp &r) const -> bool {
    return this->microseconds_since_epoch() == r.microseconds_since_epoch();
  }

  auto operator!=(const Timestamp &r) const -> bool {
    return this->microseconds_since_epoch() != r.microseconds_since_epoch();
  }

  auto to_string() const -> std::string {
    char buf[32] = {0};
    int64_t seconds = seconds_since_epoch().count();
    int64_t microseconds = microseconds_since_epoch().count();
    snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds,
             microseconds);
    return std::string(buf);
  }

public:
  static auto now() -> Timestamp { return clock::now(); }

  static auto now_after(double seconds) -> Timestamp {
    return clock::now() +
           std::chrono::duration_cast<duration>(
               std::chrono::duration<double, seconds::period>(seconds));
  }

  static auto microseconds_from_now_to(const Timestamp &ts) -> microseconds {
    return ts.microseconds_since_epoch() - now().microseconds_since_epoch();
  }

private:
  timestamp m_ts;
};

#endif
