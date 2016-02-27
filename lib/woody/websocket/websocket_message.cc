#include "woody/websocket/websocket_message.h"

#include "woody/base/base_util.h"
#include "woody/base/endian_util.h"

using namespace std;
using namespace woody;

void WebsocketMessage::Append(string& data) {
  data_.append(data);
}

void WebsocketMessage::CleanUp() {
  data_.clear();
}
