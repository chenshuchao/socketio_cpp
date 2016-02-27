#ifndef ENGINEIO_BASE_UTIL_H
#define ENGINEIO_BASE_UTIL_H

#include <string>
#include <woody/base/ssl_util.h>
#include <woody/base/base_util.h>

namespace engineio {
std::string GenerateBase64ID() {
  unsigned char bytes[15];
  for (int i = 0; i < 15; i ++) {
    bytes[i] = GetRandomByte();
  }
  return base64_encode((unsigned char*)&bytes, 15);
}
}
#endif
