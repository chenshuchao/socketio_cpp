#ifndef SOCKETIO_NAMESPACE_H
#define SOCKETIO_NAMESPACE_H

#include <vector>
#include <boost/shared_ptr.hpp>

#include "socketio/callback.h"
#include "socketio/client.h"

namespace socketio {
class Namespace {
 public:
  Namespace(const std::string& name) 
      : name_(name),
        id_(0) {
  }

  std::string GetName() const { return name_; }

  int CreateID() { return id_++; };

  void AddSocket(const SocketPtr& socket);

 private:
  const std::string name_;
  int id_;
  std::vector<SocketPtr> sockets_;
  std::map<int, SocketPtr> sockets_map_;
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
