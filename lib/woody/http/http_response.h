#ifndef WOODY_HTTP_HTTPRESPONSE_H
#define WOODY_HTTP_HTTPRESPONSE_H

#include <string>
#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

namespace woody {
class HTTPHandler;
typedef boost::shared_ptr<HTTPHandler> HTTPHandlerPtr;
class HTTPResponse {
 public:
  HTTPResponse(HTTPHandlerPtr handler) : handler_(handler) {
  }
  HTTPResponse& SetStatus(int code, const std::string& message);
  HTTPResponse& AddHeader(const std::string& name, const std::string& value);
  HTTPResponse& AddBody(const std::string& body);

  int GetStatusCode() const { return status_code_; }

  std::string GetStatusMessage() const { return status_message_; }

  std::string GetHeaderValue(const std::string& name) const;

  void GetAllHeaders(std::vector<std::string>& name, std::vector<std::string>& value) const;

  std::string GetBody() const { return body_; }

  std::string ToString() const;

  void End();

  void CleanUp();

 private:
  // TODO
  void Format();

  HTTPHandlerPtr handler_;
  int status_code_;
  std::string status_message_;
  std::map<std::string, std::string> headers_map_;
  std::string body_;
};
typedef boost::shared_ptr<HTTPResponse> HTTPResponsePtr;
}

#endif
