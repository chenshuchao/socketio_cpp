#include "engineio/transports/websocket.h"
#include <bytree/logging.hpp>
#include <woody/base/string_util.h>

using namespace std;
using namespace woody;
using namespace engineio;

WebsocketTransport::WebsocketTransport(const HTTPHandlerPtr& handler,
                                       const HTTPRequest& req,
                                       HTTPResponse& resp)
    : BaseTransport("websocket"),
      handler_(new WebsocketHandler(convert_to_std(handler->GetConn()->name()))),
      writable_(false),
      upgraded_(false) {
}

void WebsocketTransport::HandleRequest(const HTTPHandlerPtr& handler,
                                       const HTTPRequest& req,
                                       HTTPResponse& resp) {
  // Websocket transport do not handle http request.
  if (!upgraded_ && req.IsUpgrade() &&
      handler_->HandleUpgrade(handler, req, resp)) {
    handler_->SetTextMessageCallback(
      boost::bind(&WebsocketTransport::OnTextMessage, this, _1, _2));
    handler_->SetBinaryMessageCallback(
      boost::bind(&WebsocketTransport::OnBinaryMessage, this, _1, _2));
    handler_->SetCloseMessageCallback(
      boost::bind(&WebsocketTransport::OnCloseMessage, this, _1, _2));
    handler_->SetPingMessageCallback(
      boost::bind(&WebsocketTransport::OnPingMessage, this, _1, _2));
    handler_->SetPongMessageCallback(
      boost::bind(&WebsocketTransport::OnPongMessage, this, _1, _2));
    handler_->SetCloseCallback(
      boost::bind(&WebsocketTransport::OnClose, this));
    writable_ = true;
    return;
  }

  resp.SetStatus(400, "Bad Request");
  resp.End();
}

void WebsocketTransport::SendPackets(vector<Packet>& packets) {
  writable_ = false;
  for(vector<Packet>::iterator it = packets.begin();
      it != packets.end();
      it ++) {
    string data;
    Parser::EncodePacket(*it, BaseTransport::IsSupportBinary(), data);
    woody::TextMessage message;
    message.Append(data);
    handler_->SendTextMessage(message);
    writable_ = true;
  }
}

void WebsocketTransport::OnError() {
}

void WebsocketTransport::OnData(const string& data) {
  Packet packet;
  Parser::DecodePacket(data, packet);
  OnPacket(packet);
}

void WebsocketTransport::OnTextMessage(const WebsocketHandlerPtr& handler,
                                       const TextMessage& message) {
  LOG(DEBUG) << message.GetData();
  OnData(message.GetData());
}

void WebsocketTransport::OnBinaryMessage(const WebsocketHandlerPtr& handler,
                                       const BinaryMessage& message) {
  LOG(DEBUG) << message.GetData();
  OnData(message.GetData());
}

void WebsocketTransport::OnCloseMessage(const WebsocketHandlerPtr& handler,
                                        const CloseMessage& message) {
  OnClose();  
}

void WebsocketTransport::OnClose() {
  BaseTransport::OnClose();
}

