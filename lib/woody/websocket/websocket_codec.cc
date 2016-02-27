#include "woody/websocket/websocket_codec.h"

#include <boost/bind.hpp>
#include <muduo/base/Logging.h>

#include "woody/base/base_util.h"
#include "woody/base/endian_util.h"
#include "woody/websocket/websocket_frame.h"

using namespace std;
using namespace woody;

WebsocketCodec::WebsocketCodec()
  : is_expected_masking_(true),
    cur_message_type_(WebsocketMessage::kNoneMessage) {
  parser_.SetParsingCompleteCallback(
      boost::bind(&WebsocketCodec::OnParsedFrame, this, _1));
};

bool WebsocketCodec::OnData(const string& data, size_t& parsed_bytes) {
  LOG_DEBUG << "WebsocketCodec::OnData - "
            << "data :\n" << data;
  if (!parser_.Parse(data, parsed_bytes)) {
    OnCodecError("Parser error");
    return false;
  }
  return true;
}

bool WebsocketCodec::ConvertFrameToString(const WebsocketFrame &frame, string& parsed_string) {
  unsigned char first_byte = 0;
  first_byte |= (frame.GetFin() << 7);
  first_byte |= (frame.GetRsv1() << 6);
  first_byte |= (frame.GetRsv2() << 5);
  first_byte |= (frame.GetRsv3() << 4);
  first_byte |= frame.GetOpcode();
  parsed_string += string((char*)&first_byte, 1);

  unsigned char second_byte = 0;
  second_byte |= (frame.GetMask() << 7);
  uint64_t payload_length = frame.GetPayloadLength();
  uint64_t n_payload_length = HostToNetwork64(payload_length);
  char* n_payload_length_bytes = (char*)&n_payload_length;

  if (payload_length <= 125) {
    second_byte |= n_payload_length_bytes[7];
    parsed_string.append((char*)&second_byte, 1);
  }
  else if (payload_length <= 0xff) {
    second_byte |= 126;
    parsed_string.append((char*)&second_byte, 1);
    parsed_string.append(n_payload_length_bytes+6, 2);
  }
  else if (payload_length <= 0xffffffff) {
    second_byte |= 127;
    parsed_string.append((char*)&second_byte, 1);
    parsed_string.append(n_payload_length_bytes, 8);
  }
  else {
    OnCodecError("Frame too large.");
    return false;
  }

  if (frame.GetMask()) {
    uint32_t n_masking_key = HostToNetwork32(frame.GetMaskingKey());
    char* n_masking_key_bytes = (char*)&n_masking_key;
    parsed_string.append(n_masking_key_bytes, 4);
  }

  parsed_string.append(frame.GetBody());
  return true;
}

bool WebsocketCodec::ConvertMessageToFrame(
    const WebsocketMessage::MessageType type,
    const std::string data,
    WebsocketFrame& frame) {
  int fin = 1;
  int rsv1 = 0;
  int rsv2 = 0;
  int rsv3 = 0;
  int opcode = -1;
  int mask = 0;  // Server must not send any masked frame to client.
  switch (type) {
      case WebsocketMessage::kNoneMessage: {
      return false;
    }
      case WebsocketMessage::kTextMessage: {
      opcode = OPCODE_TEXT;
      break;
    }
      case WebsocketMessage::kBinaryMessage: {
      opcode = OPCODE_BINARY;
      break;
    }
      case WebsocketMessage::kCloseMessage: {
      opcode = OPCODE_CLOSE;
      break;
    }
      case WebsocketMessage::kPingMessage: {
      opcode = OPCODE_PING;
      break;
    }
      case WebsocketMessage::kPongMessage: {
      opcode = OPCODE_PONG;
      break;
    }
    default: {
      assert(false);
    }
  }
  uint32_t masking_key = 0;
  string body = data;
  if (mask && !body.empty()) {
    unsigned char bytes[4];
    for (int i = 0; i < 4; i ++) {
      bytes[i] = GetRandomByte();
    }
    masking_key = NetworkToHost32(*(uint32_t*)&bytes);
    body = WebsocketFrame::Mask(body, masking_key);
  }
  uint64_t payload_length = body.size();

  frame.Reset(fin, rsv1, rsv2, rsv3, opcode,
              mask, payload_length,
              masking_key,
              body);
  return true;

}
void WebsocketCodec::OnParsedFrame(const WebsocketFrame& frame) {
  int opcode = frame.GetOpcode();
  switch (opcode) {
    case OPCODE_CONTINUATION:
        OnContinuationFrame(frame);
        break;
    case OPCODE_TEXT:
        OnTextFrame(frame);
        break;
    case OPCODE_BINARY:
        OnBinaryFrame(frame);
        break;
    case OPCODE_CLOSE:
        OnCloseFrame(frame);
        break;
    case OPCODE_PING:
        OnPingFrame(frame);
        break;
    case OPCODE_PONG:
        OnPongFrame(frame);
        break;
  }
}

