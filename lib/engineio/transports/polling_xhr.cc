#include "engineio/transports/polling_xhr.h"
#include <bytree/logging.hpp>

using namespace std;
using namespace woody;
using namespace engineio;

void PollingXhrTransport::DoWrite(const string& data) {
  LOG(DEBUG) << "PollingXhrTransport::DoWrite - data: " << data;
  HTTPResponse resp = PollingTransport::GetResponse();
  resp.SetStatus(200, "OK");
  if (IsSupportBinary()) {
    resp.AddHeader("Content-Type", "application/octet-stream");
  } else {
    resp.AddHeader("Content-Type", "text/plain; charset=UTF-8");
  }
  resp.AddBody(data);
  resp.End();
  CleanUp();
}

