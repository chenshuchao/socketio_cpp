#include "engineio/parser.h"

#include <bytree/logging.hpp>
#include <bytree/string_util.hpp>
#include <bytree/ssl_util.hpp>

using namespace std;
using namespace bytree;
using namespace engineio;

int BinaryStringToInt(string& s) {
  int sum = 0;
  for (unsigned int i = 0, size = s.size(); i < size; i ++) {
    int t = static_cast<int>(static_cast<unsigned char>(s[i]));
    sum = sum * 10 + t;
  }
  return sum;
}

bool Parser::DecodePacket(const string& data, Packet& packet) {
  if (!data.size()) return true;

  if (data[0] == 'b') {
    return DecodeBase64Packet(data.substr(1), packet);
  }

  int type;
  if (data[0] > '0') {
    type = data[0] - '0';
  } else {
    type = data[0];
  }
  // TODO  UTF-8
  if (!Packet::IsValidPacketType(type)) {
    // error
    return false;
  }
  packet.SetType(type);
  if (data.size() > 1) {
    packet.SetBody(data.substr(1));
  }
  return true;
}

// TODO
bool Parser::DecodeBase64Packet(const string& data, Packet& packet) {
  if (0 == data.size()) return false;
  packet.SetType(data[0]-'0');
  string decoded = Base64Decode(data.substr(1));
  packet.SetBody(decoded);
  return true;
}

bool Parser::DecodePayload(const string& data, vector<Packet>& packets) {
  if (!(data[0] == 'b' || data[0] > '0')) {
    return DecodePayloadAsBinary(data, packets);
  }
  int i = 0, j = 0;
  int size = data.size();
  string len_str;
  while(i < size) {
    if (data[i] != ':') {
      i ++;
    } else {
      len_str = string(data, j, i-j);
      if (len_str.empty()) {
        LOG(ERROR) << "Parser::DecodePayload - Packet length can't be empty.";
        return false;
      }
      unsigned int len = BinaryStringToInt(len_str);
      string msg(data, i+1, len);
      if (msg.size() != len) {
        LOG(ERROR) << "Parser::DecodePayload - Packet length conflict.";
        return false;
      }
      if (!msg.empty()) {
        Packet packet;
        if (!DecodePacket(msg, packet)) {
          LOG(ERROR) << "Parser::DecodePayload - DecodePacket Error.";
          return false;
        }
        packets.push_back(packet);
      }
      i += len + 1;
      j = i;
    }
  }
  if (i != j) {
    LOG(ERROR) << "Parser::DecodePayload - Data is incomplete.";
    return false;
  }
  return true;
}

bool Parser::DecodePayloadAsBinary(const string& data, vector<Packet>& packets) {
  unsigned int i = 0, j = 0;
  unsigned int size = data.size();
  string len_str;

  LOG(DEBUG) << "Parser::DecodePayloadAsBinary - i: " << i << ", size: " << size;
  for (unsigned int k = 0; k < size; k ++) {
    LOG(DEBUG) << "data[" << k << "]: " << data[k] << ", ";
  }
  while(i < size) {
    LOG(DEBUG) << "Parser::DecodePayloadAsBinary - In while.";
    if ((unsigned char)data[i] != 255) {
      i ++;
    } else {
      len_str = string(data, j+1, i-j-1);
      // 310 = char length of Number.MAX_VALUE
      if (len_str.size() > 310) {
        LOG(ERROR) << "Parser::DecodePayloadAsBinary - Packet length too long.";
        return false;
      }
      unsigned int len = BinaryStringToInt(len_str);

      string single(data, i+1, len);
      if (single.size() != len) {
        LOG(ERROR) << "Parser::DecodePayloadAsBinary - Packet length conflict.";
        return false;
      }
     
      Packet packet;
      DecodePacket(single, packet);
      LOG(DEBUG) << "Packet type: " << packet.GetType() << ", body: " << packet.GetBody();
      packets.push_back(packet);
      i += len + 1;
      j = i;
    } 
  }

  if (i != j) {
    LOG(ERROR) << "Parser::DecodePayloadAsBinary - Data is incomplete.";
    return false;
  }
  return true;
}

void Parser::EncodePacket(const Packet& packet, bool support_binary, string& result) {
  int type = packet.GetType();
  result.append(IntToString(type));
  result.append(packet.GetBody()); 
}

void Parser::EncodePayload(vector<Packet>& packets, bool support_binary, string& result) {
  if (support_binary) {
    EncodePayloadAsBinary(packets, result);
    return;
  }
  for (vector<Packet>::iterator it = packets.begin();
       it != packets.end();
       it ++) {
    string r;
    EncodePacket(*it, false, r);
    result += IntToString(r.size()) + ":" + r;
  }
}

void Parser::EncodePayloadAsBinary(vector<Packet>& packets, string& result) {
  for (vector<Packet>::iterator it = packets.begin();
       it != packets.end();
       it ++) {
    string d;
    EncodePacket(*it, true, d);
    string encoding_len = IntToString(d.size());
    int buf_size = encoding_len.size() + 2;
    string buf(buf_size, '0');
    buf[0] = 0;
    for (unsigned int i = 0; i < encoding_len.size(); i ++) {
      buf[i+1] = encoding_len[i] - '0';
    }
    buf[buf_size - 1] = 255;
    result = buf + d;
  }
}

