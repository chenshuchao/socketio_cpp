

namespace socketio {
class Namespace {
 public:
  void Add(SocketIOClientPtr client) {
    EngineIOSocket socket(client);
    
  }
 private:
  SocketIOClient clients_;

};
}
