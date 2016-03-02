#include <stdlib.h>     /* srand, rand */
#include <time.h>  
#include <string>

#include <muduo/base/StringPiece.h>

inline std::string ConvertToStd(muduo::string s) {
  return std::string(s.data(), s.size());
}

inline muduo::string ConvertToMuduo(std::string s) {
  return muduo::string(s.c_str(), s.size());
}

inline unsigned char GetRandomByte()
{
  srand (time(NULL));
  return rand() % 256;
}
