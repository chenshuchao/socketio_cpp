#include "woody/http/http_response.h"

#include <bytree/string_util.hpp>

#include "woody/http/http_handler.h"

using namespace std;
using namespace bytree;
using namespace woody;

HTTPResponse& HTTPResponse::SetStatus(int code, const string& message) {
  status_code_ = code;
  status_message_ = message;
  return *this;
}

HTTPResponse& HTTPResponse::AddHeader(const string& name, const string& value) {
  headers_map_.insert(pair<string, string>(name, value));
  return *this;
}

HTTPResponse& HTTPResponse::AddBody(const string& body) {
  body_ = body;
  return *this; 
}

string HTTPResponse::GetHeaderValue(const string& name) const {
  map<string, string>::const_iterator it = headers_map_.find(name);
  if (it != headers_map_.end()) {
    return it->second;
  }
  return "";
}

void HTTPResponse::GetAllHeaders(vector<string>& name, vector<string>& value) const {
  name.clear();
  value.clear();
  for (map<string, string>::const_iterator it = headers_map_.begin();
       it != headers_map_.end();
       it ++) {
    name.push_back(it->first);
    value.push_back(it->second);
  }
}

std::string HTTPResponse::ToString() const {
  string first_line = "";
  first_line = "HTTP/1.1 " + IntToString(status_code_) + " " + status_message_;
  
  string headers = "";
  for (map<string, string>::const_iterator it = headers_map_.begin();
       it != headers_map_.end();
       it ++) {
    headers += it->first + ": " + it->second + "\r\n";
  }

  string message = first_line + "\r\n" +
                   headers + "\r\n" +
                   body_;
  return message;
}

void HTTPResponse::End() {
   Format();
   handler_->Send(ToString());
}

void HTTPResponse::CleanUp() {
  status_code_ = 0;
  status_message_.clear();
  headers_map_.clear();
  body_.clear();
}

void HTTPResponse::Format() {
  AddHeader("Content-Length", IntToString(body_.size()));
}

