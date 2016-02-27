#ifndef WOODY_HTTP_HTTPSERVER_H
#define WOODY_HTTP_HTTPSERVER_H

#include <utility>

#include <boost/noncopyable.hpp>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include "woody/http/http_handler.h"

namespace woody {
class HTTPApplication;
class HTTPServer : boost::noncopyable {
 public:
  HTTPServer(int port, const std::string& name);
  void Start();
  void Stop();
  void Handle(const std::string& path_prefix, HTTPApplication* app);

 private:
  void OnRequest(const HTTPHandlerPtr& handler,
                 const HTTPRequest& req,
                 HTTPResponse& resp);
  void OnCreateOrDestroyConnection(const muduo::net::TcpConnectionPtr& conn);
  void OnConnection(const muduo::net::TcpConnectionPtr& conn);
  void OnDisconnection(const muduo::net::TcpConnectionPtr& conn);
  void OnHandlerClose(const HTTPHandlerPtr& handler);

  std::string name_;
  muduo::net::EventLoop loop_;
  muduo::net::TcpServer tcp_server_;
  std::vector<std::pair<std::string, HTTPApplication*> > apps_;
  std::map<std::string, HTTPHandlerPtr> handler_map_;
};
}

#endif
