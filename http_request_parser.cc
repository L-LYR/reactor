#include "./http_request_parser.hh"

RequestHandler::RequestHandler() { reset(); }

auto RequestHandler::has_finished() -> bool { return available; }

auto RequestHandler::reset() -> void {
    is_cr = false;
    is_eof = false;
    is_first_line = true;
    need_key = true;
    available = false;
    first_line_field = 0;
    prev_ch = '\0';

    tmp_key.clear();
    tmp_value.clear();

    m_method.clear();
    m_path.clear();
    m_proto_ver.clear();
    m_headers.clear();
}

auto RequestHandler::parse(const char* buffer, int size) -> void {
    char ch;
    for (int i = 0; i < size; ++i, prev_ch = ch) {
        ch = buffer[i];
        if (ch == '\r') {
            is_first_line = false;
            is_cr = true;
        } else if (is_cr and ch == '\n') {
            is_cr = false;
            need_key = true;
            available = true;
            m_headers[tmp_key] = tmp_value;
            tmp_key.clear();
            tmp_value.clear();

        } else if (is_first_line) {
            if (ch == ' ') {
                first_line_field++;
            } else {
                switch (first_line_field) {
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
            if (ch == ' ' and prev_ch == ':') {
                tmp_key.pop_back();
                need_key = false;
            } else if (need_key) {
                tmp_key.append(1, ch);
            } else {
                tmp_value.append(1, ch);
            }
        }
    }
}

auto RequestHandler::generate_response() -> std::string {
    std::string response_body =
        "<!DOCTYPE html><html><head>"
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

    for (const auto& header : m_headers) {
        response_body += "<tr><th>" + header.first + "</th><td>" + header.second + "</td></tr>";
    }

    response_body += "</table>";
    response_body += "</body></html>\r\n";

    std::string response_headers =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: " +
        std::to_string(response_body.length()) + "\r\n\r\n";

    return response_headers + response_body;
}