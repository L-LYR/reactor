#ifndef MINI_MUDUO_DECL
#define MINI_MUDUO_DECL

// Each Channel is responsible for only one fd's IO.
class Channel;

class ChannelCallback;
class AcceptorCallback;

// Acceptor is derived from ChannelCallback.
// It will save the listenfd, and it is responsible
// for the acception and creation of connections.
class Acceptor;
// TcpConnection is derived from ChannelCallback.
// It is responsible for the only IO through one connection.
class TcpConnection;

class Selector;

class EventLoop;

// TcpServerBase is derived from AcceptorCallback.
// It will save a map: connfd -> connection handler.
// It has an acceptor for only accepting connection requests.
class TcpServerBase;

// Server is a virtual base class for user as an interface.
class Server;

#endif