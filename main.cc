#include "./decl.hh"
#include "./event_loop.hh"
#include "./echo_server.hh"

auto main() -> int {
    EventLoop event_loop;
    EchoServer echo_server(&event_loop);

    echo_server.run();
    event_loop.run();

    return 0;
}