bool WebsocketCodec::OnContinuationFrame(const WebsocketFrame& frame) {
  LOG_INFO << "WebsocketCodec::OnContinuationFrame";
  string frame_body;
  if (!HandleFrameBody(frame, frame_body)) {
    return false;
  }
  if (cur_message_type_ == WebsocketMessage::kNoneMessage) {
    OnCodecError("Message not started yet.");
    return false;
  } else if (cur_message_type_ == WebsocketMessage::kTextMessage) {
    text_message_.Append(frame_body);
    if (frame.GetFin() == 1) {
      text_message_callback_(text_message_);
      text_message_.CleanUp();
    }
  } else if (cur_message_type_ == WebsocketMessage::kBinaryMessage) {
    binary_message_.Append(frame_body);
    if (frame.GetFin() == 1) {
      binary_message_callback_(binary_message_);
      binary_message_.CleanUp();
    }
  } else {
    // error
  }

  cur_message_type_ = WebsocketMessage::kNoneMessage;
  return true;
}

bool WebsocketCodec::OnTextFrame(const WebsocketFrame& frame) {
  LOG_INFO << "WebsocketCodec::OnTextFrame";
  string frame_body;
  if (!HandleFrameBody(frame, frame_body)) {
    return false;
  }
  LOG_DEBUG << "WebsocketCodec::OnTextFrame - data: " << frame_body;
  if (cur_message_type_ != WebsocketMessage::kNoneMessage) {
    OnCodecError("Received a new message before completing previous");
    return false;
  }
  cur_message_type_ = WebsocketMessage::kTextMessage;
  text_message_.Append(frame_body);
  if (frame.GetFin() == 1) {
    text_message_callback_(text_message_);
    text_message_.CleanUp();
    cur_message_type_ = WebsocketMessage::kNoneMessage;
  }
  return true;
}

bool WebsocketCodec::OnBinaryFrame(const WebsocketFrame& frame) {
  LOG_INFO << "WebsocketCodec::OnBinaryFrame";
  string frame_body;
  if (!HandleFrameBody(frame, frame_body)) {
    return false;
  }
  LOG_DEBUG << "WebsocketCodec::OnBinaryFrame - data: " << frame_body;
  if (cur_message_type_ != WebsocketMessage::kNoneMessage) {
    OnCodecError("Received a new message before completing previous");
    return false;
  }
  cur_message_type_ = WebsocketMessage::kBinaryMessage;
  binary_message_.Append(frame_body);
  if (frame.GetFin() == 1) {
    binary_message_callback_(binary_message_);
    binary_message_.CleanUp();
    cur_message_type_ = WebsocketMessage::kNoneMessage;
  }
  return true;
}

bool WebsocketCodec::OnCloseFrame(const WebsocketFrame& frame) {
  LOG_INFO << "WebsocketCodec::OnCloseFrame";
  string frame_body;
  if (!HandleFrameBody(frame, frame_body)) {
    return false;
  }
  LOG_DEBUG << "WebsocketCodec::OnCloseFrame - data: " << frame_body;
  CloseMessage m;
  m.Append(frame_body);
  close_message_callback_(m);
  return true;
}

bool WebsocketCodec::OnPingFrame(const WebsocketFrame& frame) {
  LOG_INFO << " WebsocketCodec::OnPingFrame";
  string frame_body;
  if (!HandleFrameBody(frame, frame_body)) {
    return false;
  }
  LOG_DEBUG << "WebsocketCodec::OnPingFrame - data: " << frame_body;
  PingMessage m;
  ping_message_callback_(m);
  return true;
}

bool WebsocketCodec::OnPongFrame(const WebsocketFrame& frame) {
  LOG_INFO << "WebsocketCodec::OnPongFrame";
  string frame_body;
  if (!HandleFrameBody(frame, frame_body)) {
    return false;
  }
  LOG_DEBUG << "WebsocketCodec::OnPongFrame - data: " << frame_body;
  PongMessage message;
  pong_message_callback_(message);
  return true;
}

bool WebsocketCodec::HandleFrameBody(const WebsocketFrame& frame, string& handled_body) {
  string frame_body = frame.GetBody();
  if (!frame_body.empty()) {
    if (frame.GetMaskingKey() && is_expected_masking_) {
      handled_body = frame.UnMask(frame_body, frame.GetMaskingKey());
    }
    else if (frame.GetMaskingKey() && !is_expected_masking_) {
      OnCodecError("Masked when not expected.");
      return false;
    }
    else if (!frame.GetMaskingKey() && is_expected_masking_) {
      OnCodecError("Missing mask.");
      return false;
    }
    else {
      // TODO
    }
  }
  return true;
}

void WebsocketCodec::OnCodecError(string reason) {
  LOG_ERROR << "WebsocketCodec::OnCodecError "
            << "error : " << reason;
  error_callback_();
}

