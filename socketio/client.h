#ifndef SOCKETIO_CLIENT_H
#define SOCKETIO_CLIENT_H

#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <engineio/socket.h>

#include "socketio/socket.h"
#include "socketio/parser.h"
#include "socketio/callback.h"

namespace socketio {
class Server;
class NamespaceManager;
typedef boost::shared_ptr<NamespaceManager> NamespaceManagerPtr;

class Client : public boost::enable_shared_from_this<Client> {
 public:
  typedef boost::function<void (ClientPtr)> CloseCallback;
  Client(Server* server, const engineio::SocketPtr& socket);

  std::string GetEngineIOSocketName() const {
    return eio_socket_->GetName();
  }

  int GetID() const { return id_; }

  bool Connect(const std::string& nsp_name);

  void Emit(const std::string& event, const std::string& data);

  void SendPacket(const Packet& packet);

  void SetSocketConnectCallback(const SocketConnectCallback& cb) {
    socket_connect_callback_ = cb;
  }
  void SetCloseCallback(const CloseCallback& cb) {
    close_callback_ = cb;
  }

  void ForceClose();

 private:
  void OnData(const std::string& data);

  void OnMessage(const engineio::SocketPtr& socket, const std::string& data);

  void OnPing(const engineio::SocketPtr& socket, const std::string& data);

  void Send(const std::string& data) {
    eio_socket_->SendMessage(data);
  }

  void OnClose();

  Server* server_;
  engineio::SocketPtr eio_socket_;
  int id_;
  // { namespace_name, Socket}
  std::map<std::string, SocketPtr> sockets_;
  SocketConnectCallback socket_connect_callback_;
  CloseCallback close_callback_;
};
typedef boost::shared_ptr<Client> ClientPtr;
}

#endif
