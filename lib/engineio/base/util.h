#ifndef ENGINEIO_BASE_UTIL_H
#define ENGINEIO_BASE_UTIL_H

#include <string>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <bytree/ssl_util.hpp>

namespace engineio {
std::string GenerateBase64ID() {
  std::string chars(
     "abcdefghijklmnopqrstuvwxyz"
     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
     "1234567890"
     "!@#$%^&*()"
     "`~-_=+[{]}\\|;:'\",<.>/? ");
  boost::random::random_device rng;
  boost::random::uniform_int_distribution<> index_dist(0, chars.size() - 1); 
  char bytes[15];
  for(unsigned int i = 0; i < 15; ++i) {
    bytes[i] = chars[index_dist(rng)];
  }
  std::string b64 = bytree::Base64Encode((unsigned char*)&bytes, 15);
  // TODO
  /*
  Replace(b64, "/", "_");
  Replace(b64, "+", "-");
  */
  for (unsigned int i = 0, len = b64.size(); i < len; i ++) {
    if (b64[i] == '/') {
      b64[i] = '_';
      continue;
    }
    if (b64[i] == '+') b64[i] = '-';
  }
  return b64;
}
}
#endif
