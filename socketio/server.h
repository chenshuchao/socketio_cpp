#ifndef SOCKETIO_SERVER_H
#define SOCKETIO_SERVER_H

#include <string>

#include <boost/bind.hpp>
#include <engineio/server.h>

#include "socketio/callback.h"
#include "socketio/namespace.h"
#include "socketio/client.h"

namespace socketio {

class Server : public woody::HTTPApplication {
 public:
  typedef std::map<std::string, ClientPtr> String2ClientPtr;
  Server(const std::string& name);

  virtual void HandleRequest(const woody::HTTPHandlerPtr& handler,
                             const woody::HTTPRequest& request,
                             woody::HTTPResponse& resp);

  // Set callback when a socketio::Socket connects.
  void SetSocketConnectCallback(const SocketConnectCallback& cb) {
    socket_connect_callback_ = cb;
  }

  // Set broadcast adapter factory.
  void SetAdapterFactory(const AdapterFactory& factory) {
    adapter_factory_ = factory;
  }

  // Set http request url path prefix, defalut to "/engine.io".
  void SetPathPrefix(const std::string& prefix) {
    eio_server_.SetPathPrefix(prefix);
  }

  // Set cookie prefix, default to "io".
  void SetCookiePrefix(const std::string& prefix) {
    eio_server_.SetCookiePrefix(prefix);
  }

  void SetPingInterval(unsigned int time) {
    eio_server_.SetPingInterval(time);
  }
 
  void SetPingTimeout(unsigned int time) {
    eio_server_.SetPingTimeout(time);
  }

  // 
  void Of(std::string nsp_name);

  NamespacePtr GetNamespace(const std::string& nsp_name);

 private:
  // Call when a engineio::Socket connects.
  void OnConnection(const engineio::SocketPtr& socket);

  // Call when a socketio::Client close.
  void OnClientClose(const ClientPtr& client);

  engineio::Server eio_server_;
  // {Socket_name, ClientPtr}
  SocketConnectCallback socket_connect_callback_;
  std::map<std::string, NamespacePtr> nsps_;
  String2ClientPtr clients_;
  AdapterFactory adapter_factory_;
};
}

#endif
