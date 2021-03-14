#ifndef MINI_MODUO_ACCEPTOR_CALLBACK
#define MINI_MODUO_ACCEPTOR_CALLBACK

#include "./decl.hh"

class AcceptorCallback {
  public:
    virtual auto on_connection(int sockfd) -> void{};
};

#endif