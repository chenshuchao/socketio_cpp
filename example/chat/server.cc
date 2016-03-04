#define LOG_LEVEL DEBUG
#include <fstream>
#include <string>

#include <bytree/logging.hpp>
#include <bytree/string_util.hpp>
#include <bytree/json/json_codec.hpp>
#include <socketio/server.h>
#include <woody/http/http_server.h>
#include <woody/http/http_application.h>

using namespace std;
using namespace bytree;
using namespace woody;
using namespace socketio;

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
      if (EndsWith(url, ".js")) {
        content_type = "application/javascript";
      } else if (EndsWith(url, ".css")) {
        content_type = "text/css";
      } else if (EndsWith(url, ".swf")) {
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

// Just a simple class to store user data.
class Member {
 public:
  Member(const SocketPtr& socket) 
      : socket_(socket),
        is_added_user_(false) {
  }
  bool IsAddedUser() const { return is_added_user_; }

  void SetAddedUser(bool b) { is_added_user_ = b; }

  void SetUsername(const string& name) { username_ = name; }

  string GetUsername() const { return username_; }

 private:
  SocketPtr socket_;
  bool is_added_user_;
  string username_;
};

class ChatRoom {
 public:
  typedef map<string, Member> String2Member;
  ChatRoom() : usernum_(0) {
  }

  void OnConnection(const SocketPtr& socket) {
    LOG(DEBUG) << "ChatRoom::OnConnection.";
    socket->SetCloseCallbackWithThis(
        boost::bind(&ChatRoom::OnDisconnect, this, _1));
    socket->Join("chatroom");
    socket->On("new message", boost::bind(&ChatRoom::OnNewMessage, this, _1, _2));
    socket->On("add user", boost::bind(&ChatRoom::OnAddUser, this, _1, _2));
    //socket->On("disconnect", boost::bind(&ChatRoom::OnDisconnect, this, _1, _2));
    Member m(socket);
    // FIXME mutex begin
    members_.insert(pair<string, Member>(socket->GetSid(), m));
    // mutex end
  }

  void OnDisconnect(const SocketPtr& socket) {
    LOG(DEBUG) << "ChatRoom::OnDisconnect - " << socket->GetSid();
    // FIXME mutex begin
    String2Member::iterator it = members_.find(socket->GetSid());
    if (it == members_.end()) {
      LOG(ERROR) << "ChatRoom::OnDisconnec - Member not found.";
      return;
    }
    Member& m(it->second);
    
    if (m.IsAddedUser()) {
      usernum_ --;
      JsonCodec codec;
      codec.Add("username", m.GetUsername())
           .Add("numUsers", usernum_);
      socket->Broadcast("chatroom", "user left", codec.Stringify());
      members_.erase(socket->GetSid());
    }
    // mutex end
  }

 private:
  // Called when client call socket->emit("add user", data);
  void OnAddUser(const SocketPtr& socket, const string& username) {
    LOG(DEBUG) << "ChatRoom::OnAddUser - username: " << username;

    // FIXME mutex begin
    String2Member::iterator it = members_.find(socket->GetSid());
    if (it == members_.end()) {
      LOG(ERROR) << "ChatRoom::OnAddUser - member not found.";
      return;
    }
    Member& m = it->second;
    if (m.IsAddedUser()) {
      LOG(DEBUG) << "ChatRoom::OnAddUser - Member has added.";
      return;
    }
    m.SetAddedUser(true);
    m.SetUsername(username);
    usernum_ ++;
    int cur_num = usernum_;
    // mutex end

    JsonCodec codec;
    codec.Add("numUsers", cur_num);
    string login_resp = codec.Stringify();
    LOG(DEBUG) << "OnAddUser - login_resp: " << login_resp;
    socket->Emit("login", login_resp);

    JsonCodec codec2;
    codec2.Add("username", username)
          .Add("numUsers", cur_num);
    socket->Broadcast("chatroom", "user joined", codec2.Stringify());
  }

  // Called when client call socket->emit("new message", data);
  void OnNewMessage(const SocketPtr& socket, const string& data) {
    // FIXME mutex begin.
    String2Member::iterator it = members_.find(socket->GetSid());
    if (it == members_.end()) {
      LOG(ERROR) << "ChatRoom::OnNewMessage - Member not found.";
      return;
    }
    Member m = it->second;
    // mutex end.
    JsonCodec codec;
    codec.Add("username", m.GetUsername())
         .Add("message", data);
    socket->Broadcast("chatroom", "new message", codec.Stringify());
  }

  unsigned int usernum_;
  map<string, Member> members_;
};
 
int main(int argc, char** argv) {
  if (argc < 2) {
    LOG(ERROR) << "Program needs more params.";
    return 1;
  }
  string root_path(argv[1]);
  FileApp file_app(root_path);

  Server sio_server("SocketIOServer");
  ChatRoom room;
  sio_server.SetSocketConnectCallback(
      boost::bind(&ChatRoom::OnConnection, &room, _1));

  HTTPServer http_server(5011, "HTTPServer");
  http_server.Handle("/socket.io", &sio_server);
  http_server.Handle("/", &file_app);
  http_server.Start();
}

