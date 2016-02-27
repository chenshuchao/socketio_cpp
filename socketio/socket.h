#ifndef SOCKETIO_SOCKET_H
#define SOCKETIO_SOCKET_H

#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/weak_ptr.hpp>

#include "socketio/packet.h"
#include "socketio/callback.h"

namespace socketio {
class Client;
class Namespace;
class NamespaceManager; 
typedef boost::shared_ptr<Client> ClientPtr;
typedef boost::shared_ptr<Namespace> NamespacePtr;
typedef boost::shared_ptr<NamespaceManager> NamespaceManagerPtr;

class Socket : public boost::enable_shared_from_this<Socket> {
 public:
  Socket(const ClientPtr& client, const NamespacePtr& nsp);

  Socket& On(const std::string& event, const EventCallback& cb);

  void Emit(const std::string& event, const std::string& data);

  int GetID() const { return id_; }

  void OnConnect();

  void OnPacket(const Packet& packet);

 private:
  void OnEventPacket(const Packet& packet);
  void OnAckPacket(const Packet& packet);
  void ForceClose();
  void OnError(const std::string& data);

  void SendPacket(const Packet& packet);

  int id_;
  boost::weak_ptr<Client> client_wptr_;
  boost::weak_ptr<Namespace> nsp_wptr_;
  std::map<std::string, EventCallback> events_;
};
typedef boost::shared_ptr<Socket> SocketPtr;
}

#endif
