#ifndef WOODY_WEBSOCKET_WEBSOCKETSERVER_H
#define WOODY_WEBSOCKET_WEBSOCKETSERVER_H

#include <boost/noncopyable.hpp>
#include <muduo/net/EventLoop.h>

#include "woody/tcp_server.h"
#include "woody/http/http_handler.h"
#include "woody/http/http_application.h"
#include "woody/websocket/websocket_handler.h"

namespace woody {

class WebsocketServer : public HTTPApplication {
 public:
  typedef boost::function<void (const WebsocketHandlerPtr&)>
      HandlerCloseCallback;

  WebsocketServer(const std::string& name) { }

  ~WebsocketServer() { }

  void SetHandlerCloseCallback(const HandlerCloseCallback& cb) {
    handler_close_callback_ = cb;
  }

  virtual void HandleRequest(const HTTPHandlerPtr& handler,
                             const HTTPRequest& req,
                             HTTPResponse& resp);

  void OnTextMessage(const WebsocketHandlerPtr& handler,
                     const TextMessage&);

  void OnBinaryMessage(const WebsocketHandlerPtr& handler,
                       const BinaryMessage&);

  void OnCloseMessage(const WebsocketHandlerPtr& handler,
                      const CloseMessage&);

  void OnPingMessage(const WebsocketHandlerPtr& handler,
                     const PingMessage&);

  void OnPongMessage(const WebsocketHandlerPtr& handler,
                     const PongMessage&);

 private:
  //
  void OnHandlerClose(const WebsocketHandlerPtr& handler);

  std::string name_;

  std::map<std::string, WebsocketHandlerPtr> handler_map_;

  HandlerCloseCallback handler_close_callback_;
};
}

#endif
