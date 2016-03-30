#include "socketio/socket.h"

#include <jsoncpp/json/json.h>
#include <bytree/logging.hpp>
#include <bytree/string_util.hpp>

#include "socketio/namespace.h"
#include "socketio/callback.h"
#include "socketio/client.h"
#include "socketio/packet.h"

using namespace std;
using namespace bytree;
using namespace socketio;
/*
string& ArrayStringify(vector<string>& vec, string& res) {
  res.clear();
  res += "[";
  for (int i = 0, size = vec.size(); i < size; i ++) {
    LOG(DEBUG) << "vec:" << i << ", "
               << vec[i] << ", " << vec[i][0];
    if (vec[i].size() && vec[i][0] != '{' && vec[i][0] != '[') {
      res += "\"" + vec[i] + "\"";
    } else {
      res += vec[i];
    }
    if (i != size - 1) res += ",";
  }
  res += "]";
  return res;
}

void EncodePacketBody(const string& event, const string& body, string& result) {
  //ArrayStringify(v, result);
}
*/
Socket::Socket(const ClientPtr& client, const NamespacePtr& nsp)
    : sid_(client->GetSid()),
      client_wptr_(client),
      nsp_wptr_(nsp) {
}

Socket& Socket::On(const std::string& event, const EventCallback& cb) {
  assert(event.size() != 0);
  events_[event] = cb; 
  return *this;
}

void Socket::Emit(const string& event, Json::Value& value) {
  Packet packet;
  // TODO BINARY

  Json::StyledWriter writer;
  Json::Value vec(Json::arrayValue);
  vec.append(event);
  vec.append(value);
  string body = writer.write(vec);
  LOG(DEBUG) << "EcodePacketBody - " << body;

  packet.SetType(Packet::kTypeEvent)
        .SetBody(body);
  NamespacePtr nsp = nsp_wptr_.lock();
  if (!nsp) {
    return;
  }
  /* TODO callback
  int id = nsp->CreateID();
  packet.SetID(id);
  acks_[id] = id;
  */
  SendPacket(packet);
}

void Socket::OnConnect() {
  LOG(DEBUG) << "Socket::OnConnect.";
  Packet packet;
  packet.SetType(Packet::kTypeConnect);
  SendPacket(packet);
}

void Socket::OnPacket(const Packet& packet) {
  switch (packet.GetType()) {
    case Packet::kTypeEvent: {
      OnEventPacket(packet);
      break;
    }
    case Packet::kTypeBinaryEvent: {
      OnEventPacket(packet);
      break;
    }
    case Packet::kTypeAck: {
      OnAckPacket(packet);
      break;
    }
    case Packet::kTypeBinaryAck: {
      OnAckPacket(packet);
      break;
    }
    case Packet::kTypeDisconnect: {
      Close();
      break;
    }
    case Packet::kTypeError: {
      OnError(packet.GetBody());
      break;
    }
  }
}

void Socket::Join(const string& room) {
  NamespacePtr nsp = nsp_wptr_.lock();
  // TODO assert or report error?
  assert(nsp);
  nsp->Join(sid_, room);
}

void Socket::Leave(const string& room) {
  NamespacePtr nsp = nsp_wptr_.lock();
  assert(nsp);
  nsp->Leave(sid_, room);
}

void Socket::Broadcast(const string& room, const string& event, Json::Value& value) {
  NamespacePtr nsp = nsp_wptr_.lock();
  assert(nsp);
  nsp->Broadcast(sid_, room, event, value);
}

void Socket::Close() {
  if (close_callback_) {
    close_callback_();
  }
  if (close_callback_with_this_) {
    close_callback_with_this_(shared_from_this());
  }
  ClientPtr client = client_wptr_.lock();
  if (client) {
    client->Remove(shared_from_this());
  }
}


// socket.broadcast
// nsp.broadcast(emit);
// adaper.broadcast check socket is joined.
bool Socket::OnEventPacket(const Packet& packet) {
  int id = packet.GetID();
  if (id > 0) {
  }
  string body = packet.GetBody();
  LOG(DEBUG) << "Socket::OnEventPacket - Packet body: " << body;
  Json::Reader reader;
  Json::Value root;
  if (!reader.parse(body, root)) {
    LOG(ERROR) << "Socket::OnEventPacket - Json parse error.";
    return false;
  }
  if (!root.isArray()) {
    LOG(ERROR) << "Socket::OnEventPacket - Not an array json.";
    return false;
  }
  if (root.size() < 2) {
    LOG(ERROR) << "Socket::OnEventPacket - Json array size error.";
    return false;
  }
  EventCallback cb = events_[root[0].asString()];
  LOG(DEBUG) << "Socket::OnEventPacket - Event: " << root[0].asString()
             << "data: " << root[1].asString();
  if (!cb) {
    LOG(DEBUG) << "Socket::OnEventPacket - "
               << "No event register, event name: " << root[0].asString() << ".";
    return false;
  }
  cb(shared_from_this(), root[1].asString());
  return true;
}

void Socket::OnAckPacket(const Packet& packet) {
}

void Socket::SendPacket(const Packet& packet) {
  ClientPtr client = client_wptr_.lock();
  if (!client) {
    //
    return;
  }
  client->SendPacket(packet);
}

void Socket::OnError(const string& what) {
  LOG(ERROR) << "Socket::OnError - " << what;
  Close();
}

