#ifndef SOCKETIO_PACKET_H
#define SOCKETIO_PACKET_H

#include <string>

namespace socketio {
class Packet {
 public:
  static const int kTypeConnect = 0;
  static const int kTypeDisconnect = 1;
  static const int kTypeEvent = 2;
  static const int kTypeAck = 3;
  static const int kTypeError = 4;
  static const int kTypeBinaryEvent = 5;
  static const int kTypeBinaryAck = 6;

  Packet() : id_(-1) {
  }

  int GetType() const { return type_; }

  Packet& SetType(int type) {
    type_ = type;
    return *this;
  }

  int GetID() const { return id_; }

  Packet& SetID(int id) {
    id_ = id;
    return *this;
  }

  std::string GetNamespace() const { return nsp_; }

  Packet& SetNamespace(const std::string& nsp_name) {
    nsp_ = nsp_name;
    return *this;
  }

  std::string GetBody() const { return body_; }

  Packet& SetBody(const std::string& data) {
    body_ = data;
    return *this;
  }

  Packet& Append(const std::string& data) {
    body_.append(data);
    return *this;
  }

 private:
  int type_;
  int id_;
  std::string nsp_;
  std::string body_;
};
}

#endif
