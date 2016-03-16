#include "engineio/server.h"

#include <fstream>
#include <jsoncpp/json/json.h>
#include <bytree/logging.hpp>
#include <bytree/string_util.hpp>

#include "engineio/base/util.h"
#include "engineio/transports/websocket.h"

using namespace std;
using namespace bytree;
using namespace woody;
using namespace engineio;

static const char* kErrorMessage [] = {
  "Transport unknown",
  "Session ID unknown",
  "Bad handshake method",
  "Bad request"
};

Server::Server(const string& name) 
    : cookie_prefix_("io"),
      path_prefix_("/engine.io"),
      ping_interval_(25000),
      ping_timeout_(60000) {
}

void Server::HandleRequest(const HTTPHandlerPtr& handler, 
                           const HTTPRequest& req,
                           HTTPResponse& resp) {
  LOG(DEBUG) << "Server::HandleReqeust - "
             << "method: " << req.GetMethod()
             << ", url: " << req.GetUrl();
  if (!path_prefix_.empty() &&
      !StartsWith(req.GetUrl(), path_prefix_)) {
    LOG(ERROR) << "Server::HandleReqeust - "
               << "Error: HTTP request did not start with "
               << path_prefix_ << ".";
    return;
  }

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
  string sid;
  while(true) {
    sid = GenerateBase64ID();
    if(sockets_.find(sid) == sockets_.end()) break;
  }
  LOG(DEBUG) << "Server::Handshake - " << "sid : " << sid;

  resp.AddHeader("Set-Cookie", cookie_prefix_ + "=" + sid);

  BaseTransportFactoryPtr factory = transports_.GetFactory(transport_name);
  BaseTransportPtr tran(factory->Create(handler, req, resp));
  
  // "b64" in request query means no supporting binary.
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
  socket->HandleRequest(handler, req, resp);

  string body;
  Json::StyledWriter writer;
  Json::Value root(Json::objectValue);

  vector<string> upgrades;
  tran->GetAllUpgrades(upgrades);
  Json::Value vec(Json::arrayValue);
  for (unsigned int i = 0; i < upgrades.size(); i ++) {
    vec.append(upgrades[i]);
  }

  root["sid"] = sid;
  root["upgrades"] = vec;
  root["pingInterval"] = ping_interval_;
  root["pingTimeout"] = ping_timeout_;

  socket->SendOpenPacket(writer.write(root));

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
    LOG(ERROR) << "Unknown transport : " << transport;
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
 
  Json::StyledWriter writer;
  Json::Value root;
  root["code"] = code;
  root["message"] = kErrorMessage[code];

  resp.AddBody(writer.write(root));
  resp.End();
}

void Server::OnSocketClose(const SocketPtr& socket) {
  LOG(DEBUG) << "Server::OnSocketClose - "
            << socket->GetName();
  sockets_.erase(socket->GetSid());
}

