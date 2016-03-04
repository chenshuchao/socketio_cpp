#ifndef ENGINEIO_BASE_UTIL_H
#define ENGINEIO_BASE_UTIL_H

#include <string>
#include <bytree/ssl_util.hpp>
#include <woody/base/base_util.h>

namespace engineio {
std::string GenerateBase64ID() {
  unsigned char bytes[15];
  for (int i = 0; i < 15; i ++) {
    bytes[i] = GetRandomByte();
  }
  return bytree::Base64Encode((unsigned char*)&bytes, 15);
}
}
#endif
