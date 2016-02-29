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
  Server(const std::string& name);

  virtual void HandleRequest(const woody::HTTPHandlerPtr& handler,
                             const woody::HTTPRequest& request,
                             woody::HTTPResponse& resp);

  void SetSocketConnectCallback(const SocketConnectCallback& cb) {
    socket_connect_callback_ = cb;
  }

  void SetPathPrefix(const std::string& prefix) {
    eio_server_.SetPathPrefix(prefix);
  }

  void SetCookiePrefix(const std::string& prefix) {
    eio_server_.SetCookiePrefix(prefix);
  }

  void SetPingInterval(unsigned int time) {
    eio_server_.SetPingInterval(time);
  }
 
  void SetPingTimeout(unsigned int time) {
    eio_server_.SetPingTimeout(time);
  }

  void Of(std::string nsp_name);

  NamespacePtr GetNamespace(const std::string& nsp_name);

 private:
  void OnConnection(const engineio::SocketPtr& socket);

  void OnClientClose(const ClientPtr& client);

  engineio::Server eio_server_;
  // {Socket_name, ClientPtr}
  SocketConnectCallback socket_connect_callback_;
  std::map<std::string, NamespacePtr> nsps_;
  std::map<std::string, ClientPtr> clients_;
};
}

#endif
