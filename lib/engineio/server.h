#ifndef ENGINEIO_SERVER_H
#define ENGINEIO_SERVER_H

#include <map>

#include <woody/http/http_application.h>
#include "engineio/socket.h"
#include "engineio/transports/transport_factory.h"

namespace engineio {
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
  
  // Implement interface, handling http request.
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

  void SetPathPrefix(const std::string& prefix) {
    path_prefix_ = prefix;
  }

  void SetCookiePrefix(const std::string& prefix) {
    cookie_prefix_ = prefix;
  }

  void SetPingInterval(unsigned int time) {
    ping_interval_ = time;
  }
 
  void SetPingTimeout(unsigned int time) {
    ping_timeout_ = time;
  }

 private:
  // Called when ping message was parsed from transport.
  void OnPingMessage(const SocketPtr&, const std::string& data);

  // Called when text message was parsed from transport.
  void OnMessage(const SocketPtr& socket, const std::string& data);

  bool VerifyRequest(const woody::HTTPHandlerPtr& handler,
                     const woody::HTTPRequest& req,
                     woody::HTTPResponse& resp);

  // Handshake a new client. If successfully, a new Socket will be created.
  void Handshake(const woody::HTTPHandlerPtr& handler,
                 const woody::HTTPRequest& req,
                 woody::HTTPResponse& resp,
                 const std::string& transport_name);

  // Send error message to client
  void HandleRequestError(const woody::HTTPHandlerPtr& handler,
                          const woody::HTTPRequest& req,
                          woody::HTTPResponse& resp,
                          ErrorCode code);

  // Called when Socket was closed to clean up server's data.
  void OnSocketClose(const SocketPtr& socket);

  std::string cookie_prefix_;  // prefix of cookie
  std::string path_prefix_;    // prefix of url path
  unsigned int ping_interval_; // ms
  unsigned int ping_timeout_;  // ms

  
  // {sid : socket}, for http request to find its sockets.
  std::map<std::string, SocketPtr> sockets_;

  // { transport_name : transport_factory }
  Transports transports_;
  MessageCallback message_callback_;
  MessageCallback ping_message_callback_;
  ConnectionCallback connection_callback_;
};
}

#endif
