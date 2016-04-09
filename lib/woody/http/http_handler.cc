#include "woody/http/http_handler.h"

#include <bytree/logging.hpp>
#include <bytree/string_util.hpp>

#include "woody/base/base_util.h"

using namespace std;
using namespace bytree;
using namespace woody;

HTTPHandler::HTTPHandler(const string& name,
                         const muduo::net::TcpConnectionPtr& conn)
    : BaseHandler(), name_(name), conn_(conn), is_dying_(false) {
  codec_.SetMessageBeginCallback(
      boost::bind(&HTTPHandler::OnMessageBegin, this, _1));
  codec_.SetMessageCompleteCallback(
      boost::bind(&HTTPHandler::OnMessageComplete, this, _1, _2));
  codec_.SetErrorCallback(
      boost::bind(&HTTPHandler::HandleError, this));

  conn_->setMessageCallback(
      boost::bind(&HTTPHandler::OnData, this, _1, _2, _3));
  conn_->setCloseCallbackWithoutThis(boost::bind(&HTTPHandler::OnClose, this));
}

void HTTPHandler::ForceClose() {
  conn_->forceClose();
}

void HTTPHandler::Send(const string& data) {
  LOG(DEBUG) << "HTTPHandler::Send [" << GetName()
            << "] - response:\n" << data;
  conn_->send(data);
}

void HTTPHandler::OnData(const muduo::net::TcpConnectionPtr& conn,
                         muduo::net::Buffer* buf,
                         muduo::Timestamp) {
  LOG(DEBUG) << "HTTPHandler::OnData [" << GetName()
           << "] - protocol: HTTP.";
  while (buf->readableBytes() > 0) {
    string data(buf->peek(), buf->readableBytes());
    size_t parsed_bytes = codec_.OnData(data);
    if (!parsed_bytes) {
      // It means we need to wait for more data.
      break; 
    }
    buf->retrieve(parsed_bytes);
  }
  if (is_dying_) {
    OnClose();
  }
}

void HTTPHandler::OnMessageComplete(HTTPCodec::StreamID id, HTTPRequest& req) {
  LOG(DEBUG) << "HTTPHandler::OnMessageComplete []";
  HTTPResponse resp(shared_from_this());
  request_complete_callback_(shared_from_this(), req, resp);
  codec_.CleanUp();
}

void HTTPHandler::OnClose() {
  if (close_callback_) {
    close_callback_();
  }
  if (close_callback_with_this_) {
    close_callback_with_this_(shared_from_this());
  }
}

