#ifndef MINI_MODUO_HTTP_REQUEST_PARSER
#define MINI_MODUO_HTTP_REQUEST_PARSER

#include <string>
#include <map>

class RequestHandler {
  public:
    RequestHandler();
    ~RequestHandler() = default;

    auto parse(const char *buffer, int size) -> void;
    auto reset() -> void;
    auto has_finished() -> bool;
    auto generate_response() -> std::string;

  private:
    // used for parse, save state
    bool is_cr;
    bool is_eof;
    bool is_first_line;
    bool need_key;
    bool available;
    int first_line_field;
    char prev_ch;
    std::string tmp_key;
    std::string tmp_value;

    // results
    std::string m_method;
    std::string m_path;
    std::string m_proto_ver;

    std::map<std::string, std::string> m_headers;
};

#endif