#include "woody/http/http_server.h"

#include <assert.h>

#include <muduo/base/Logging.h>
#include "woody/base/string_util.h"
#include "woody/http/http_application.h"

using namespace std;
using namespace woody;

HTTPServer::HTTPServer(int port, const string& name)
    : loop_(),
      tcp_server_(&loop_,
                  muduo::net::InetAddress(port),
                  convert_to_muduo(name),
                  muduo::net::TcpServer::kNoReusePort) {
  tcp_server_.setConnectionCallback(
      boost::bind(&HTTPServer::OnCreateOrDestroyConnection, this, _1));
}

void HTTPServer::Start() {
  LOG_INFO << "HTTPServer::Start [" << name_ << "].";
  tcp_server_.start();
  loop_.loop();
}

void HTTPServer::Handle(const string& path_prefix, HTTPApplication* app) {
  apps_.push_back(make_pair(path_prefix, app));
}

void HTTPServer::OnRequest(const HTTPHandlerPtr& handler,
                           const HTTPRequest& req,
                           HTTPResponse& resp) {
  for (vector<pair<string, HTTPApplication*> >::iterator it = apps_.begin();
       it != apps_.end();
       it ++) {
    if (IsStringBeginWith(req.GetUrl(), (*it).first)) {
      LOG_DEBUG << "LALA : url:" << req.GetUrl()
          << "prefix:" << (*it).first;
      (*it).second->HandleRequest(handler, req, resp);
      break;
    }
  }
}

void HTTPServer::OnCreateOrDestroyConnection(
         const muduo::net::TcpConnectionPtr& conn) {
  if (conn->connected()) {
    OnConnection(conn);
  }
  else {
    OnDisconnection(conn);
  }
}

void HTTPServer::OnConnection(const muduo::net::TcpConnectionPtr& conn) {
  string conn_name = convert_to_std(conn->name());
  HTTPHandlerPtr handler(new HTTPHandler(conn_name, conn));
  handler->SetRequestCompleteCallback(
      boost::bind(&HTTPServer::OnRequest, this, _1, _2, _3));
  handler_map_.insert(pair<string, HTTPHandlerPtr>(conn_name, handler));
}

void HTTPServer::OnDisconnection(const muduo::net::TcpConnectionPtr& conn) {
  // Just do nothing because conn will call HTTPHandler::OnClose().
}

void HTTPServer::OnHandlerClose(const HTTPHandlerPtr& handler) {
  LOG_DEBUG << "HTTPServer::OnHandlerClose - "
            << handler->GetName();
  handler_map_.erase(convert_to_std(handler->GetConn()->name()));
}

