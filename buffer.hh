#ifndef MINI_MODUO_BUFFER
#define MINI_MODUO_BUFFER

#include <string>
#include <vector>

#include "./def.hh"

class Buffer {
  public:
    explicit Buffer(size_t initial_size = buffer_initial_size);
    ~Buffer() = default;

    auto readable_bytes() -> size_t;
    auto writable_bytes() -> size_t;

    auto readable_begin() -> const char*;
    auto writable_begin() -> char*;

    auto read_from_sockfd(int sockfd) -> int;

    auto append(const char* data, size_t length) -> void;

    auto retrieve_all() -> void;
    auto retrieve(size_t length) -> void;
    auto has_written(size_t length) -> void;

  private:
    auto prependable_bytes() -> size_t;

    auto begin() -> char*;

    auto make_space(size_t length) -> void;

  private:
    std::vector<char> m_buffer;
    size_t m_read_index;
    size_t m_write_index;
};

#endif