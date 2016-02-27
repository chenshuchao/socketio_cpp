#ifndef WOODY_HTTPAPPLICATION_H
#define WOODY_HTTPAPPLICATION_H

#include <boost/shared_ptr.hpp>

namespace woody {
class HTTPHandler;
typedef boost::shared_ptr<HTTPHandler> HTTPHandlerPtr;
class HTTPRequest;
class HTTPResponse;
class HTTPApplication {
 public:
  virtual void HandleRequest(const HTTPHandlerPtr& handler,
                             const HTTPRequest& req,
                             HTTPResponse& resp) = 0;
};
}
#endif
