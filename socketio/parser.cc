#include "socketio/parser.h"

#include <bytree/string_util.hpp>
#include "socketio/packet.h"

using namespace std;
using namespace bytree;
using namespace socketio;

bool Parser::Decode(const string& data, Packet& packet) {
  return DecodeString(data, packet);
}

bool Parser::DecodeString(const string& data, Packet& packet) {
  int size = data.size();
  if (size == 0) return false;
  int i = 0;
  int type = data[0] - '0';
  packet.SetType(type);
  i ++;
  
  if (type == Packet::kTypeBinaryEvent ||
      type == Packet::kTypeBinaryAck) {
    int j = i;
    while(j < size && data[j] != '-') j ++;
    if (j == size) {
      // error 
      return false;
    }
    string attachments(data, i, j-i);
    // TODO
    i = j + 1;
  }

  if (i >= size) {
    // error
  }

  if ('/' == data[i]) {
    int j = i + 1;
    while (j < size && data[j] != ',') j ++;
    packet.SetNamespace(string(data, i, j-i));
    i = j;
  } else {
    packet.SetNamespace("/");
  }
  
  if (i < size && isdigit(data[i])) {
    int j = i+1;
    while(j < size && isdigit(data[j])) j++;
    string str_id(data, i, j-i);
    // TODO
    int id = StringToInt(str_id);
    packet.SetID(id);
    i = j;
  }

  string body(data, i);
  packet.SetBody(body);
  return true;
}

bool Parser::Encode(const Packet& packet, string& data) {
  int type = packet.GetType();
  if (type == Packet::kTypeBinaryEvent ||
      type == Packet::kTypeBinaryAck) {
    return EncodeAsBinary(packet, data);
  } else {
    return EncodeAsString(packet, data);
  }
}

bool Parser::EncodeAsString(const Packet& packet, string& data) {
  int type = packet.GetType();
  data.append(IntToString(type));

  string nsp = packet.GetNamespace();
  bool has_nsp = false;
  if (!nsp.empty() && "/" != nsp) {
    data.append(nsp);
    has_nsp = true;
  }
  
  int id = packet.GetID();
  if (id > 0) {
    if (has_nsp) {
      data.append(",");
      has_nsp = false;
    }
    data.append(IntToString(id));
  }

  string body = packet.GetBody();
  if (!body.empty()) {
    if (has_nsp) data.append(",");
    data.append(body);
  }
  return true;
}

bool Parser::EncodeAsBinary(const Packet& packet, string& data) {
  return true;
}

