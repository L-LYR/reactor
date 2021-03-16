#ifndef MINI_MUDUO_HTTP_REQUEST_PARSER
#define MINI_MUDUO_HTTP_REQUEST_PARSER

#include <string>
#include <map>

class RequestHandler {
  public:
    RequestHandler() = default;
    ~RequestHandler() = default;

    auto parse(const char *buffer, int size) -> void;
    auto generate_response() -> bool;
    auto get_response() -> const char *;
    auto get_response_length() -> int;

  private:
    struct ParseState {
        // used for parse, save state
        bool is_cr;
        bool is_eof;
        bool is_first_line = true;
        bool need_key;
        bool available;
        int first_line_field = 0;
        char prev_ch;
        std::string tmp_key;
        std::string tmp_value;
        
        ParseState() = default;
    };

    ParseState m_parse_state;

    // results
    std::string m_method;
    std::string m_path;
    std::string m_proto_ver;

    std::map<std::string, std::string> m_headers;

    std::string m_response;
};

#endif