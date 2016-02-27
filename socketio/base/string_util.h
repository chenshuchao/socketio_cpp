#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <muduo/base/StringPiece.h>

namespace socketio {
inline std::string convert_to_std(muduo::string s) {
  return std::string(s.data(), s.size());
}

inline muduo::string convert_to_muduo(std::string s) {
  return muduo::string(s.c_str(), s.size());
}

inline std::string int_to_string(int a) {
  return boost::lexical_cast<std::string>(a);
}

inline int string_to_int(std::string s) {
  return boost::lexical_cast<int>(s);
}

inline void split(std::string s,
                  std::string pattern,
                  std::vector<std::string>& res) {
  boost::algorithm::split(res, s, boost::algorithm::is_any_of(pattern));
}

inline std::string& LTrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

inline std::string& RTrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

inline std::string& Trim(std::string &s) {
  return LTrim(RTrim(s));
}

inline std::string& LTrim(std::string & s, char x) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::bind2nd(std::equal_to<char>(), x))));
  return s;
}

inline std::string& RTrim(std::string & s, char x) {
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::bind2nd(std::equal_to<char>(), x))).base(), s.end());
  return s;
}

inline std::string& Trim(std::string &s, char x) {
  return LTrim(RTrim(s, x), x);
}
}

#endif
