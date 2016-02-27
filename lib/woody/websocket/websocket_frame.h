#ifndef WOODY_WEBSOCKET_WEBSOCKETFRAME_H
#define WOODY_WEBSOCKET_WEBSOCKETFRAME_H

#include <stdint.h>
#include <string>

namespace woody {
const int OPCODE_CONTINUATION = 0x0;
const int OPCODE_TEXT = 0x1;
const int OPCODE_BINARY = 0x2;
const int OPCODE_CLOSE = 0x8;
const int OPCODE_PING = 0x9;
const int OPCODE_PONG = 0xa;

class WebsocketFrame {
 public:
  WebsocketFrame() { }
  WebsocketFrame(int fin,
                 int rsv1,
                 int rsv2,
                 int rsv3,
                 int opcode, 
                 int mask,
                 uint64_t payload_length,
                 uint32_t masking_key,
                 const std::string& body)
    : fin_(fin),
      rsv1_(rsv1),
      rsv2_(rsv2),
      rsv3_(rsv3),
      opcode_(opcode),
      mask_(mask),
      payload_length_(payload_length),
      masking_key_(masking_key),
      body_(body) {
  }

  static std::string Mask(std::string data, uint32_t masking_key);
  static std::string UnMask(std::string data, uint32_t masking_key);
  int GetFin() const { return fin_; }
  int GetRsv1() const { return rsv1_; }
  int GetRsv2() const { return rsv2_; }
  int GetRsv3() const { return rsv3_; }
  int GetOpcode() const { return opcode_; }
  int GetMask() const { return mask_; }
  uint64_t GetPayloadLength() const { return payload_length_; }
  uint32_t GetMaskingKey() const { return masking_key_; }
  std::string GetBody() const { return body_; }

  void Reset(int fin, int rsv1, int rsv2, int rsv3, int opcode,
             int mask, uint64_t payload_length,
             uint32_t masking_key,
             std::string& body) {
    fin_ = fin;
    rsv1_ = rsv1;
    rsv2_ = rsv2;
    rsv3_ = rsv3;
    opcode_ = opcode;
    mask_ = mask;
    payload_length_ = payload_length;
    masking_key_ = masking_key;
    body_ = body;
  }
             
 private:
  int fin_;
  int rsv1_;
  int rsv2_;
  int rsv3_;
  int opcode_;
  int mask_;
  uint64_t payload_length_;
  uint32_t masking_key_;
  std::string body_;
};
}

#endif
