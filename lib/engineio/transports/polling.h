#ifndef ENGINEIO_TRANSPORTS_POLLING_H
#define ENGINEIO_TRANSPORTS_POLLING_H

#include "engineio/transport.h"

namespace engineio {

class PollingTransport : public BaseTransport {
 public:
  PollingTransport(const woody::HTTPHandlerPtr& handler,
                   const woody::HTTPRequest& req,
                   woody::HTTPResponse& resp)
      : BaseTransport("polling"),
        resp_(resp),
        writable_(false) {
  }
  virtual ~PollingTransport() { }

  virtual void GetAllUpgrades(std::vector<std::string>& vec) {
    vec.push_back("websocket");
  }

  virtual void HandleRequest(const woody::HTTPHandlerPtr& handler,
                             const woody::HTTPRequest& req,
                             woody::HTTPResponse& resp);

  virtual void SendPackets(std::vector<Packet>& packets);

  virtual void DoWrite(const std::string& data) = 0;

  virtual bool IsWritable() const { return writable_; }

  virtual void ForceClose() {
  }

  virtual void OnClose() {
    BaseTransport::OnClose();
  }

  woody::HTTPResponse GetResponse() const {
    return resp_;
  }

  void CleanUp() {
    resp_.CleanUp();
    req_.reset();
  }
 private:
  void HandlePollRequest(const woody::HTTPRequest& req,
                         woody::HTTPResponse& resp);
  void HandleDataRequest(const woody::HTTPRequest& req,
                         woody::HTTPResponse& resp);

  virtual void OnData(const std::string& data);

  woody::HTTPRequestPtr req_;
  woody::HTTPResponse resp_;
  bool writable_;
};

}
#endif
