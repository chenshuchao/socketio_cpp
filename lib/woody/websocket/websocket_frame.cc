#include "woody/websocket/websocket_frame.h"
#include "woody/base/endian_util.h"

using namespace std;
using namespace woody;

string WebsocketFrame::Mask(string data, uint32_t masking_key) {
  unsigned char* bytes = (unsigned char*)data.c_str();
  uint32_t network_masking_key = HostToNetwork32(masking_key);
  unsigned char* keys = (unsigned char*)&network_masking_key;
  size_t len = data.size();
  for (size_t i = 0; i < len; i ++) {
    bytes[i] = bytes[i] ^ keys[i%4];
  }
  return string((char*)bytes, len);
}

string WebsocketFrame::UnMask(string data, uint32_t masking_key) {
  return Mask(data, masking_key);
}

