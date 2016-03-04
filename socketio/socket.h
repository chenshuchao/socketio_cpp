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
  typedef boost::function<void (const SocketPtr&)> CloseCallbackWithThis;
  typedef boost::function<void ()> CloseCallback;
  Socket(const ClientPtr& client, const NamespacePtr& nsp);

  Socket& On(const std::string& event, const EventCallback& cb);

  void Emit(const std::string& event, const std::string& data);

  std::string GetSid() const { return sid_; }

  void OnConnect();

  void OnPacket(const Packet& packet);

  void Join(const std::string& room);

  void Leave(const std::string& room);

  void Broadcast(const std::string& room,
                 const std::string& event,
                 const std::string& data);

  void SetCloseCallback(const CloseCallback& cb) {
    close_callback_ = cb;
  }
  void SetCloseCallbackWithThis(const CloseCallbackWithThis& cb) {
    close_callback_with_this_ = cb;
  }

  void Close();

 private:
  void OnEventPacket(const Packet& packet);
  void OnAckPacket(const Packet& packet);
  void OnError(const std::string& data);

  void SendPacket(const Packet& packet);

  std::string sid_;
  boost::weak_ptr<Client> client_wptr_;
  boost::weak_ptr<Namespace> nsp_wptr_;
  std::map<std::string, EventCallback> events_;
  CloseCallbackWithThis close_callback_with_this_;
  CloseCallback close_callback_;
};
typedef boost::shared_ptr<Socket> SocketPtr;
}

#endif
