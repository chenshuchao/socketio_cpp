#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <muduo/base/StringPiece.h>

namespace woody {

inline bool IsStringBeginWith(const std::string& s, const std::string& p) {
  return 0 == s.find(p);
}

inline bool IsStringEndWith(const std::string& s, const std::string& p) {
  return s.find(p) == (s.size() - p.size());
}

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

// TODO
inline void trim(std::string& s) {
}
}

#endif
