#ifndef ENGINEIO_SOCKET_H
#define ENGINEIO_SOCKET_H

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "engineio/transport.h"
#include "engineio/transports/transport_factory.h"

namespace engineio {
class Socket : public boost::enable_shared_from_this<Socket> {
 public:
  typedef boost::shared_ptr<Socket> SocketPtr;
  typedef boost::function<void (const SocketPtr&)> CloseCallbackWithThis;
  typedef boost::function<void ()> CloseCallback;
  typedef boost::function<void (const SocketPtr&, const std::string&)>
      PingCallback;
  typedef boost::function<void (const SocketPtr&, const std::string&)>
      MessageCallback;
  enum State {
    kStateOpening,
    kStateOpen,
    kStateClosing,
    kStateClose,
  };

  Socket(const std::string sid,
         const BaseTransportPtr& tran);

  std::string GetName() const { return name_; }

  std::string GetSid() const { return sid_; }

  BaseTransportPtr GetTransport() const { return transport_; }
  
  void SetTransport(const BaseTransportPtr& tran);

  void SetCloseCallback(const CloseCallback& cb) {
    close_callback_ = cb;
  }
  void SetCloseCallbackWithThis(const CloseCallbackWithThis& cb) {
    close_callback_with_this_ = cb;
  }
  void SetMessageCallback(const MessageCallback& cb) {
    message_callback_ = cb;
  }
  void SetPingCallback(const PingCallback& cb) {
    ping_callback_ = cb;
  }

  void HandleRequest(const woody::HTTPHandlerPtr& handler,
                     const woody::HTTPRequest& req,
                     woody::HTTPResponse& resp);

  void SendOpenPacket(const std::string& data) {
    CreateAndSendPacket(Packet::kPacketOpen, data);
  }
  void SendMessage(const std::string& data) {
    CreateAndSendPacket(Packet::kPacketMessage, data);
  }

  void ForceClose();

  void OnClose();

 private:
  // Called when packet was send from transport
  void OnPacket(const Packet& packet);
  //
  void OnPacketWhenUpgrading(const Packet& packet);
  // Handle ping packet
  void OnPingPacket(const Packet& packet);
  // Handle message packet
  void OnMessagePacket(const Packet& packet);

  void OnUpgradePacket(const Packet& packet);

  void CreateAndSendPacket(int packet_type, const std::string& data);

  // Handle upgrade request and wait for upgrade packet.
  bool MaybeUpgrade(const woody::HTTPHandlerPtr& handler,
                    const woody::HTTPRequest& req,
                    woody::HTTPResponse& resp);
  void Flush();

  std::string name_;
  std::string sid_;
  State state_;
  BaseTransportPtr transport_;
  BaseTransportPtr upgrading_transport_;
  Transports transports_;

  CloseCallback close_callback_;
  CloseCallbackWithThis close_callback_with_this_;
  
  PingCallback ping_callback_;
  MessageCallback message_callback_;
  bool upgraded_;
 
  // TODO
  std::vector<Packet> write_buffer_;
};
typedef boost::shared_ptr<Socket> SocketPtr;
}

#endif
