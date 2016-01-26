

namespace socketio {
class SocketIOSocket {
 public:
  SocketIOSocket(clientPtr, nspPtr) {
  
  }
 private:
  boost::weak_ptr<SocketIOClient> client_;
  boost::weak_ptr<Namespace> nsp_;

};

}
