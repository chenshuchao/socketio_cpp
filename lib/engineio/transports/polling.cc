#include "engineio/transports/polling.h"

#include <bytree/logging.hpp>

using namespace std;
using namespace woody;
using namespace engineio;

void PollingTransport::SendPackets(vector<Packet>& packets) {
  string data;
  Parser::EncodePayload(packets, BaseTransport::IsSupportBinary(), data);
  DoWrite(data);
  writable_ = false;
}

void PollingTransport::HandleRequest(const HTTPHandlerPtr& handler,
                                     const HTTPRequest& req,
                                     HTTPResponse& resp) {
  if ("GET" == req.GetMethod()) {
    HandlePollRequest(req, resp);
  } else if ("POST" == req.GetMethod()) {
    HandleDataRequest(req, resp);
  } else {
    resp.SetStatus(500, "Internal Server Error");
    resp.End();
  }
}

void PollingTransport::OnData(const std::string& data) {
  vector<Packet> packets;
  Parser::DecodePayload(data, packets);
  for (unsigned int i = 0; i < packets.size(); i ++) {
    OnPacket(packets[i]);
  }
}

void PollingTransport::HandlePollRequest(const HTTPRequest& req,
                                         HTTPResponse& resp) {
  LOG(DEBUG) << "PollingTransport::HandlePollRequest";
  // TODO req resp constructor function
  if (req_ != NULL) {
    LOG(ERROR) << "Request overlap.";
    // OnError
    resp.SetStatus(500, "Internal server error");
    resp.End();
    return;
  }
  req_ = HTTPRequestPtr(new HTTPRequest(req));
  resp_ = resp;
  writable_ = true;
  TriggerFlush();
}

void PollingTransport::HandleDataRequest(const HTTPRequest& req,
                                         HTTPResponse& resp) {
  if (req_ != NULL) {
    LOG(DEBUG) << "Request overlap.";
    // OnError
    resp.SetStatus(500, "Internal server error");
    resp.End();
    return;
  }
  req_ = HTTPRequestPtr(new HTTPRequest(req));
  OnData(req.GetBody());
  resp.SetStatus(200, "OK")
       .AddHeader("Content-Type", "text/html")
       .AddHeader("Content-Length", "2")
       .AddBody("OK");
  resp.End();
  req_.reset();
}

