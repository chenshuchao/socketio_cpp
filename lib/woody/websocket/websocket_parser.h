#ifndef WOODY_WEBSOCKET_WEBSOCKETPARSER_H
#define WOODY_WEBSOCKET_WEBSOCKETPARSER_H

#include <string>
#include <boost/function.hpp>

namespace woody {
class WebsocketFrame;
class WebsocketParser {
 public:
  typedef boost::function<void (const WebsocketFrame&)> ParsingCompleteCallback;
  typedef boost::function<void ()> ErrorCallback;
  enum ParsingState {
    kParsingHeaderBegin,
    kParsingBodyBegin,
    kParsingComplete,
  };

  WebsocketParser();
  bool Parse(const std::string& data, size_t& parsed_bytes);
  bool Parse(const char* data, size_t len, size_t& parsed_bytes);

  void SetParsingCompleteCallback(const ParsingCompleteCallback& cb) {
    parsing_complete_callback_ = cb;
  }
  void OnParsingComplete();
  void OnParsingError(std::string reason);
  void CleanUp();

 private:
  ParsingState state_; 
  int fin_;
  int rsv1_;
  int rsv2_;
  int rsv3_;
  int opcode_;
  int mask_;
  uint64_t payload_length_;
  uint32_t masking_key_;
  std::string body_;

  ParsingCompleteCallback parsing_complete_callback_;
  ErrorCallback error_callback_;
};
}

#endif
