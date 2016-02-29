#include "socketio/client.h"

#include <bytree/logging.hpp>
#include "socketio/server.h"
#include "socketio/namespace.h"

using namespace std;
using namespace socketio;

Client::Client(Server* server, const engineio::SocketPtr& socket)
    : server_(server),
      eio_socket_(socket) {
  eio_socket_->SetMessageCallback(
      boost::bind(&Client::OnMessage, this, _1, _2));
  eio_socket_->SetPingCallback(boost::bind(&Client::OnPing, this, _1, _2));
  eio_socket_->SetCloseCallback(boost::bind(&Client::OnClose, this));
}

bool Client::Connect(const string& nsp_name) {
  LOG(DEBUG) << "Client::Connect - nsp: " << nsp_name;
 
  NamespacePtr nsp = server_->GetNamespace(nsp_name);
  if (!nsp) {
    Packet packet;
    packet.SetType(Packet::kTypeError)
          .SetNamespace(nsp_name)
          .SetBody("Invalid namespace");
    SendPacket(packet);
    return false;
  }
  SocketPtr socket(new Socket(shared_from_this(), nsp));
  // TODO return bool
  nsp->AddSocket(socket);
  sockets_[nsp_name] = socket;
  socket_connect_callback_(socket);
  return true;
}

void Client::SendPacket(const Packet& packet) {
  string data;
  Parser::Encode(packet, data);
  LOG(DEBUG) << "Client::SendPacket - type: " << packet.GetType()
             << ", data: " << packet.GetBody();
  Send(data);
}

void Client::ForceClose() {
  eio_socket_->ForceClose();
}

void Client::OnData(const string& data) {
  LOG(DEBUG) << "Client::OnData.";
  Packet packet;
  Parser::Decode(data, packet);
  if (packet.GetType() == Packet::kTypeConnect) {
    Connect(packet.GetNamespace());
  } else {
    map<string, SocketPtr>::const_iterator it =
        sockets_.find(packet.GetNamespace());
    if (it == sockets_.end()) {
      // error
      return;
    }
    it->second->OnPacket(packet);
  }
}

void Client::OnMessage(const engineio::SocketPtr& socket, const string& data) {
  OnData(data);
}

void Client::OnPing(const engineio::SocketPtr& socket, const string& data) {
}

void Client::OnClose() {
  close_callback_(shared_from_this());
}

