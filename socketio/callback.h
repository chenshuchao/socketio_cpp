#ifndef SOCKETIO_CALLBACK_H
#define SOCKETIO_CALLBACK_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace socketio {
class Client;
typedef boost::shared_ptr<Client> ClientPtr;
class Socket;
typedef boost::shared_ptr<Socket> SocketPtr;
typedef boost::function<void (SocketPtr&)> SocketConnectCallback;
typedef boost::function<void (const SocketPtr&, const std::string&)>
    EventCallback;
}

#endif
