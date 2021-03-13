#pragma once

#include <sys/epoll.h>
#include <arpa/inet.h>
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

const int max_listen_fd = 5;
const int max_lines = 100;
const int max_events = 500;

class tcp_server {
  public:
    tcp_server() = default;
    ~tcp_server() = default;

    auto run() -> void;
};