#include "woody/websocket/websocket_parser.h"

#include <bytree/logging.hpp>

#include "woody/base/endian_util.h"
#include "woody/websocket/websocket_frame.h"

using namespace std;
using namespace bytree;
using namespace woody;

WebsocketParser::WebsocketParser()
    : state_(kParsingHeaderBegin) {
}

bool WebsocketParser::Parse(const string& data, size_t& parsed_bytes) {
  return Parse(data.c_str(), data.size(), parsed_bytes);
}

bool WebsocketParser::Parse(const char* data, size_t len, size_t& parsed_bytes) {
  size_t header_bytes = 0;
  if (state_ == kParsingHeaderBegin) {
    if(len < 1) return parsed_bytes;
    char first_byte = data[0];
    fin_ = (first_byte >> 7) & 1;
    rsv1_ = (first_byte >> 6) & 1;
    rsv2_ = (first_byte >> 5) & 1;
    rsv3_ = (first_byte >> 4) & 1;
    opcode_ = first_byte & 0xf;
    
    if (rsv1_ || rsv2_ || rsv3_) {
      OnParsingError("rsv must be 0x0.");
      return false;
    }
    // control frames between 3 and 7 as well as above 0xA are currently reserved.
    if ((opcode_ > 2 && opcode_ < 8) || opcode_ > 0xA) {
      OnParsingError("use reserved opcode");
      return false;
    }
    if (opcode_ > 0x7 && fin_ == 0){
      OnParsingError("control frames cannot be fragmented.");
      return false;
    }
    header_bytes ++;
    
    // Second byte
    if (len < 2) return parsed_bytes;
    char second_byte = data[1];
    mask_ = (second_byte >> 7) & 1;
    payload_length_ = second_byte & 0x7f;

    //control frames must have a payload length <= 125 bytes.
    if (opcode_ > 127 && payload_length_ > 125) {
      OnParsingError("control frame payload_length too large.");
      return false;
    }
    header_bytes ++;

    if (payload_length_ == 127) {
      // we need 8 bytes
      if (len < header_bytes + 8) return true;
      payload_length_ = NetworkToHostInt64((const unsigned char*)(data+header_bytes));
      header_bytes += 8;
    }
    else if (payload_length_ == 126) {
      // we need 4 bytes
      if (len < header_bytes + 2) return true;
      payload_length_ = NetworkToHostInt16((const unsigned char*)(data+header_bytes));
      header_bytes += 2;
    }

    if (mask_) {
      // parse masking-key, 4 bytes
      if (len < header_bytes + 4) return true;
      masking_key_ = NetworkToHostInt32((const unsigned char*)(data+header_bytes));
      header_bytes += 4;
    }

    parsed_bytes += header_bytes;
    state_ = kParsingBodyBegin;
  }

  size_t body_need_bytes = payload_length_ - body_.size();
  if (body_need_bytes <= len - parsed_bytes) {
    body_.append(data+parsed_bytes, body_need_bytes);
    parsed_bytes += body_need_bytes;
    state_ = kParsingComplete;
    OnParsingComplete();
  }
  else {
    body_.append(data+parsed_bytes, len-parsed_bytes);
    parsed_bytes += len-parsed_bytes;
  }
  return true;
}

void WebsocketParser::OnParsingComplete() {
  WebsocketFrame frame(fin_, rsv1_, rsv2_, rsv3_, opcode_,
                       mask_, payload_length_,
                       masking_key_,
                       body_);

  parsing_complete_callback_(frame);
  CleanUp();
}

void WebsocketParser::OnParsingError(string reason) {
  LOG(ERROR) << "WebsocketParser::OnParsingError -"
             << "error: " << reason;
  //error_callback_();
}

void WebsocketParser::CleanUp() {
  body_.clear();
  state_ = kParsingHeaderBegin;
}

