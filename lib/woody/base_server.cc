#include "woody/base_server.h"

#include <boost/bind.hpp>
#include <boost/bind/placeholders.hpp>
#include <muduo/base/Logging.h>

#include "woody/base/string_util.h"

using namespace std;
using namespace woody;

BaseServer::BaseServer(muduo::net::EventLoop& loop,
                       int port,
                       BaseHandlerFactory* factory,
                       const string& name)
    : handler_factory_(factory),
      tcp_server_(&loop,
                  muduo::net::InetAddress(port),
                  convert_to_muduo(name),
                  muduo::net::TcpServer::kNoReusePort) {
  tcp_server_.setConnectionCallback(
      boost::bind(&BaseServer::OnCreateOrDestroyConnection, this, _1));
  tcp_server_.setMessageCallback(
      boost::bind(&BaseServer::OnData, this, _1, _2, _3));
}

void BaseServer::Start() {
  LOG_INFO << "BaseServer::Start [" << name_ << "].";
  tcp_server_.start();
}

// TODO
void BaseServer::Stop() { }

void BaseServer::OnCreateOrDestroyConnection(
         const muduo::net::TcpConnectionPtr& conn) {
  if (conn->connected()) {
    OnConnection(conn);
  }
  else {
    OnDisconnection(conn);
  }
}

void BaseServer::OnConnection(const muduo::net::TcpConnectionPtr& conn) {
  CreateHandler(conn);
}

void BaseServer::OnDisconnection(const muduo::net::TcpConnectionPtr& conn) {
  BaseHandlerPtr handler = FindHandler(conn);
  if (!handler) {
    LOG_ERROR << "BaseServer::OnDisconnection [" << name_
              << "] connection [" << conn->name()
              << " handler not found.";
  }
  RemoveHandler(handler);
}

void BaseServer::OnData(const muduo::net::TcpConnectionPtr& conn,
                        muduo::net::Buffer* buf,
                        muduo::Timestamp) {
  BaseHandlerPtr handler = FindHandler(conn);
  if (!handler) {
    LOG_ERROR << "BaseServer::OnData [" << name_
              << "] connecton [" << conn->name()
              << " handler not found.";
    return;
  }
  handler->OnData(buf);
}

void BaseServer::OnHandlerError(const BaseHandlerPtr& handler) {
  tcp_server_.removeAndDestroyConnection(handler->GetConn());
}

void BaseServer::CreateHandler(const muduo::net::TcpConnectionPtr& conn) {
  string conn_name = convert_to_std(conn->name());
  BaseHandlerPtr handler(handler_factory_->CreateHandler(conn_name, conn));
  handler->SetErrorCallback(
      boost::bind(&BaseServer::OnHandlerError, this, _1));
  handler_map_.insert(
      pair<string, BaseHandlerPtr>(convert_to_std(conn->name()), handler));
}

void BaseServer::RemoveHandler(const BaseHandlerPtr& handler) {
  handler_map_.erase(convert_to_std(handler->GetConn()->name()));
}

BaseHandlerPtr BaseServer::FindHandler(const muduo::net::TcpConnectionPtr& conn) {
  return handler_map_[convert_to_std(conn->name())];
}

