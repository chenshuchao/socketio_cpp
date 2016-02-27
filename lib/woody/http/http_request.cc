#include "woody/http/http_request.h"

#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace woody;

HTTPRequest::HTTPRequest()
    : is_header_finished_(false),
      is_body_finished_(false),
      is_upgrade_(false) {
}

void HTTPRequest::AddHeader(const string name, const string& value) {
  headers_map_.insert(pair<string, string>(name, value));
}

bool HTTPRequest::GetHeader(const string& name, string& value) const {
  map<string, string>::const_iterator it;
  it = headers_map_.find(name);
  if (it != headers_map_.end()) {
    value = it->second;
    return true;
  }
  return false;
}

bool HTTPRequest::GetGETParams(const string& name, string& value) const {
  map<string, string>::const_iterator it;
  it = method_get_params_.find(name);
  if (it != method_get_params_.end()) {
    value = it->second;
    return true;
  }
  return false;
}

void HTTPRequest::SetUrl(const std::string& url) {
  url_ = url;
  return;
}

void HTTPRequest::SetMethod(const string& method) {
  //todo check method
  string temp(method);
  //method_ = boost::to_upper_copy<string>(temp);
  cout << method_ << endl;
  method_ = temp;
  return;
}


void HTTPRequest::ParseUrl() {
  if(url_.empty()) {
    return;
  }

  size_t pos = url_.find('?');
  if(string::npos == pos) {
    return ;
  }
  size_t kleft = 0, kright = 0;
  size_t vleft = 0, vright = 0;
  for(size_t i = pos + 1; i < url_.size();) {
    kleft = i;
    while(i < url_.size() && url_[i] != '=') {
      i++;
    }
    if(i >= url_.size()) {
      break;
    }
    kright = i;
    i++;
    vleft = i;
    while(i < url_.size() && url_[i] != '&' && url_[i] != ' ') {
      i++;
    }
    vright = i;
    method_get_params_[url_.substr(kleft, kright - kleft)] = url_.substr(vleft, vright - vleft);
    i++;
  }
  return;
}

void HTTPRequest::CleanUp() {
  is_header_finished_ = false;
  is_body_finished_ = false;
  is_upgrade_ = false;
  method_.clear();
  url_.clear();
  version_.clear();
  headers_map_.clear();
  method_get_params_.clear();
}


