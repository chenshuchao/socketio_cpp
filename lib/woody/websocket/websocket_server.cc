#include "woody/websocket/websocket_server.h"

#include <boost/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <muduo/base/Logging.h>

#include "woody/base/string_util.h"

using namespace std;
using namespace woody;

WebsocketServer::WebsocketServer(int port, const string& name)
    : loop_(),
      tcp_server_(&loop_,
                  muduo::net::InetAddress(port),
                  convert_to_muduo(name),
                  muduo::net::TcpServer::kNoReusePort) {
  tcp_server_.setConnectionCallback(
      boost::bind(&WebsocketServer::OnCreateOrDestroyConnection, this, _1));
  tcp_server_.setMessageCallback(
      boost::bind(&WebsocketServer::OnData, this, _1, _2, _3));
}

void WebsocketServer::Start() {
  LOG_INFO << "WebsocketServer::Start [" << name_ << "].";
  tcp_server_.start();
  loop_.loop();
}

// TODO
void WebsocketServer::Stop() { }

void WebsocketServer::OnCreateOrDestroyConnection(
         const muduo::net::TcpConnectionPtr& conn) {
  if (conn->connected()) {
    OnConnection(conn);
  }
  else {
    OnDisconnection(conn);
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

void WebsocketServer::OnData(const muduo::net::TcpConnectionPtr& conn,
                        muduo::net::Buffer* buf,
                        muduo::Timestamp) {
  WebsocketHandlerPtr handler = handler_map_[convert_to_std(conn->name())];
  if (!handler) {
    LOG_ERROR << "WebsocketServer::OnData [" << name_
              << "] connecton [" << conn->name()
              << " handler not found.";
    return;
  }
  handler->OnData(buf);
}

void WebsocketServer::OnHandlerClose(const WebsocketHandlerPtr& handler) {
  LOG_DEBUG << "WebsocketServer::OnHandlerClose - "
            << handler->GetName();
  handler_map_.erase(convert_to_std(handler->GetConn()->name()));
  if (handler_close_callback_) {
    handler_close_callback_(handler);
  }
}

void WebsocketServer::OnHandlerForceClose(const WebsocketHandlerPtr& handler) {
  tcp_server_.removeAndDestroyConnection(handler->GetConn());
}

