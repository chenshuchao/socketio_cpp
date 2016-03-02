#include "woody/websocket/websocket_server.h"

#include <boost/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <bytree/logging.hpp>
#include <bytree/string_util.hpp>

using namespace std;
using namespace bytree;
using namespace woody;

void WebsocketServer::HandleRequest(const HTTPHandlerPtr& handler,
                                    const HTTPRequest& req,
                                    HTTPResponse& resp) {
  if (req.IsUpgrade()) {
  }
}

void WebsocketServer::OnConnection(const muduo::net::TcpConnectionPtr& conn) {
  string conn_name = convert_to_std(conn->name());
  WebsocketHandlerPtr handler(new WebsocketHandler(conn_name, conn));
  handler->SetRequestCompleteCallback(
      boost::bind(&WebsocketServer::OnRequest, this, _1, _2, _3));
  handler->SetTextMessageCallback(
      boost::bind(&WebsocketServer::OnWebsocketTextMessage, this, _1, _2));
  handler->SetBinaryMessageCallback(
      boost::bind(&WebsocketServer::OnBinaryMessage, this, _1, _2));
  handler->SetCloseMessageCallback(
      boost::bind(&WebsocketServer::OnCloseMessage, this, _1, _2));
  handler->SetPingMessageCallback(
      boost::bind(&WebsocketServer::OnPingMessage, this, _1, _2));
  handler->SetPongMessageCallback(
      boost::bind(&WebsocketServer::OnPongMessage, this, _1, _2));
  handler->SetOnCloseCallback(
      boost::bind(&WebsocketServer::OnHandlerClose, this, _1));
  handler->SetForceCloseCallback(
      boost::bind(&WebsocketServer::OnHandlerForceClose, this, _1));
  handler_map_.insert(
      pair<string, WebsocketHandlerPtr>(conn_name, handler));
}

void WebsocketServer::OnDisconnection(const muduo::net::TcpConnectionPtr& conn) {
  WebsocketHandlerPtr handler = handler_map_[convert_to_std(conn->name())];
  if (!handler) {
    LOG_ERROR << "WebsocketServer::OnDisconnection [" << name_
              << "] connection [" << conn->name()
              << " handler not found.";
  }
  handler->OnClose();
}

void WebsocketServer::OnHandlerClose(const WebsocketHandlerPtr& handler) {
  LOG_DEBUG << "WebsocketServer::OnHandlerClose - "
            << handler->GetName();
  handler_map_.erase(convert_to_std(handler->GetConn()->name()));
  if (handler_close_callback_) {
    handler_close_callback_(handler);
  }
}

