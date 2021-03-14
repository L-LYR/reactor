#ifndef MINI_MODUO_DECL
#define MINI_MODUO_DECL

// Each Channel is responsible for only one fd's IO.
class Channel;

class ChannelCallback;
class AcceptorCallback;

// Acceptor is derived from ChannelCallback.
// It will save the listenfd, and it is responsible 
// for the acception and creation of connections.
class Acceptor;
// TcpConnectionHandler is derived from ChannelCallback.
// It is responsible for the only IO through one connection.
class TcpConnectionHandler;

// TcpServer is derived from AcceptorCallback.
// It will save a map: connfd -> connection handler.
// It has an acceptor for only accepting connection requests.
class TcpServer;

#endif