#include "logger.hh"

#include <fmt/chrono.h>
#include <fmt/core.h>

#include <chrono>
#include <iostream>
#include <numeric>

using Option = Logger::Option;

auto Option::mute_all() -> Option & {
  m_muted = true;
  return *this;
}

auto Option::with_level(const LogLevel &l) -> Option & {
  m_mask = l;
  return *this;
}

auto Option::with_color(bool t) -> Option & {
  m_need_color = t;
  return *this;
}

auto Option::with_timestamp(bool t) -> Option & {
  m_need_timestamp = t;
  return *this;
}

auto Option::with_file_location(bool t) -> Option & {
  m_need_location = t;
  return *this;
}

auto Option::with_function_name(bool t) -> Option & {
  m_need_function_name = t;
  return *this;
}

auto Logger::mute(const LogLevel &l) const -> bool {
  return m_option.m_mask > l;
}

auto Logger::set_option(const Option &opt) -> void { m_option = opt; }

auto Logger::log_level(const LogLevel &l) const -> const Logger & {
  if (m_option.m_need_color) {
    fmt::print(l.get_color(), "[{}]", l.to_str());
  } else {
    fmt::print("[{}]", l.to_str());
  }
  return *this;
}

auto Logger::log_timestamp() const -> const Logger & {
  if (m_option.m_need_timestamp) {
    fmt::print("[{:%Y-%m-%d %H:%M:%S}]", std::chrono::system_clock::now());
  }
  return *this;
}

auto Logger::log_location(const char *filename, int line_number) const
    -> const Logger & {
  if (m_option.m_need_location) {
    fmt::print("[{}:{}]", std::filesystem::relative(filename).c_str(),
               line_number);
  }
  return *this;
}

auto Logger::log_function_name(const char *function_name) const
    -> const Logger & {
  if (m_option.m_need_function_name) {
    fmt::print("[{}]", function_name);
  }
  return *this;
}

auto set_global_logger_option(const Logger::Option &opt) -> void {
  GlobalLogger::get().set_option(opt);
}