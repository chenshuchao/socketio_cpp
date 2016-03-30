# socketio_cpp
[socket.io] server C++版

##使用

### 依赖软件

g++ (version >= 4.4 is recommended);
cmake (version >= 2.6 is recommended);
boost (version >= 1.59 is recommended);

###下载编译
```
git clone https://github.com/chenshuchao/socketio_cpp.git
cd socketio_cpp
bash ./build.sh
```

###执行示例
```
cd build/release
./bin/echo ../../example/echo
```
浏览器访问 http://127.0.0.1:5011/index.html (请使用 chrome 等高级浏览器）

###安装项目
```
cd bulid/release
make install
```
在build 目录下生成 release-install 目录。

##示例
```
// 自定义应用类
class SimpleChat {
 public:
  void OnConnection(const SocketPtr& socket) {
    socket->On("chat message",
               boost::bind(&SimpleChat::OnChatMessage, this, _1, _2));
  }
 private:
  void OnChatMessage(const SocketPtr& socket, const string& data) {
    Json::Value v(Json::stringValue);
    v = "From sever: " + data;
    socket->Emit("chat message", v); 
  }
};

int main(int argc, char** argv) {
  ...
  SimpleChat simple_chat;
  Server sio_server("SocketIOServer");
  // 绑定连接回调函数
  sio_server.SetSocketConnectCallback(
    boost::bind(&SimpleChat::OnConnection, &simple_chat, _1));
  // 配置端口（5011）
  HTTPServer http_server(5011, "HTTPServer");
  // 配置路由
  http_server.Handle("/socket.io", &sio_server);
  ...
  http_server.Start();
```
[详细示例]


## 代码详解
[socketio_cpp详解]


[socket.io]:https://github.com/socketio/socket.io
[详细示例]:https://github.com/chenshuchao/socketio_cpp/tree/master/example
[socketio_cpp详解]:http://shuchao.me/2016/03/13/socketio_cpp%E8%AF%A6%E8%A7%A3/

## benchmark(TODO)
[branch](https://github.com/chenshuchao/socketio_cpp/tree/benchmark)

