#ifndef WOODY_HTTP_HTTPCODEC_H
#define WOODY_HTTP_HTTPCODEC_H

#include <map>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "woody/lib/http_parser/http_parser.h"
#include "woody/http/http_request.h"
#include "woody/http/http_response.h"

namespace woody {
class Buffer;
class HTTPCodec {
 public:
  enum HeaderParseState {
    kParseHeaderNothing,
    kParseHeaderBegin,
    kParseHeaderName,
    kParseHeaderValue,
    kParseHeaderComplete
  };
  typedef uint32_t StreamID;
  typedef boost::function<void (StreamID)> MessageBeginCallback;
  typedef boost::function<void (StreamID, HTTPRequest)> MessageCompleteCallback;
  typedef boost::function<void ()> HeadersCompleteCallback;
  typedef boost::function<void ()> ErrorCallback;

  explicit HTTPCodec();

  static void InitParserSettings();

  std::string GetName() { return name_; }

  StreamID CreateStreamID()
  { return ++cur_stream_id_; }

  size_t OnData(const std::string& data);

  bool OnMessageBegin();

  bool OnUrl(const char* buf, size_t len);

  bool OnHeaderField(const char* buf, size_t len);

  bool OnHeaderValue(const char* buf, size_t len);

  bool OnHeadersComplete();

  bool OnBody(const char* buf, size_t len);

  bool OnMessageComplete();

  /* TODO delete?
  std::string GetHeaderName() { return cur_header_name_; }
  std::string GetHeaderValue() { return cur_header_value_; }
  */

  void SetMessageBeginCallback(const MessageBeginCallback& cb) {
    message_begin_callback_ = cb;
  }

  void SetMessageCompleteCallback(const MessageCompleteCallback& cb) {
    message_complete_callback_ = cb;
  }

  void SetHeadersCompleteCallback(const HeadersCompleteCallback& cb) { 
    headers_complete_callback_ = cb; 
  }

  void SetErrorCallback(const ErrorCallback& cb) {
    error_callback_ = cb;
  }
  
 private:
  HeaderParseState parseState_;
  static int OnMessageBeginCallback(http_parser *parser);
  static int OnUrlCallback(http_parser *parser, const char* buf, size_t len);
  static int OnHeaderFieldCallback(http_parser *parser, const char* buf, size_t len);
  static int OnHeaderValueCallback(http_parser *parser, const char* buf, size_t len);
  static int OnHeadersCompleteCallback(http_parser *parser);
  static int OnBodyCallback(http_parser *parser, const char* buf, size_t len);
  static int OnMessageCompleteCallback(http_parser *parser);
  static int OnChunkHeaderCallback(http_parser *parser);
  static int OnChunkHeaderCompleteCallback(http_parser *parser);

  void OnParseError(const std::string what);

  // TODO parse GET parmas

  std::string name_;
  http_parser parser_;
  bool is_headers_complete_;
  bool is_body_complete_;
  bool is_upgrade_complete_;
  std::string cur_header_value_;
  std::string cur_header_name_;
  StreamID cur_stream_id_;
  HTTPRequest request_;

  MessageBeginCallback message_begin_callback_;
  MessageCompleteCallback message_complete_callback_;
  HeadersCompleteCallback headers_complete_callback_;
  ErrorCallback error_callback_;

  static http_parser_settings kParserSettings;
};
}

#endif
