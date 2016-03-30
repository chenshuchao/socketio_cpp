#define LOGGING_LEVEL INFO
#include <fstream>
#include <string>

#include <jsoncpp/json/json.h>
#include <bytree/logging.hpp>
#include <woody/http/http_server.h>
#include <woody/http/http_application.h>
#include <socketio/server.h>
#include <socketio/socket.h>

using namespace std;
using namespace socketio;
using namespace woody;

bool IsStringEndWith(string s, string p) {
  return s.find(p) == (s.size() - p.size());
}

class FileApp : public HTTPApplication {
 public:
  FileApp(const string& root) : root_(root) {
  }
  virtual ~FileApp() { }

  virtual void HandleRequest(const HTTPHandlerPtr& handler,
                             const HTTPRequest& req,
                             HTTPResponse& resp) {
    string url = req.GetUrl();
    if (url == "/index.html" || url.find("/static/") == 0) {
      string abs_url = root_ + "/" + url;
      ifstream ifs(abs_url.c_str());
      string content((std::istreambuf_iterator<char>(ifs)),
                          (std::istreambuf_iterator<char>()));
      string content_type;
      if (IsStringEndWith(url, ".js")) {
        content_type = "application/javascript";
      } else if (IsStringEndWith(url, ".css")) {
        content_type = "text/css";
      } else if (IsStringEndWith(url, ".swf")) {
        content_type = "application/x-shockwave-flash";
      } else {
        content_type = "text/html";
      }
      resp.SetStatus(200, "OK")
          .AddHeader("Content-Type", content_type)
          .AddBody(content);
      resp.End();
      return;
    }
    resp.SetStatus(404, "Not Found");
    resp.End();
  }
  private:
   string root_;
};

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
  if (argc < 2) {
    LOG(ERROR) << "Program needs more params.";
    return 1;
  }
  string root_path(argv[1]);
  FileApp file_app(root_path);

  SimpleChat simple_chat;
  Server sio_server("SocketIOServer");
  sio_server.SetSocketConnectCallback(
    boost::bind(&SimpleChat::OnConnection, &simple_chat, _1));

  HTTPServer http_server(5011, "HTTPServer");
  http_server.Handle("/socket.io", &sio_server);
  http_server.Handle("/", &file_app);

  http_server.Start();
}

