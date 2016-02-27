#ifndef ENGINEIO_TRANSPORTS_TRANSPORTFACTORY_H
#define ENGINEIO_TRANSPORTS_TRANSPORTFACTORY_H

#include <boost/pointer_cast.hpp>
#include "engineio/transports/websocket.h"
#include "engineio/transports/polling_xhr.h"
#include "engineio/transports/polling_jsonp.h"

namespace engineio {
class BaseTransportFactory {
 public:
  virtual BaseTransport* Create(const woody::HTTPHandlerPtr& handler,
                                const woody::HTTPRequest& req,
                                woody::HTTPResponse& resp) = 0;
};
typedef boost::shared_ptr<BaseTransportFactory> BaseTransportFactoryPtr;

class WebsocketTransportFactory : public BaseTransportFactory {
 public:
  WebsocketTransportFactory() : BaseTransportFactory() { }
  virtual ~WebsocketTransportFactory() { }
  WebsocketTransport* Create(const woody::HTTPHandlerPtr& handler,
                             const woody::HTTPRequest& req,
                             woody::HTTPResponse& resp) {
    return new WebsocketTransport(handler, req, resp);
  }
};
typedef boost::shared_ptr<WebsocketTransportFactory>
    WebsocketTransportFactoryPtr;

class PollingTransportFactory : public BaseTransportFactory {
 public:
  PollingTransportFactory() : BaseTransportFactory() { }
  virtual ~PollingTransportFactory() { }
  PollingTransport* Create(const woody::HTTPHandlerPtr& handler,
                           const woody::HTTPRequest& req,
                           woody::HTTPResponse& resp) {
    std::string j;
    if (req.GetGETParams("j", j)) {
      return boost::dynamic_pointer_cast<PollingTransport>(
          new PollingJsonpTransport(handler, req, resp));
    } else {
      return boost::dynamic_pointer_cast<PollingTransport>(
          new PollingXhrTransport(handler, req, resp));
    }
  }
};
typedef boost::shared_ptr<PollingTransportFactory> PollingTransportFactoryPtr;

class Transports {
 public:
  Transports() {
    factories_["polling"] = PollingTransportFactoryPtr(
        new PollingTransportFactory());
    factories_["websocket"] = WebsocketTransportFactoryPtr(
        new WebsocketTransportFactory());
  }
  bool IsValid(std::string transport_name) {
    return factories_.find(transport_name) != factories_.end();
  }
  BaseTransportFactoryPtr GetFactory(std::string transport_name) {
    if (!IsValid(transport_name)) return BaseTransportFactoryPtr();
    return factories_[transport_name];
  }
 private:
  std::map<std::string, BaseTransportFactoryPtr> factories_;
};
typedef boost::shared_ptr<Transports> TransportsPtr;
}

#endif
