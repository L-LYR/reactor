#include "./tcp_server.hh"

auto main() -> int {
    TcpServer server;
    server.run();
    return 0;
}
