#ifndef BASESERVER_H
#define BASESERVER_H

#include <boost/noncopyable.hpp>

#include "woody/tcp_server.h"
#include "woody/base_handler.h"

namespace woody {
class BaseServer : boost::noncopyable {
 public:
  BaseServer(muduo::net::EventLoop &loop,
             int port,
             BaseHandlerFactory* factory,
             const std::string& name);
  void Start();
  void Stop();
  void Bind();

 private:
  //
  void OnCreateOrDestroyConnection(const muduo::net::TcpConnectionPtr& conn);
  void OnConnection(const muduo::net::TcpConnectionPtr& conn);
  void OnDisconnection(const muduo::net::TcpConnectionPtr& conn);
  void OnData(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp);
  //
  void OnHandlerError(const BaseHandlerPtr& handler);

  void CreateHandler(const muduo::net::TcpConnectionPtr& conn);
  void RemoveHandler(const BaseHandlerPtr& handler);
  BaseHandlerPtr FindHandler(const muduo::net::TcpConnectionPtr& conn);

  std::string name_;
  BaseHandlerFactoryPtr handler_factory_;
  muduo::net::TcpServer tcp_server_;
  std::map<std::string, BaseHandlerPtr> handler_map_;
};

typedef boost::shared_ptr<BaseServer> BaseServerPtr;
}

#endif

