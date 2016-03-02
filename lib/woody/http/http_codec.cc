#include "woody/http/http_codec.h"

#include <string>

#include <bytree/string_util.hpp>
#include <bytree/logging.hpp>
#include <muduo/net/Buffer.h>

using namespace std;
using namespace bytree;
using namespace woody;

http_parser_settings HTTPCodec::kParserSettings;

HTTPCodec::HTTPCodec()
    : parseState_(kParseHeaderNothing),
      is_headers_complete_(false),
      is_body_complete_(false),
      is_upgrade_complete_(false),
      cur_stream_id_(0) {
  http_parser_init(&parser_, HTTP_REQUEST);
  parser_.data = this;
  InitParserSettings();
}

void HTTPCodec::InitParserSettings() {
  kParserSettings.on_message_begin = HTTPCodec::OnMessageBeginCallback;
  kParserSettings.on_url = HTTPCodec::OnUrlCallback;
  kParserSettings.on_header_field = HTTPCodec::OnHeaderFieldCallback;
  kParserSettings.on_header_value = HTTPCodec::OnHeaderValueCallback;
  kParserSettings.on_headers_complete = HTTPCodec::OnHeadersCompleteCallback;
  kParserSettings.on_body = HTTPCodec::OnBodyCallback;
  kParserSettings.on_message_complete = HTTPCodec::OnMessageCompleteCallback;
// kParserSettings.on_chunk_header = HTTPCodec::OnChunkHeaderCallback;
 // kParserSettings.on_chunk_complete = HTTPCodec::OnChunkCompleteCallback;
}

size_t HTTPCodec::OnData(const string& data) {
  LOG(DEBUG) << "HTTPCodec::OnData [" << name_
             << "], data:\n" << data;
  size_t bytes_parsed = http_parser_execute(&parser_,
                                            &kParserSettings,
                                            data.c_str(),
                                            data.size());
  LOG(DEBUG) << "HTTPCodec::OnData [" << name_
             << "], parse " << bytes_parsed << " bytes.";

  if ((HTTP_PARSER_ERRNO(&parser_) != HPE_OK) &&
      (HTTP_PARSER_ERRNO(&parser_) != HPE_PAUSED)) {
    OnParseError("");
  }
  return bytes_parsed;
}

bool HTTPCodec::OnMessageBegin() {
  parseState_ = kParseHeaderBegin;
  //LOG_INFO << "HTTPCodec::OnMessageBegin [].";
  if (message_begin_callback_) {
    message_begin_callback_(cur_stream_id_);
  }
  return true;
}

bool HTTPCodec::OnUrl(const char* buf, size_t len) {
  //LOG_INFO << "HTTPCodec::OnUrl [" << name_<< "]."
  //         << "len = " << len;
  request_.SetUrl(string(buf, len));
  return true;
}

bool HTTPCodec::OnHeaderField(const char* buf, size_t len) {
 // LOG_INFO << "HTTPCodec::OnHeaderField";
  if (parseState_ == kParseHeaderValue) {
    //LOG_INFO << "HTTPCodec::OnHeadersComplete [" << name_
    //      << "] AddHeader - cur_header_name_: " << cur_header_name_
    //       << ", cur_header_value_: " << cur_header_value_ << ".";
    request_.AddHeader(cur_header_name_, cur_header_value_);
    cur_header_name_.clear();
    cur_header_name_.append(buf, len);
  } else if (parseState_ == kParseHeaderName ||
             parseState_ == kParseHeaderBegin) {
    cur_header_name_.append(buf, len);
  }
  parseState_ = kParseHeaderName;
 // LOG_INFO << "HTTPCodec::OnHeaderField [" << name_
 //          << "] - cur_header_name_: " << cur_header_name_;
  return true;
}

bool HTTPCodec::OnHeaderValue(const char *buf, size_t len) {
  //LOG_INFO << "HTTPCodec::OnHeaderValue";
  if (parseState_ == kParseHeaderName) {
    cur_header_value_.clear();
    cur_header_value_.append(buf, len);
  }
  else if (parseState_ == kParseHeaderValue) {
    cur_header_value_.append(buf, len);
  }
  parseState_ = kParseHeaderValue;
  //LOG_INFO << "HTTPCodec::OnHeaderValue [" << name_
  //       << "] - cur_header_value_: " << cur_header_value_;
  return true;
}

bool HTTPCodec::OnHeadersComplete() {
  //LOG_INFO << "HTTPCodec::OnHeadersComplete [" << name_
  //         << "] AddHeader - cur_header_name_: " << cur_header_name_
  //         << ", cur_header_value_: " << cur_header_value_ << ".";
  request_.AddHeader(cur_header_name_, cur_header_value_);
  is_headers_complete_ = true;
  string method(http_method_str(static_cast<http_method>(parser_.method)));
  request_.SetMethod(method);
  request_.ParseUrl();
  if (parser_.upgrade == 1) {
    request_.SetUpgrade();
  }
  if (headers_complete_callback_) {
    headers_complete_callback_();
  }
  return true;
}

bool HTTPCodec::OnBody(const char* buf, size_t len) {
  request_.SetBody(string(buf, len));
  return true;
}

bool HTTPCodec::OnMessageComplete() {
  //LOG_INFO << "HTTPCodec::OnMessageComplete [" << name_ << "].";
  if (message_complete_callback_) {
    message_complete_callback_(cur_stream_id_, request_);
    request_.CleanUp();
  }
  return true;
}

int HTTPCodec::OnMessageBeginCallback(http_parser *parser) {
  HTTPCodec* codec = static_cast<HTTPCodec*>(parser->data);
  assert (codec != NULL);
  if (!codec->OnMessageBegin()) {
    return 1;
  }
  return 0;
}

int HTTPCodec::OnUrlCallback(http_parser *parser, const char* buf, size_t len) {
  HTTPCodec* codec = static_cast<HTTPCodec*>(parser->data);
  if (!codec->OnUrl(buf, len)) {
    // TODO call OnParseError() or not
    return 1;
  } 
  return 0;
}

int HTTPCodec::OnHeaderFieldCallback(http_parser *parser, const char* buf, size_t len) {
  HTTPCodec* codec = static_cast<HTTPCodec*>(parser->data);
  if (!codec->OnHeaderField(buf, len)) {
    return 1;
  }
  return 0;
}

int HTTPCodec::OnHeaderValueCallback(http_parser *parser, const char* buf, size_t len) {
  HTTPCodec* codec = static_cast<HTTPCodec*>(parser->data);
  if (!codec->OnHeaderValue(buf, len)) {
    return 1;
  }
  return 0;
}

int HTTPCodec::OnHeadersCompleteCallback(http_parser *parser) {
  HTTPCodec* codec = static_cast<HTTPCodec*>(parser->data);
  if (!codec->OnHeadersComplete()) {
    return 1;
  }
  return 0;
}

int HTTPCodec::OnBodyCallback(http_parser *parser, const char* buf, size_t len) {
  HTTPCodec* codec = static_cast<HTTPCodec*>(parser->data);
  if (!codec->OnBody(buf, len)) {
    return 1;
  }
  return 0;
}

int HTTPCodec::OnMessageCompleteCallback(http_parser *parser) {
  HTTPCodec* codec = static_cast<HTTPCodec*>(parser->data);
  if (!codec->OnMessageComplete()) {
    return 1;
  }
  return 0;
}

void HTTPCodec::OnParseError(string what) {
  LOG(ERROR) << "HTTPCodec::OnParseError - "
             << "error: " << what;
  error_callback_();
}

