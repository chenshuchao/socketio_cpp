#ifndef ENGINEIO_TRANSPORT_H
#define ENGINEIO_TRANSPORT_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <woody/http/http_request.h>
#include <woody/http/http_response.h>

#include "engineio/parser.h"

namespace engineio {
class BaseTransport {
 public:
  typedef boost::function<void (const Packet&)>
      PacketCompleteCallback;
  typedef boost::function<void ()> TriggerFlushCallback;
  typedef boost::function<void ()> CloseCallback;
  BaseTransport(const std::string& name)
      : name_(name),
        is_support_binary_(false) {
  }

  // Get All supported upgrade
  virtual void GetAllUpgrades(std::vector<std::string>& vec) = 0;
  // Handle http request
  virtual void HandleRequest(const woody::HTTPHandlerPtr& handler,
                             const woody::HTTPRequest& req,
                             woody::HTTPResponse& resp) = 0;

  // Send engineio packet to transport
  virtual void SendPackets(std::vector<Packet>& packets) = 0;

  virtual void OnClose() {
    close_callback_();
  }

  virtual void ForceClose() = 0;

  virtual bool IsWritable() const = 0;

  // Called when data was decoded to packet
  void OnPacket(const Packet& packet) {
    packet_complete_callback_(packet);
  }

  void SetPacketCompleteCallback(const PacketCompleteCallback& cb) {
    packet_complete_callback_ = cb;
  }
  void SetTriggerFlushCallback(const TriggerFlushCallback& cb) {
    trigger_flush_callback_ = cb;
  }
  void SetCloseCallback(const CloseCallback& cb) {
    close_callback_ = cb;
  }

  std::string GetName() const { return name_; }

  bool IsSupportBinary() const { return is_support_binary_; }

  void SetSupportBinary(bool b) { is_support_binary_ = b; }

  void TriggerFlush() {
    trigger_flush_callback_();
  }

 private:
  // 
  virtual void OnData(const std::string& data) = 0;

  std::string name_;
  bool is_support_binary_;
  PacketCompleteCallback packet_complete_callback_;
  TriggerFlushCallback trigger_flush_callback_;
  CloseCallback close_callback_;
};
typedef boost::shared_ptr<BaseTransport> BaseTransportPtr;
}

#endif
