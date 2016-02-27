#ifndef WOODY_HTTP_HTTPREQUEST_H
#define WOODY_HTTP_HTTPREQUEST_H

#include <map>
#include <boost/shared_ptr.hpp>

namespace woody {
class HTTPRequest {
 public:
  HTTPRequest();
 
  void AddHeader(const std::string name, const std::string& value);

  // TODO return boolean
  bool GetHeader(const std::string& name, std::string& value) const;
  bool GetGETParams(const std::string& name, std::string& value) const;

  void ParseUrl();

  void SetMethod(const std::string& method);

  std::string GetMethod() const { return method_; }

  void SetUrl(const std::string& url);

  std::string GetUrl() const { return url_; }

  void SetBody(const std::string& body) { body_ = body; }

  std::string GetBody() const { return body_; }

  bool IsUpgrade() const { return is_upgrade_; }

  void SetUpgrade() { is_upgrade_ = true; }

  void CleanUp();

 private:
  bool is_header_finished_;
  bool is_body_finished_;
  bool is_upgrade_;

  std::string method_;
  std::string url_;
  std::string version_;
  std::map<std::string, std::string> headers_map_;
  std::map<std::string, std::string> method_get_params_;
  std::string body_;
};

typedef boost::shared_ptr<HTTPRequest> HTTPRequestPtr;
}

#endif
