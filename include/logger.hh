#ifndef MINI_MUDUO_LOGGER
#define MINI_MUDUO_LOGGER

#include <fmt/color.h>

#include <filesystem>
#include <mutex>
#include <vector>

class LogLevel {
public:
  enum class Value : uint8_t {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    NOTICE = 3,
    WARNING = 4,
    ERROR = 5,
    CRITICAL = 6,
    FATAL = 7,
  };

  constexpr static const char *level_str[] = {
      "TRACE", "DEBUG",    "INFO",  "NOTICE", "WARNING",
      "ERROR", "CRITICAL", "ALERT", "FATAL",
  };

  constexpr static const fmt::text_style text_color[] = {
      fmt::fg(fmt::terminal_color::white),                        // TRACE
      fmt::fg(fmt::terminal_color::white) | fmt::emphasis::bold,  // DEBUG
      fmt::fg(fmt::terminal_color::blue),                         // INFO
      fmt::fg(fmt::terminal_color::blue) | fmt::emphasis::bold,   // NOTICE
      fmt::fg(fmt::terminal_color::yellow),                       // WARN
      fmt::fg(fmt::terminal_color::yellow) | fmt::emphasis::bold, // ERROR
      fmt::fg(fmt::terminal_color::red),                          // CRITICAL
      fmt::fg(fmt::terminal_color::red) | fmt::emphasis::bold,    // FATAL
  };

public:
  LogLevel() = delete;
  constexpr LogLevel(Value v) : m_v(v) {}

  ~LogLevel() = default;

  // allow comparison and switch-case
  constexpr operator Value() const { return m_v; };
  // prevent if-condition
  explicit operator bool() = delete;

public:
  constexpr auto to_str() const -> const char * {
    return level_str[static_cast<uint8_t>(m_v)];
  };
  constexpr auto get_color() const -> fmt::text_style {
    return text_color[static_cast<uint8_t>(m_v)];
  }

private:
  Value m_v;
};

class Logger {
public:
  class Option {
    friend class Logger;

  public:
    Option() = default;
    ~Option() = default;

  public:
    auto mute_all() -> Option &;
    auto with_level(const LogLevel &l) -> Option &;
    auto with_color(bool b) -> Option &;
    auto with_timestamp(bool b) -> Option &;
    auto with_file_location(bool b) -> Option &;
    auto with_function_name(bool b) -> Option &;

  private:
  private:
    LogLevel::Value m_mask = LogLevel::Value::TRACE;
    bool m_muted = false;
    bool m_need_color = true;
    bool m_need_timestamp = true;
    bool m_need_location = true;
    bool m_need_function_name = true;
  };

public:
  Logger() : m_option() {}

  Logger(const Option &opt) : m_option(opt) {}

  ~Logger() = default;

  template <typename... Args>
  auto log(const LogLevel &l, const char *filename, int line_number,
           const char *function_name, const char *format_str,
           Args &&...args) const -> void;

  auto set_option(const Option &opt) -> void;

private:
  auto log_level(const LogLevel &l) const -> const Logger &;
  auto log_timestamp() const -> const Logger &;
  auto log_location(const char *filename, int line_number) const
      -> const Logger &;
  auto log_function_name(const char *function_name) const -> const Logger &;

  template <typename... Args>
  auto log(const LogLevel &l, const char *format_str, Args &&...args) const
      -> void;

private:
  auto mute(const LogLevel &) const -> bool;

private:
  Option m_option;
};

template <typename... Args>
auto Logger::log(const LogLevel &l, const char *filename, int line_number,
                 const char *function_name, const char *format_str,
                 Args &&...args) const -> void {
  if (m_option.m_muted || mute(l)) {
    return;
  }
  this->log_timestamp()
      .log_level(l)
      .log_location(filename, line_number)
      .log_function_name(function_name)
      .log(l, format_str, args...);
}

template <typename... Args>
auto Logger::log(const LogLevel &l, const char *format_str,
                 Args &&...args) const -> void {
  if (m_option.m_need_color) {
    fmt::print(l.get_color(), " {}\n", fmt::format(format_str, args...));
  } else {
    fmt::print(" {}\n", fmt::format(format_str, args...));
  }
}

class GlobalLogger : public Logger {
public:
  static auto get() -> GlobalLogger & {
    static GlobalLogger instance;
    return instance;
  }

public:
  GlobalLogger(const GlobalLogger &) = delete;
  auto operator=(const GlobalLogger &) -> GlobalLogger & = delete;

private:
  GlobalLogger() {}
  ~GlobalLogger() {}
};

#define glog(l, ...)                                                           \
  GlobalLogger::get().log(l, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define trace(...) glog(LogLevel::Value::TRACE, __VA_ARGS__)
#define debug(...) glog(LogLevel::Value::DEBUG, __VA_ARGS__)
#define info(...) glog(LogLevel::Value::INFO, __VA_ARGS__)
#define notice(...) glog(LogLevel::Value::NOTICE, __VA_ARGS__)
#define warning(...) glog(LogLevel::Value::WARNING, __VA_ARGS__)
#define error(...) glog(LogLevel::Value::ERROR, __VA_ARGS__)
#define critical(...) glog(LogLevel::Value::CRITICAL, __VA_ARGS__)
#define fatal(...) glog(LogLevel::Value::FATAL, __VA_ARGS__)

auto set_global_logger_option(const Logger::Option& opt) -> void;

#endif