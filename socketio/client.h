
namespace socketio {
class SocketIOClient {
 public:
 
 void Connect(const std::string& nsp_name) {
   // 
   nsp = server.FindNamespace(nsp_name);
   SocketIOSocketPtr socket = nsp.add(shared_from_this());
   
   sockets_[nsp_name] = socket;
 }
 private:
  engineio::EngineIOSocketPtr engineio_socket_;
  std::map<std::string, SocketIOSocketPtr> sockets_;
};
}
