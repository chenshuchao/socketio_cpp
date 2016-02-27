#ifndef SOCKETIO_PARSER_H
#define SOCKETIO_PARSER_H

#include <string>

#include "socketio/packet.h"

namespace socketio {
class Parser {
 public:
  static bool Decode(const std::string& data, Packet& packet);
  static bool Encode(const Packet& packet, std::string& data);
 private:
  static bool DecodeString(const std::string& data, Packet& packet);
  static bool EncodeAsString(const Packet& packet, std::string& data);
  static bool EncodeAsBinary(const Packet& packet, std::string& data);
};
}

#endif
