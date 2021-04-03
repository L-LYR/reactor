#include "./buffer.hh"

#include <sys/uio.h>

#include <iostream>

#include "./def.hh"

Buffer::Buffer(size_t initial_size) : m_buffer(buffer_prepend_size + initial_size),
                                      m_read_index(buffer_prepend_size),
                                      m_write_index(buffer_prepend_size) {}

auto Buffer::readable_bytes() -> size_t { return m_write_index - m_read_index; }

auto Buffer::writable_bytes() -> size_t { return m_buffer.size() - m_write_index; }

auto Buffer::readable_begin() -> const char* { return begin() + m_read_index; }

auto Buffer::read_from_sockfd(int sockfd) -> int {
    static char extra_buffer[extra_buffer_size];

    size_t cur_writable_bytes = writable_bytes();
    iovec io_vec[2] = {
        {
            writable_begin(),
            cur_writable_bytes,
        },
        {
            extra_buffer,
            extra_buffer_size,
        },
    };
    int iovec_cnt = (cur_writable_bytes < extra_buffer_size) ? 2 : 1;
    ssize_t ret = ::readv(sockfd, io_vec, iovec_cnt);
    if (ret < 0) {
        std::cout << "error in readv(), errno: " << errno << std::endl;
    } else if (size_t(ret) <= cur_writable_bytes) {
        has_written(ret);
    } else {
        m_write_index = m_buffer.size();
        append(extra_buffer, ret - cur_writable_bytes);
    }
    return ret;
}

auto Buffer::append(const char* data, size_t length) -> void {
    if (writable_bytes() < length) {
        make_space(length);
    }
    std::copy(data, data + length, writable_begin());
    has_written(length);
}

auto Buffer::writable_begin() -> char* { return begin() + m_write_index; }

auto Buffer::retrieve(size_t length) -> void {
    if (length < readable_bytes()) {
        m_read_index += length;
    } else {
        retrieve_all();
    }
}

auto Buffer::has_written(size_t length) -> void {
    m_write_index += length;
}

auto Buffer::begin() -> char* { return &*m_buffer.begin(); }

auto Buffer::prependable_bytes() -> size_t { return m_read_index; }

auto Buffer::retrieve_all() -> void {
    m_read_index = buffer_prepend_size;
    m_write_index = buffer_prepend_size;
}

auto Buffer::make_space(size_t length) -> void {
    if (writable_bytes() + prependable_bytes() < length + buffer_prepend_size) {
        m_buffer.resize(m_write_index + length);
    } else {
        size_t readable = readable_bytes();
        std::copy(begin() + m_read_index,
                  begin() + m_write_index,
                  begin() + buffer_prepend_size);
        m_read_index = buffer_prepend_size;
        m_write_index = m_read_index + readable;
    }
}