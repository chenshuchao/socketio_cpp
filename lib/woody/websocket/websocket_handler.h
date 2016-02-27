#ifndef WOODY_WEBSOCKET_WEBSOCKETHANDLER_H
#define WOODY_WEBSOCKET_WEBSOCKETHANDLER_H

#include <vector>

#include <boost/enable_shared_from_this.hpp>

#include "woody/websocket/websocket_codec.h"
#include "woody/http/http_handler.h"

namespace woody {
static const int kHTTP = 0;
static const int kWebsocket = 1;
//static const int kVersions[] = {9, 10, 11, 12, 13}
static const int kVersion = 13; // only support 13
static const std::string kWebsocketGUID("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

class WebsocketHandler : public BaseHandler,
                         public boost::enable_shared_from_this<WebsocketHandler> {
 public:
  typedef boost::shared_ptr<WebsocketHandler> WebsocketHandlerPtr;
  typedef boost::function<void ()> CloseCallback;
  typedef boost::function<void (const WebsocketHandlerPtr&)>
      CloseCallbackWithThis;
  typedef boost::function<void (const WebsocketHandlerPtr&,
                                const TextMessage&)> TextMessageCallback;
  typedef boost::function<void (const WebsocketHandlerPtr&,
                                const BinaryMessage&)> BinaryMessageCallback;
  typedef boost::function<void (const WebsocketHandlerPtr&,
                                const CloseMessage&)> CloseMessageCallback;
  typedef boost::function<void (const WebsocketHandlerPtr&,
                                const PingMessage&)> PingMessageCallback;
  typedef boost::function<void (const WebsocketHandlerPtr&,
                                const PongMessage&)> PongMessageCallback;

  WebsocketHandler(const std::string& name);

  virtual ~WebsocketHandler() { }

  std::string GetName() const { return name_; }

  bool HandleUpgrade(const HTTPHandlerPtr& handler,
                     const HTTPRequest& req,
                     HTTPResponse& resp);

  void AddSubProtocol(std::string protocol) {
    sub_protocols_.push_back(protocol);
  }

  virtual void ForceClose();

  bool SendTextMessage(const TextMessage& message);

  bool SendBinaryMessage(const BinaryMessage& message);

  bool SendCloseMessage(const CloseMessage& message);

  bool SendPingMessage(const PingMessage& message);
  // TODO private?
  bool SendPongMessage(const PongMessage& message);

  void SetCloseCallback(const CloseCallback& cb) {
    close_callback_ = cb;
  }
  void SetCloseCallbackWithThis(const CloseCallbackWithThis& cb) {
    close_callback_with_this_ = cb;
  }

  void SetTextMessageCallback(const TextMessageCallback& cb) {
    text_message_callback_ = cb;
  }
  void SetBinaryMessageCallback(const BinaryMessageCallback& cb) {
    binary_message_callback_ = cb;
  }
  void SetCloseMessageCallback(const CloseMessageCallback& cb) {
    close_message_callback_ = cb;
  }
  void SetPingMessageCallback(const PingMessageCallback& cb) {
    ping_message_callback_ = cb;
  }
  void SetPongMessageCallback(const PongMessageCallback& cb) {
    pong_message_callback_ = cb;
  }
 
 private:
  void SetCallback(const muduo::net::TcpConnectionPtr& conn);

  virtual void OnData(const muduo::net::TcpConnectionPtr& conn,
                      muduo::net::Buffer* buf,
                      muduo::Timestamp);
 
  virtual void OnClose();

  void HandleError();
 
  void OnTextMessage(const TextMessage& message);

  void OnBinaryMessage(const BinaryMessage& message);

  void OnCloseMessage(const CloseMessage& message);

  void OnPingMessage(const PingMessage& message);

  void OnPongMessage(const PongMessage& message);

  bool SendWebsocketMessage(const WebsocketMessage::MessageType type,
                            const std::string& data);

  std::string name_;
  muduo::net::TcpConnectionPtr conn_;
  std::vector<std::string> sub_protocols_;
  WebsocketCodec ws_codec_;

  TextMessageCallback text_message_callback_;
  BinaryMessageCallback binary_message_callback_;
  CloseMessageCallback close_message_callback_;
  PingMessageCallback ping_message_callback_;
  PongMessageCallback pong_message_callback_;

  CloseCallback close_callback_;
  CloseCallbackWithThis close_callback_with_this_;
};
typedef boost::shared_ptr<WebsocketHandler> WebsocketHandlerPtr;
}

#endif
