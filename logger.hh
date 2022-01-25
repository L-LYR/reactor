#ifndef MINI_MUDUO_LOGGER
#define MINI_MUDUO_LOGGER

#include "./logger_base.hh"

using Logger = _Logger<SimpleLogger>;

extern Logger gLogger;

template <typename... Args>
auto fatal(const char *format, Args... args) -> void {
  gLogger.fatal(format, args...);
}

template <typename... Args>
auto error(const char *format, Args... args) -> void {
  gLogger.error(format, args...);
}

template <typename... Args>
auto warn(const char *format, Args... args) -> void {
  gLogger.warn(format, args...);
}

template <typename... Args>
auto info(const char *format, Args... args) -> void {
  gLogger.info(format, args...);
}

template <typename... Args>
auto trace(const char *format, Args... args) -> void {
  gLogger.trace(format, args...);
}

#endif