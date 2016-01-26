#ifndef SOCKETIO_SERVER_H
#define SOCKETIO_SERVER_H

namespace socketio {
class SocketIOServer {
 public:
  
  void AddListener(std::string& event, const EventCallback& cb);
  void OnMessage(const engineio::EngineIOSocketPtr socket,
                 const std::string& data);
 private:
  std::map<std::string, NamespacePtr> nsps_;
  std::map<engineio::EngineIOSocketPtr, SocketIOClient> clients_;

};
}
#endif
