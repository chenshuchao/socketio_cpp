#ifndef WOODY_WEBSOCKET_WEBSOCKETMESSAGE_H
#define WOODY_WEBSOCKET_WEBSOCKETMESSAGE_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "woody/websocket/websocket_frame.h"

namespace woody {
class WebsocketMessage {
 public:
  enum MessageType {
    kNoneMessage,
    kTextMessage,
    kBinaryMessage,
    kCloseMessage,
    kPingMessage,
    kPongMessage,
  };
  WebsocketMessage(MessageType type = kNoneMessage)
    : type_(type) {
  }
  //void SetComplete(bool b) { is_complete_ = b; }
  bool IsComplete() const { return is_complete_; }
  void Append(std::string& data);
  void CleanUp();
  MessageType GetType() const { return type_; }
//  void SetType(MessageType type) { type_ = type; }
  std::string GetData() const { return data_; }

 private:
  MessageType type_;
  std::string data_;
  bool is_complete_;
};
typedef boost::shared_ptr<WebsocketMessage> WebsocketMessagePtr;

class TextMessage : public WebsocketMessage
{
 public:
  TextMessage() {}
};

class BinaryMessage : public WebsocketMessage
{
};

class CloseMessage : public WebsocketMessage
{
};
class PingMessage : public WebsocketMessage
{

};

class PongMessage : public WebsocketMessage
{
};
}
#endif
