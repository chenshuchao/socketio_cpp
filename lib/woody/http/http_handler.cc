#include "woody/http/http_handler.h"

#include <muduo/base/Logging.h>
#include "woody/base/string_util.h"

using namespace std;
using namespace woody;

HTTPHandler::HTTPHandler(const string& name,
                         const muduo::net::TcpConnectionPtr& conn)
    : BaseHandler(), name_(name), conn_(conn) {
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
  LOG_INFO << "HTTPHandler::Send [" << GetName()
           << "] - response:\n" << data;
  conn_->send(data);
}

void HTTPHandler::OnData(const muduo::net::TcpConnectionPtr& conn,
                         muduo::net::Buffer* buf,
                         muduo::Timestamp) {
  LOG_DEBUG << "HTTPHandler::OnData [" << GetName()
           << "] - protocol: HTTP.";
  while (buf->readableBytes() > 0) {
    string data = convert_to_std(buf->retrieveAllAsString());
    size_t bytes_parsed = codec_.OnData(data);
    if (!bytes_parsed) {
      // It means we need to wait for more data.
      break; 
    }
    string unparsed_str(data, bytes_parsed);
    LOG_DEBUG << "unparsed bytes" << unparsed_str.size()
              << "unparsed_str : " << unparsed_str;
    buf->prepend(unparsed_str.c_str(), unparsed_str.size());
    LOG_DEBUG << "readable bytes: " << buf->readableBytes();
  }
}

void HTTPHandler::OnMessageComplete(HTTPCodec::StreamID id, HTTPRequest& req) {
  LOG_DEBUG << "HTTPHandler::OnMessageComplete []";
  HTTPResponse resp(shared_from_this());
  request_complete_callback_(shared_from_this(), req, resp);
}

void HTTPHandler::OnClose() {
  if (close_callback_with_this_) {
    close_callback_with_this_(shared_from_this());
  }
  if (close_callback_) {
    close_callback_();
  }
}

