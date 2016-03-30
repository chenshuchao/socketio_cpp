#include "socketio/server.h"

#include <boost/bind.hpp>
#include <bytree/logging.hpp>

using namespace std;
using namespace woody;
using namespace socketio;

#include "socketio/client.h"

Server::Server(const string& name)
    : eio_server_(name) { 
  eio_server_.SetConnectionCallback(
      boost::bind(&Server::OnConnection, this, _1));
  Of("/");

  SetPathPrefix("/socket.io");
}

void Server::HandleRequest(const HTTPHandlerPtr& handler,
                           const HTTPRequest& req,
                           HTTPResponse& resp) {
  eio_server_.HandleRequest(handler, req, resp);
}

void Server::Of(string nsp_name) {
  if (0 == nsp_name.size()) {
    LOG(ERROR) << "Server::Of - Namespace name cannot be empty.";
    return;
  }
  if (nsp_name[0] != '/') nsp_name = "/" + nsp_name;
  if (GetNamespace(nsp_name) == NULL) {
    LOG(DEBUG) << "Server::Of - Initializing namespace: " << nsp_name;
    NamespacePtr nsp(new Namespace(nsp_name,
                     adapter_factory_.Create()));
    nsps_[nsp_name] = nsp;
  }
}

NamespacePtr Server::GetNamespace(const string& nsp_name) {
  map<string, NamespacePtr>::const_iterator it = nsps_.find(nsp_name);
  if (it == nsps_.end()) {
    return NamespacePtr();
  }
  return nsps_[nsp_name];
}

void Server::OnConnection(const engineio::SocketPtr& socket) {
  string sid = socket->GetSid();
  LOG(DEBUG) << "Server::OnConnection - " << "Client sid: " << sid;
           
  ClientPtr client(new Client(this, socket));
  client->SetCloseCallback(
     boost::bind(&Server::OnClientClose, this, _1));
  client->SetSocketConnectCallback(socket_connect_callback_);
  String2ClientPtr::iterator it = clients_.find(sid);
  // TODO assert or just log error
  assert(it == clients_.end());
  clients_[sid] = client;
  client->Connect("/");
}

void Server::OnClientClose(const ClientPtr& client) {
  string sid = client->GetSid();
  String2ClientPtr::iterator it = clients_.find(sid);
  // TODO assert or just log error
  assert (it != clients_.end());
  clients_.erase(it);
}

