#include "echo_service.hh"

EchoCtx::EchoCtx() { reset(); }

auto EchoCtx::reset() -> void {
  is_kill = is_cr = is_eof = need_key = available = false;
  is_first_line = true;
  first_line_field = 0;
  prev_ch = '\0';
  tmp_key.clear();
  tmp_value.clear();
}

EchoService::EchoService() { reset(); }

auto EchoService::reset() -> void {
  m_parse_state.reset();
  m_method.clear();
  m_path.clear();
  m_proto_ver.clear();
  m_headers.clear();
  m_response.clear();
}

auto EchoService::get_response() -> const char * { return m_response.c_str(); }

auto EchoService::get_response_length() -> size_t {
  return m_response.length();
}

auto EchoService::get_kill_sig() -> bool { return m_parse_state.is_kill; }

auto EchoService::parse(const char *buffer, int size) -> void {
  char ch;
  for (int i = 0; i < size; ++i, m_parse_state.prev_ch = ch) {
    ch = buffer[i];
    if (ch == '\r') {
      m_parse_state.is_first_line = false;
      m_parse_state.is_cr = true;
    } else if (m_parse_state.is_cr and ch == '\n') {
      m_parse_state.is_cr = false;
      m_parse_state.need_key = true;
      m_parse_state.available = true;
      m_headers[m_parse_state.tmp_key] = m_parse_state.tmp_value;
      m_parse_state.tmp_key.clear();
      m_parse_state.tmp_value.clear();
    } else if (m_parse_state.is_first_line) {
      if (ch == ' ') {
        m_parse_state.first_line_field++;
      } else {
        switch (m_parse_state.first_line_field) {
        case 0:
          m_method.append(1, ch);
          break;
        case 1:
          m_path.append(1, ch);
          break;
        case 2:
          m_proto_ver.append(1, ch);
          break;
        default:
          break;
        }
      }
    } else {
      if (ch == ' ' and m_parse_state.prev_ch == ':') {
        m_parse_state.tmp_key.pop_back();
        m_parse_state.need_key = false;
      } else if (m_parse_state.need_key) {
        m_parse_state.tmp_key.append(1, ch);
      } else {
        m_parse_state.tmp_value.append(1, ch);
      }
    }
  }
  if (m_path == "/stop") {
    m_parse_state.is_kill = true;
  }
}

auto EchoService::generate_response() -> bool {
  if (!m_parse_state.available) {
    return false;
  }

  if (m_parse_state.is_kill) {
    std::string response_body = "Good Bye!";
    m_response = "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/html; charset=UTF-8\r\n"
                 "Content-Length: " +
                 std::to_string(response_body.length()) + "\r\n\r\n" +
                 response_body;
    return true;
  }

  std::string response_body = "<!DOCTYPE html><html><head>"
                              "<title>Request info</title>"
                              "<style>"
                              "table, th, td { border: 1px solid #333; }"
                              "th, td { padding: 3px 5px; }"
                              "th { text-align: right; }"
                              "td { text-align: left; }"
                              "</style>"
                              "</head>"
                              "<body><h1>Request info</h1>";

  response_body += "<table>";

  response_body += "<tr><th>Method</th><td>" + m_method + "</td></tr>";
  response_body += "<tr><th>Path</th><td>" + m_path + "</td></tr>";
  response_body += "<tr><th>Protocol</th><td>" + m_proto_ver + "</td></tr>";

  for (const auto &header : m_headers) {
    response_body +=
        "<tr><th>" + header.first + "</th><td>" + header.second + "</td></tr>";
  }

  response_body += "</table>";
  response_body += "</body></html>\r\n";

  m_response = "HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html; charset=UTF-8\r\n"
               "Connection: keep-alive\r\n"
               "Content-Length: " +
               std::to_string(response_body.length()) + "\r\n\r\n" +
               response_body;
  return true;
}