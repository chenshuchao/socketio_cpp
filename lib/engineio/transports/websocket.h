#ifndef ENGINEIO_TRANSPORTS_WEBSOCKET_H
#define ENGINEIO_TRANSPORTS_WEBSOCKET_H

#include <woody/websocket/websocket_handler.h>
#include "engineio/transport.h"

namespace engineio {
class WebsocketTransport : public BaseTransport {
 public:
  WebsocketTransport(const woody::HTTPHandlerPtr& handler,
                     const woody::HTTPRequest& req,
                     woody::HTTPResponse& resp);

  virtual ~WebsocketTransport() { }

  virtual void GetAllUpgrades(std::vector<std::string>& vec) {
    // do not support upgrade
  }

  virtual void HandleRequest(const woody::HTTPHandlerPtr& handler,
                             const woody::HTTPRequest& req,
                             woody::HTTPResponse& resp);

  virtual bool IsWritable() const { return writable_; }

  virtual void SendPackets(std::vector<Packet>& packets);

  virtual void OnError();

  virtual void ForceClose() {
    handler_->ForceClose();
  }

 private:
  virtual void OnData(const std::string& data);

  void OnTextMessage(const woody::WebsocketHandlerPtr& handler,
                     const woody::TextMessage& message);

  void OnBinaryMessage(const woody::WebsocketHandlerPtr& handler,
                       const woody::BinaryMessage& message);
 
  void OnCloseMessage(const woody::WebsocketHandlerPtr& handler,
                      const woody::CloseMessage& message);
  
  void OnPingMessage(const woody::WebsocketHandlerPtr& handler,
                     const woody::PingMessage& message) {
  }

  void OnPongMessage(const woody::WebsocketHandlerPtr& handler,
                     const woody::PongMessage& message) {
  }

  virtual void OnClose();

  woody::WebsocketHandlerPtr handler_;
  bool writable_;
  bool upgraded_;
};
}

#endif
