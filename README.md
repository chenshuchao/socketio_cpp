# socketio_cpp
[socket.io](https://github.com/socketio/socket.io) server C++版

##示例
```
class SimpleChat {
 public:
  void OnConnection(const SocketPtr& socket) {
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
执行示例

    ./bin/simple ../example/simple
    
浏览器访问 http://127.0.0.1:5011/index.html (请使用 chrome 等高级浏览器）
