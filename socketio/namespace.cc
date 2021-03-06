#include "socketio/namespace.h"
#include "socketio/socket.h"

using namespace std;
//using namespace woody;
using namespace socketio;

void Namespace::AddSocket(const SocketPtr& socket) {
  string sid = socket->GetSid();
  map<string, SocketPtr>::const_iterator it = sockets_map_.find(sid);
  assert(it == sockets_map_.end());
  //sockets_.push_back(socket);
  sockets_map_[sid] = socket;
  socket->OnConnect();
}

void Namespace::RemoveSocket(const SocketPtr& socket) {
  string sid = socket->GetSid();
  map<string, SocketPtr>::iterator it = sockets_map_.find(sid);
  if (it != sockets_map_.end()) {
    sockets_map_.erase(it);
  }
}

SocketPtr Namespace::GetSocket(const string& sid) {
  map<string, SocketPtr>::iterator it = sockets_map_.find(sid);
  if (it == sockets_map_.end()) {
    return SocketPtr();
  }
  return sockets_map_[sid];  
}

void Namespace::Join(const string& id, const string& room) {
  adapter_->Add(id, room);
}

void Namespace::Leave(const string& id, const string& room) {
  adapter_->Del(id, room);
}

void Namespace::Broadcast(const string& id,
                          const string& room,
                          const string& event,
                          Json::Value& value) {
  adapter_->Broadcast(id, room, event, value);
}

