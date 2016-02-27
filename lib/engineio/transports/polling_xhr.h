#ifndef ENGINEIO_TRANSPORTS_POLLINGXHR_H
#define ENGINEIO_TRANSPORTS_POLLINGXHR_H

#include "engineio/transports/polling.h"

namespace engineio {
class PollingXhrTransport : public PollingTransport {
 public:
  PollingXhrTransport(const woody::HTTPHandlerPtr& handler,
                      const woody::HTTPRequest& req,
                      woody::HTTPResponse& resp)
      : PollingTransport(handler, req, resp) {
  }
  virtual ~PollingXhrTransport() { }
  virtual void DoWrite(const std::string& data);
};

}

#endif
