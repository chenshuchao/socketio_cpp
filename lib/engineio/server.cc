#include "engineio/server.h"

#include <fstream>
#include <muduo/base/Logging.h>

#include "engineio/base/util.h"
#include "engineio/base/json_util.h"
#include "engineio/transports/websocket.h"

using namespace std;
using namespace woody;
using namespace engineio;

Server::Server(const string& name) 
    : cookie_prefix_("io"),
      pingInterval_(25000),
      pingTimeout_(60000) {
}

void Server::HandleRequest(const HTTPHandlerPtr& handler, 
                           const HTTPRequest& req,
                           HTTPResponse& resp) {
  // TODO url prefix
  LOG_DEBUG << "Handling http request, method: " << req.GetMethod()
            << ", url: " << req.GetUrl();
  if (!VerifyRequest(handler, req, resp)) return;
  string sid;
  req.GetGETParams("sid", sid);
  string transport;
  req.GetGETParams("transport", transport);

  if (sid.empty()) {
    Handshake(handler, req, resp, transport);
  } else {
    SocketPtr socket = sockets_[sid];
    socket->HandleRequest(handler, req, resp);
  }
}

void Server::Handshake(const HTTPHandlerPtr& handler,
                       const HTTPRequest& req,
                       HTTPResponse& resp,
                       const string& transport_name) {
  // TODO
  string sid =  GenerateBase64ID();
  LOG_DEBUG << "Server::Handshake - " << "sid : " << sid;

  resp.AddHeader("Set-Cookie", cookie_prefix_ + "=" + sid);
  BaseTransportPtr tran(transports_.GetFactory(transport_name)->Create(handler, req, resp));
  string b64;
  if (req.GetGETParams("b64", b64)) {
    tran->SetSupportBinary(false);
  } else {
    tran->SetSupportBinary(true);
  }

  SocketPtr socket(new Socket(sid, tran));
  socket->SetCloseCallbackWithThis(
      boost::bind(&Server::OnSocketClose, this, _1));
  socket->SetMessageCallback(
      boost::bind(&Server::OnMessage, this, _1, _2));
  socket->SetPingCallback(
      boost::bind(&Server::OnPingMessage, this, _1, _2));
  sockets_[sid] = socket;

  tran->HandleRequest(handler, req, resp);
  string body;
  JsonCodec codec;
  vector<string> upgrades;
  tran->GetAllUpgrades(upgrades);
  codec.Add("sid", sid)
       .Add("upgrades", upgrades)
       .Add("pingInterval", pingInterval_)
       .Add("pingTimeout", pingTimeout_);
  socket->SendOpenPacket(codec.Stringify());
  if (connection_callback_) {
    connection_callback_(socket);
  }
}

void Server::OnPingMessage(const SocketPtr& socket, const string& data) {
  if (ping_message_callback_) {
    ping_message_callback_(socket, data);
  }
}

void Server::OnMessage(const SocketPtr& socket, const std::string& data) {
  if (message_callback_) {
    message_callback_(socket, data);
  }
}

bool Server::VerifyRequest(const HTTPHandlerPtr& handler,
                           const HTTPRequest& req,
                           HTTPResponse& resp) {
  string transport;
  if (!(req.GetGETParams("transport", transport) &&
       transports_.IsValid(transport))) {
    LOG_ERROR << "Unknown transport : " << transport;
    HandleRequestError(handler, req, resp, kUnknownTransport);
    return false;
  }

  string sid;
  req.GetGETParams("sid", sid);
  if (sid.empty()) {
    if ("GET" != req.GetMethod()) {
      HandleRequestError(handler, req, resp, kBadHandshakeMethod);
      return false;
    }
  } else {
    if (sockets_.find(sid) == sockets_.end()) {
      HandleRequestError(handler, req, resp, kUnknownSid);
      return false;
    }
    if (!req.IsUpgrade() &&
        sockets_[sid]->GetTransport()->GetName() != transport) {
      HandleRequestError(handler, req, resp, kBadRequest);
      return false;
    }
  }
  return true;
}

void Server::HandleRequestError(const HTTPHandlerPtr& handler,
                                const HTTPRequest& req,
                                HTTPResponse& resp,
                                ErrorCode code) {
  resp.SetStatus(400, "Bad Request");
  resp.AddHeader("Content-Type", "application/json");
  string origin;
  if (req.GetHeader("origin", origin)) {
    resp.AddHeader("Access-Control-Allow-Credentials", "true")
        .AddHeader("Access-Control-Allow-Origin", origin);
  } else {
    resp.AddHeader("Access-Control-Allow-Origin", "*");
  }
 
  JsonCodec codec;
  codec.Add("code", code);
  const char* message = kErrorMessage[code];
  codec.Add("message", message);

  resp.AddBody(codec.Stringify());
  resp.End();
}

void Server::OnSocketClose(const SocketPtr& socket) {
  LOG_DEBUG << "Server::OnSocketClose - "
            << socket->GetName();
  sockets_.erase(socket->GetSid());
}

