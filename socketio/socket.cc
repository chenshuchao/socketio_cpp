#include "socketio/socket.h"

#include <bytree/logging.hpp>
#include <bytree/string_util.hpp>

#include "socketio/namespace.h"
#include "socketio/callback.h"

using namespace std;
using namespace bytree;
using namespace socketio;

void DecodePacketBody(string& body, vector<string>& v) {
  Trim(body);
  LTrim(body, '[');
  RTrim(body, ']');
  Split(body, ",", v);
  for (int i = 0, len = v.size(); i < len; i++) {
    Trim(v[i]);
    Trim(v[i], '\"');
  }
}

void EncodePacketBody(const string& event, const string& body, string& result) {
  result = "[\"" + event + "\",\"" + body + "\"]";
}

Socket::Socket(const ClientPtr& client, const NamespacePtr& nsp)
    : id_(client->GetID()),
      client_wptr_(client),
      nsp_wptr_(nsp) {
}

Socket& Socket::On(const std::string& event, const EventCallback& cb) {
  assert(event.size() != 0);
  events_[event] = cb; 
  return *this;
}

void Socket::Emit(const string& event, const std::string& data) {
  Packet packet;
  // TODO BINARY
  string body;
  EncodePacketBody(event, data, body);
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
      ForceClose();
    }
    case Packet::kTypeError: {
      OnError(packet.GetBody());
    }
  }
}

void Socket::OnEventPacket(const Packet& packet) {
  int id = packet.GetID();
  if (id > 0) {
  }
  string body = packet.GetBody();

  vector<string> v;
  DecodePacketBody(body, v);
  // TODO 
  if (v.size() < 2) return; 
  EventCallback cb = events_[v[0]];
  LOG(DEBUG) << "Socket::OnEventPacket - Event: " << v[0]
             << "data: " << v[1];
  if (!cb) {
    LOG(DEBUG) << "Socket::OnEventPacket - "
               << "No event register, event name: " << v[0] << ".";
    return;
  }
  cb(shared_from_this(), v[1]);
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

void Socket::ForceClose() {
  ClientPtr client = client_wptr_.lock();
  if (!client) {
    //
    return;
  }
  client->ForceClose();
}

void Socket::OnError(const string& what) {
  LOG(ERROR) << "Socket::OnError - " << what;
  ForceClose();
}

