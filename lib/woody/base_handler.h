#ifndef WOODY_BASEHANDLER_H
#define WOODY_BASEHANDLER_H

#include <string>
#include <boost/enable_shared_from_this.hpp>
#include <muduo/net/Buffer.h>

namespace woody {
class BaseHandler {
 public:
  typedef boost::shared_ptr<BaseHandler> BaseHandlerPtr;
  typedef boost::function<void (BaseHandlerPtr)> ErrorCallback;
  BaseHandler() { }
  virtual ~BaseHandler() { }

  virtual void ForceClose() {
  }

 private:
  virtual void OnData(const muduo::net::TcpConnectionPtr& conn,
                      muduo::net::Buffer* buf,
                      muduo::Timestamp) = 0;
  virtual void OnClose() = 0;

  //ErrorCallback error_callback_;
};
typedef boost::shared_ptr<BaseHandler> BaseHandlerPtr;

class BaseHandlerFactory {
 public:
  virtual BaseHandler* CreateHandler(const std::string& name,
                                     const muduo::net::TcpConnectionPtr& conn) = 0;
};
typedef boost::shared_ptr<BaseHandlerFactory> BaseHandlerFactoryPtr;
}

#endif
