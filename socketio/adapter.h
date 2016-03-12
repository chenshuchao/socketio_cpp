#ifndef SOCKETIO_ADAPTER_H
#define SOCKETIO_ADAPTER_H

#include <string>
#include <map>

#include <boost/weak_ptr.hpp>
#include <jsoncpp/json/json-forwards.h>

namespace socketio {
class Namespace;
class Adapter {
 typedef std::map<std::string, bool> String2Bool;
 public:

  void SetNamespace(Namespace* nsp) {
    nsp_ = nsp;
  }

  virtual void Add(const std::string& id, const std::string& room);

  virtual void Del(const std::string& id, const std::string& room);

  virtual void DelAll(const std::string& id);

  virtual bool Broadcast(const std::string& sender_id,
                         const std::string& room,
                         const std::string& event,
                         Json::Value& value);

 private:
  Namespace *nsp_;

  // { sid : { room_id, bool }}
  std::map<std::string, String2Bool> sids_;
  // { room_id : { sid, bool }}
  std::map<std::string, String2Bool> rooms_;
};
typedef boost::shared_ptr<Adapter> AdapterPtr;

class AdapterFactory {
 public:
  virtual Adapter* Create() {
    return new Adapter();
  }
};
}

#endif
