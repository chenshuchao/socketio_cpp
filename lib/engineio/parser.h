#ifndef ENGINEIO_PARSER_H
#define ENGINEIO_PARSER_H

#include <string>
#include <vector>
#include "engineio/packet.h"

namespace engineio {
class Parser {
 public:
  static bool DecodePacket(const std::string& data, Packet& packet);

  static bool DecodeBase64Packet(const std::string& data, Packet& packet);

  static bool DecodePayload(const std::string& data, std::vector<Packet>& packets);

  static bool DecodePayloadAsBinary(const std::string& data, std::vector<Packet>& packets);

  static void EncodePacket(const Packet& packet, bool is_bianry, std::string& data);

  static void EncodePayload(std::vector<Packet>& packets, bool is_binary, std::string& data);

  static void EncodePayloadAsBinary(std::vector<Packet>& packets, std::string& data);
};
}

#endif
