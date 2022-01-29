#ifndef MINI_MUDUO_LOGGER_BASE
#define MINI_MUDUO_LOGGER_BASE

#include <cstdarg>
#include <cstdio>

template <typename Base> class _Logger {
public:
  template <typename... Args>
  auto fatal(const char *format, Args... args) -> void {
    get_base()->print("Fatal", format, args...);
  }
  template <typename... Args>
  auto error(const char *format, Args... args) -> void {
    get_base()->print("Error", format, args...);
  }
  template <typename... Args>
  auto warn(const char *format, Args... args) -> void {
    get_base()->print("Warn", format, args...);
  }
  template <typename... Args>
  auto info(const char *format, Args... args) -> void {
    get_base()->print("Info", format, args...);
  }
  template <typename... Args>
  auto trace(const char *format, Args... args) -> void {
    get_base()->print("Trace", format, args...);
  }

private:
  auto get_base() -> Base * { return static_cast<Base *>(this); }
};

class SimpleLogger : public _Logger<SimpleLogger> {
  friend class _Logger<SimpleLogger>;

private:
  template <typename... Args>
  auto print(const char *level, const char *format, Args... args) -> void {
    ::printf("[%s] ", level);
    ::printf(format, args...);
  }
  auto print(const char *level, const char *str) -> void {
    ::printf("[%s] %s", level, str);
  }
};

#endif