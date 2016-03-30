#ifndef SOCKETIO_CLIENT_H
#define SOCKETIO_CLIENT_H

#include <map>

#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <engineio/socket.h>

#include "socketio/callback.h"

namespace socketio {
class Server;
class Socket;
class NamespaceManager;
class Packet;
typedef boost::shared_ptr<Socket> SocketPtr;
typedef boost::shared_ptr<NamespaceManager> NamespaceManagerPtr;

class Client : public boost::enable_shared_from_this<Client> {
 public:
  typedef std::map<std::string, SocketPtr> String2SocketPtr;
  typedef boost::function<void (ClientPtr)> CloseCallback;
  Client(Server* server, const engineio::SocketPtr& socket);

  std::string GetEngineIOSocketName() const {
    return eio_socket_->GetName();
  }

  std::string GetSid() const { return id_; }

  // Try to connect a namespace.
  bool Connect(const std::string& nsp_name);

  // Send socketio::Packet content to client.
  void SendPacket(const Packet& packet);

  void SetSocketConnectCallback(const SocketConnectCallback& cb) {
    socket_connect_callback_ = cb;
  }
  void SetCloseCallback(const CloseCallback& cb) {
    close_callback_ = cb;
  }

  void ForceClose();

  // Remove engineio::Socket.
  void Remove(const SocketPtr& socket);

 private:
  void OnData(const std::string& data);

  // Call when engineio gets a text engineio::Packet.
  void OnMessage(const engineio::SocketPtr& socket, const std::string& data);

  // Call when engineio gets a ping engineio::Packet.
  void OnPing(const engineio::SocketPtr& socket, const std::string& data);

  void Send(const std::string& data) {
    eio_socket_->SendMessage(data);
  }

  void OnClose();

  Server* server_;
  engineio::SocketPtr eio_socket_;
  std::string id_;
  // { namespace_name, Socket}
  String2SocketPtr sockets_;
  SocketConnectCallback socket_connect_callback_;
  CloseCallback close_callback_;
};
typedef boost::shared_ptr<Client> ClientPtr;
}

#endif
