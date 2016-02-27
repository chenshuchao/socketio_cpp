#ifndef WOODY_HTTP_HTTPHANDLER_H
#define WOODY_HTTP_HTTPHANDLER_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <muduo/net/TcpConnection.h>

#include "woody/base_handler.h"
#include "woody/http/http_codec.h"

namespace woody {
class HTTPHandler : public BaseHandler,
                    public boost::enable_shared_from_this<HTTPHandler> {
 public:
  typedef boost::shared_ptr<HTTPHandler> HTTPHandlerPtr;
  typedef boost::function<void (const HTTPHandlerPtr&, const HTTPRequest&, HTTPResponse&)>
      RequestCompleteCallback;
  typedef boost::function<void ()> CloseCallback;
  typedef boost::function<void (const HTTPHandlerPtr&)> CloseCallbackWithThis;
  
  HTTPHandler(const std::string& name,
              const muduo::net::TcpConnectionPtr& conn);
  virtual ~HTTPHandler() { }

  std::string GetName() const { return name_; }

  muduo::net::TcpConnectionPtr GetConn() const { return conn_; }

  virtual void ForceClose();

  virtual void Send(const std::string& data);

  void SetRequestCompleteCallback(const RequestCompleteCallback& cb) {
    request_complete_callback_ = cb;
  }
  void SetCloseCallback(const CloseCallback& cb) {
    close_callback_ = cb;
  }
  void SetCloseCallbackWithThis(const CloseCallbackWithThis& cb) {
    close_callback_with_this_ = cb;
  }

 private:
  virtual void OnData(const muduo::net::TcpConnectionPtr& conn,
                      muduo::net::Buffer* buf,
                      muduo::Timestamp);
  virtual void OnMessageBegin(HTTPCodec::StreamID id) { }
  virtual void OnMessageComplete(HTTPCodec::StreamID id, HTTPRequest& request);
  virtual void OnClose();
  void FormatResponse(HTTPResponse& resp); 
  void HandleError() {
    ForceClose();
  }

  std::string name_;
  muduo::net::TcpConnectionPtr conn_;
  HTTPCodec codec_;
  RequestCompleteCallback request_complete_callback_;
  CloseCallback close_callback_;
  CloseCallbackWithThis close_callback_with_this_;
};
typedef boost::shared_ptr<HTTPHandler> HTTPHandlerPtr;
}

#endif
