#include "woody/websocket/websocket_handler.h"

#include <muduo/base/Logging.h>
#include "woody/base/string_util.h"
#include "woody/base/ssl_util.h"

using namespace std;
using namespace woody;

WebsocketHandler::WebsocketHandler(const std::string& name)
    : BaseHandler(),
      name_(name) {
}
 
bool WebsocketHandler::HandleUpgrade(const HTTPHandlerPtr& handler,
                                     const HTTPRequest& req,
                                     HTTPResponse& resp) {
  if (req.GetMethod() != "GET") {
    // error
    return false;
  }
  string v;
  if (!req.GetHeader("Upgrade", v) &&
       v == "websocket" &&
       req.GetHeader("Connection", v) &&
       v == "upgrade") {
    // error
    return false;
  }

  string key;
  if (req.GetHeader("Sec-WebSocket-Key", key)) {
    string ws_key = base64_decode(key);
    if (ws_key.size() != 16) {
      //error
      return false;
    }
  }

  string version;
  if(req.GetHeader("Sec-WebSocket-Version", version)) {
    if(string_to_int(version) != kVersion) {
      //error
      return false;
    }
  }
  
  string protocols;
  if (req.GetHeader("Sec-WebSocket-Protocol", protocols)) {
    vector<string> protocol_vec;
    if (!protocols.empty()) {
      split(protocols, ",", protocol_vec);
      for(int i = 0, len = protocol_vec.size(); i < len; i ++) {
        trim(protocol_vec[i]);
        AddSubProtocol(protocol_vec[i]);
      }
    }
  }
  // TODO extension

  string temp = key + kWebsocketGUID;
  string accept_key = base64_encode(sha1(temp));

  resp.AddHeader("Upgrade", "websocket")
      .AddHeader("Connection", "Upgrade")
      .AddHeader("Sec-WebSocket-Version", version)
      .AddHeader("Sec-WebSocket-Accept", accept_key)
      .SetStatus(101, "Switching Protocols");

  SetCallback(handler->GetConn());
  
  resp.End();
  return true;
}

void WebsocketHandler::ForceClose() {
  assert(conn_ != NULL);
  conn_->forceClose();
}

void WebsocketHandler::SetCallback(const muduo::net::TcpConnectionPtr& conn) {
  ws_codec_.SetErrorCallback(
      boost::bind(&WebsocketHandler::HandleError, this));
  // websocket message callback TODO simplify?
  ws_codec_.SetTextMessageCallback(
      boost::bind(&WebsocketHandler::OnTextMessage, this, _1));
  ws_codec_.SetBinaryMessageCallback(
      boost::bind(&WebsocketHandler::OnBinaryMessage, this, _1));
  ws_codec_.SetCloseMessageCallback(
      boost::bind(&WebsocketHandler::OnCloseMessage, this, _1));
  ws_codec_.SetPingMessageCallback(
      boost::bind(&WebsocketHandler::OnPingMessage, this, _1));
  ws_codec_.SetPongMessageCallback(
      boost::bind(&WebsocketHandler::OnPongMessage, this, _1));

  conn_ = conn;
  conn_->setMessageCallback(
      boost::bind(&WebsocketHandler::OnData, this, _1, _2, _3));
  conn_->setCloseCallback(
      boost::bind(&WebsocketHandler::OnClose, this));
}

void WebsocketHandler::OnData(const muduo::net::TcpConnectionPtr& conn,
                              muduo::net::Buffer* buf,
                              muduo::Timestamp) {
  LOG_INFO << "WebsocketHandler::OnData [" << GetName()
           << "] - protocol: Websocket.";
  while (buf->readableBytes() > 0) {
    string data = convert_to_std(buf->retrieveAllAsString());
    size_t parsed_bytes = 0;
    // It means codec error or we need to wait for more data.
    if (!ws_codec_.OnData(data, parsed_bytes) ||
        !parsed_bytes) {
      break; 
    }
    string unparsed_str(data, parsed_bytes);
    LOG_DEBUG << "unparsed bytes" << unparsed_str.size()
              << "unparsed_str : " << unparsed_str;
    buf->prepend(unparsed_str.c_str(), unparsed_str.size());
    LOG_DEBUG << "readable bytes: " << buf->readableBytes();
  }
}

void WebsocketHandler::OnClose() {
  if (close_callback_with_this_) {
    close_callback_with_this_(shared_from_this());
  }
  if (close_callback_) {
    close_callback_();
  }
}

void WebsocketHandler::HandleError() { 
  //TODO log_error
  ForceClose();
}

bool WebsocketHandler::SendTextMessage(const TextMessage& message) {
  return SendWebsocketMessage(WebsocketMessage::kTextMessage, message.GetData());
}

void WebsocketHandler::OnTextMessage(const TextMessage& message) {
  text_message_callback_(shared_from_this(), message);
}

void WebsocketHandler::OnBinaryMessage(const BinaryMessage& message) {
  binary_message_callback_(shared_from_this(), message);
}

void WebsocketHandler::OnCloseMessage(const CloseMessage& message) {
  close_message_callback_(shared_from_this(), message);
}

void WebsocketHandler::OnPingMessage(const PingMessage& message) {
  // TODO PONG
  ping_message_callback_(shared_from_this(), message);
}

void WebsocketHandler::OnPongMessage(const PongMessage& message) {
  pong_message_callback_(shared_from_this(), message);
}

bool WebsocketHandler::SendWebsocketMessage(
    const WebsocketMessage::MessageType type,
    const string& data) {
  WebsocketFrame frame;
  if (!ws_codec_.ConvertMessageToFrame(type, data, frame)) {
    LOG_ERROR << "message.SingleFrame error";
    return false;
  }
  string frame_string;
  if(!ws_codec_.ConvertFrameToString(frame, frame_string)) {
    LOG_ERROR << "message.SingleFrame error";
    return false;
  }
  conn_->send(frame_string);
  return true;
}

