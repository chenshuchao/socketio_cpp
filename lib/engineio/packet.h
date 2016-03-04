#ifndef ENGINEIO_PACKET_H
#define ENGINEIO_PACKET_H

namespace engineio {
class Packet {
 public:
  static const int kPacketOpen = 0;
  static const int kPacketClose = 1;
  static const int kPacketPing = 2;
  static const int kPacketPong = 3;
  static const int kPacketMessage = 4;
  static const int kPacketUpgrade = 5;
  static const int kPacketNoop = 6;
  
  static  bool IsValidPacketType(int type) {
    return type >= 0 && type <= 6;
  }
  // TODO typedef PacketType int
  int GetType() const { return type_; }
  // TODO isValid?
  bool SetType(int type) {
    if (!IsValidPacketType(type)) return false;
    type_ = type;
    return true;
  }

  std::string GetBody() const { return body_; }

  void SetBody(const std::string& body) { body_ = body; }

 private:
  int type_;
  std::string body_;
};

}

#endif
