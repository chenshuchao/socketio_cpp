#ifndef ENGINEIO_SERVER_H
#define ENGINEIO_SERVER_H

#include <map>

#include <woody/http/http_application.h>
#include "engineio/socket.h"
#include "engineio/transports/transport_factory.h"

namespace engineio {
static const char* kErrorMessage [] = {
  "Transport unknown",
  "Session ID unknown",
  "Bad handshake method",
  "Bad request"
};

class Server : public woody::HTTPApplication {
 public:
  enum ErrorCode {
    kUnknownTransport = 0,
    kUnknownSid = 1,
    kBadHandshakeMethod = 2,
    kBadRequest = 3,
  };
  typedef boost::function<void (const SocketPtr& socket)>
      ConnectionCallback;
  typedef boost::function<void (const SocketPtr& socket,
                                const std::string& data)>
      MessageCallback;
  Server(const std::string& name);

  virtual ~Server() { }

  virtual void HandleRequest(const woody::HTTPHandlerPtr& handler,
                             const woody::HTTPRequest& req,
                             woody::HTTPResponse& resp);
 
  void SetConnectionCallback(const ConnectionCallback& cb) {
    connection_callback_ = cb;
  } 
  void SetMessageCallback(const MessageCallback& cb) {
    message_callback_ = cb;
  }
  void SetPingMessageCallback(const MessageCallback& cb) {
    ping_message_callback_ = cb;
  }

 private:
  // Called when ping message was parsed from transport
  void OnPingMessage(const SocketPtr&, const std::string& data);
  // Called when text message was parsed from transport
  void OnMessage(const SocketPtr& socket, const std::string& data);

  bool VerifyRequest(const woody::HTTPHandlerPtr& handler,
                     const woody::HTTPRequest& req,
                     woody::HTTPResponse& resp);
  // Handshake a new client
  // If successfully, a new Socket will be created.
  void Handshake(const woody::HTTPHandlerPtr& handler,
                 const woody::HTTPRequest& req,
                 woody::HTTPResponse& resp,
                 const std::string& transport_name);
  // Send error message to client
  void HandleRequestError(const woody::HTTPHandlerPtr& handler,
                          const woody::HTTPRequest& req,
                          woody::HTTPResponse& resp,
                          ErrorCode code);

  void OnSocketClose(const SocketPtr& socket);

  std::string cookie_prefix_;  // prefix of cookie
  int pingInterval_;   // ms
  int pingTimeout_;
  // {sid : socket}
  // for http request to find its sockets
  std::map<std::string, SocketPtr> sockets_;
  Transports transports_;
  MessageCallback message_callback_;
  MessageCallback ping_message_callback_;
  ConnectionCallback connection_callback_;
};
}

#endif
