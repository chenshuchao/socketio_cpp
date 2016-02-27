#include "socketio/namespace.h"

using namespace std;
using namespace woody;
using namespace engineio;
using namespace socketio;

void Namespace::AddSocket(const SocketPtr& socket) {
  sockets_.push_back(socket);
  sockets_map_[socket->GetID()] = socket;
  socket->OnConnect();
}

