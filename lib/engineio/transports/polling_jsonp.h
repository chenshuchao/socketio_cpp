#ifndef ENGINEIO_TRANSPORTS_POLLINGJSONP_H
#define ENGINEIO_TRANSPORTS_POLLINGJSONP_H

namespace engineio {

class PollingJsonpTransport : public PollingTransport {
 public:
  PollingJsonpTransport(const woody::HTTPHandlerPtr& handler,
                        const woody::HTTPRequest& req,
                        woody::HTTPResponse& resp)
      : PollingTransport(handler, req, resp) {
  }
  virtual ~PollingJsonpTransport() { }
  virtual void DoWrite(const std::string& data) { }

};
}
#endif
