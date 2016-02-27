#include <string>

std::string base64_encode(const std::string& message);
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
std::string base64_decode(std::string const& encoded_string);

std::string sha1(std::string buf);
