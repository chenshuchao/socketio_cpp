#ifndef WOODY_WEBSOCKET_WEBSOCKETCODEC_H
#define WOODY_WEBSOCKET_WEBSOCKETCODEC_H

#include <string>

#include "woody/websocket/websocket_parser.h"
#include "woody/websocket/websocket_message.h"

namespace woody {
class WebsocketCodec {
 public:
  typedef boost::function<void (const TextMessage&)> TextMessageCallback;
  typedef boost::function<void (const BinaryMessage&)> BinaryMessageCallback;
  typedef boost::function<void (const CloseMessage&)> CloseMessageCallback;
  typedef boost::function<void (const PingMessage&)> PingMessageCallback;
  typedef boost::function<void (const PongMessage&)> PongMessageCallback;
  typedef boost::function<void ()> ErrorCallback;

  WebsocketCodec();

  bool OnData(const std::string& data, size_t& parsed_bytes);
  bool ConvertFrameToString(const WebsocketFrame& frame, std::string& parsed_string);
  bool ConvertMessageToFrame(const WebsocketMessage::MessageType type,
                             const std::string data,
                             WebsocketFrame& frame);

/*
  void OnTextMessage(const WebsocketMessage& message)
  { text_message_callback_(); }
  void OnBinaryMessage(const BinaryMessage& message);
  void OnCloseMessage(const CloseControlMessage& message);
  void OnPingMessage(const PingControlMessage& message);
  void OnPongMessage(const PongControlMessage& message);
*/
  void SetTextMessageCallback(const TextMessageCallback& cb) {
    text_message_callback_ = cb;
  }
  void SetBinaryMessageCallback(const BinaryMessageCallback& cb) {
    binary_message_callback_ = cb;
  }
  void SetCloseMessageCallback(const CloseMessageCallback& cb) {
    close_message_callback_ = cb;
  }
  void SetPingMessageCallback(const PingMessageCallback& cb) {
    ping_message_callback_ = cb;
  }
  void SetPongMessageCallback(const PongMessageCallback& cb) {
    pong_message_callback_ = cb;
  }
  void SetErrorCallback(const ErrorCallback& cb) {
    error_callback_ = cb;
  }

 private:
  void OnParsedFrame(const WebsocketFrame& frame);
  bool OnContinuationFrame(const WebsocketFrame& frame);
  bool OnTextFrame(const WebsocketFrame& frame);
  bool OnBinaryFrame(const WebsocketFrame& frame);
  bool OnCloseFrame(const WebsocketFrame& frame);
  bool OnPingFrame(const WebsocketFrame& frame);
  bool OnPongFrame(const WebsocketFrame& frame);
  bool HandleFrameBody(const WebsocketFrame& frame, std::string& handled_body);

  void OnCodecError(std::string reason);

  WebsocketParser parser_;
  bool is_expected_masking_;
  WebsocketMessage::MessageType cur_message_type_;
  TextMessage text_message_;
  BinaryMessage binary_message_;

  TextMessageCallback text_message_callback_;
  BinaryMessageCallback binary_message_callback_;
  CloseMessageCallback close_message_callback_;
  PingMessageCallback ping_message_callback_;
  PongMessageCallback pong_message_callback_;
  ErrorCallback error_callback_;
};
}

#endif
