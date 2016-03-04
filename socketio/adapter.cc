#include "socketio/adapter.h"

#include <bytree/logging.hpp>

#include "socketio/namespace.h"

using namespace std;
using namespace bytree;
using namespace socketio;

void Adapter::Add(const string& id, const string& room) {
  if (sids_.find(id) == sids_.end()) {
    String2Bool m_v;
    sids_[id] = m_v;
  }
  sids_[id][room] = true;

  if (rooms_.find(room) == rooms_.end()) {
    String2Bool r_v;
    rooms_[room] = r_v;
  }
  rooms_[room][id] = true;
}

void Adapter::Del(const string& id, const string& room) {
  if (sids_.find(id) == sids_.end()) {
    String2Bool m_v;
    sids_[id] = m_v;
  }
  if (rooms_.find(room) == rooms_.end()) {
    String2Bool r_v;
    rooms_[room] = r_v;
  }
  sids_[id].erase(room);
  rooms_[room].erase(id);
  if (sids_[id].empty()) {
    sids_.erase(id);
  }
  if (rooms_[room].empty()) {
    rooms_.erase(room);
  }
}

void Adapter::DelAll(const string& id) {
  map<string, String2Bool>::iterator it = sids_.find(id);
  if (it == sids_.end()) return;

  String2Bool::iterator inner_it;
  string room;
  for (inner_it = it->second.begin();
       inner_it != it->second.end();
       inner_it ++) {
    room = inner_it->first;
    Del(id, room);
  }
}

bool Adapter::Broadcast(const string& sender_id,
                        const string& room,
                        const string& event,
                        const string& data) {
  //NamespacePtr nsp = nsp_wptr_.lock();
  LOG(DEBUG) << "Adapter::Broadcast - "
             << "Sender sid: " << sender_id
             << ", room: " << room
             << ", event: " << event
             << ", data: " << data;
  assert(nsp_);
  map<string, String2Bool>::iterator it = rooms_.find(room);
  if (it == rooms_.end()) {
    LOG(ERROR) << "Adapter::Broadcast - Room[" << room << "] not found.";
    return false;
  }

  // TODO check whether sender_id in this;
  for(String2Bool::iterator inner_it = it->second.begin();
      inner_it != it->second.end();
      inner_it ++) {
    if (sender_id == inner_it->first) continue;
    SocketPtr socket = nsp_->GetSocket(inner_it->first);
    if (!socket) {
      LOG(ERROR) << "Adapter::Broadcast - socket not found, sid: " << inner_it->first;
      return false;
    }
    socket->Emit(event, data);
  }
  return true;
}

