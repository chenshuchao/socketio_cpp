#ifndef SOCKETIO_NAMESPACE_H
#define SOCKETIO_NAMESPACE_H

#include <vector>
#include <boost/shared_ptr.hpp>

#include "socketio/callback.h"
#include "socketio/adapter.h"

namespace socketio {
class Namespace {
 public:
  Namespace(const std::string& name, Adapter* adapter) 
      : name_(name),
        id_(0),
        adapter_(adapter) {
    adapter_->SetNamespace(this);
  }

  std::string GetName() const { return name_; }

  int CreateID() { return id_++; };

  void AddSocket(const SocketPtr& socket);

  void RemoveSocket(const SocketPtr& socket);

  SocketPtr GetSocket(const std::string& sid);

  void Join(const std::string& id, const std::string& room);

  void Leave(const std::string& id, const std::string& room);

  void Broadcast(const std::string& id,
                 const std::string& room,
                 const std::string& event,
                 Json::Value& value);

 private:
  const std::string name_;
  int id_;
  //std::vector<SocketPtr> sockets_;

  std::map<std::string, SocketPtr> sockets_map_;
  AdapterPtr adapter_;
};
typedef boost::shared_ptr<Namespace> NamespacePtr;
}

/*
class NamespaceManager {
 public:
  NamespaceManager() {
    Add("/", NamespacePtr(new Namespace("/")));
  }
  void Add(const std::string& name, const NamespacePtr& nsp) {
    nsps_[name] = nsp;
  }
  NamespacePtr GetNamespace(const std::string& name) {
    std::map<std::string, NamespacePtr>::const_iterator it
        = nsps_.find(name);
    if (it == nsps_.end()) {
      return NamespacePtr();
    }
    return nsps_[name];
  }
  void Remove(const std::string& name) {
    nsps_.erase(name);
  }
 private:
  std::map<std::string, NamespacePtr> nsps_;
};
typedef boost::shared_ptr<NamespaceManager> NamespaceManagerPtr;

*/

#endif
