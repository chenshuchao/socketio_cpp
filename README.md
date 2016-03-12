# socketio_cpp
[socket.io](https://github.com/socketio/socket.io) server C++版

##示例
```
class SimpleChat {
 public:
  void OnConnection(const SocketPtr& socket) {
    // Bind Event listener.
    socket->On("chat message",
               boost::bind(&SimpleChat::OnChatMessage, this, _1, _2));
  }
 private:
  void OnChatMessage(const SocketPtr& socket, const string& data) {
    socket->Emit("chat message", "From server: " + data);
  }
};

//...
SimpleChat simple_chat;
Server sio_server("SocketIOServer");
sio_server.SetSocketConnectCallback(
    boost::bind(&SimpleChat::OnConnection, &simple_chat, _1));

```
[更多示例](https://github.com/chenshuchao/socketio_cpp/tree/master/example)


##安装

安装编译
```
git clone https://github.com/chenshuchao/socketio_cpp.git
cd socketio_cpp
mkdir build
cd build
cmake ..
make
```

