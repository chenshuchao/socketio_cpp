#include "engineio/socket.h"
#include <bytree/logging.hpp>

#include "engineio/transport.h"
#include "engineio/transports/transport_factory.h"

using namespace std;
using namespace woody;
using namespace engineio;

Socket::Socket(const string sid,
               const BaseTransportPtr& tran)
    : sid_(sid),
      state_(kStateOpening),
      transport_(tran),
      upgraded_(false) {
  SetTransport(transport_);
}

void Socket::HandleRequest(const woody::HTTPHandlerPtr& handler,
                           const woody::HTTPRequest& req,
                           woody::HTTPResponse& resp) {
  if(req.IsUpgrade()) {
    LOG(DEBUG) << "Socket::HandleRequest - HTTP request upgrade.";
    MaybeUpgrade(handler, req, resp);
    return;
  }
  transport_->HandleRequest(handler, req, resp);
}

void Socket::SetTransport(const BaseTransportPtr& tran) {
  tran->SetPacketCompleteCallback(
      boost::bind(&Socket::OnPacket, this, _1));
  tran->SetTriggerFlushCallback(boost::bind(&Socket::Flush, this));
  tran->SetCloseCallback(boost::bind(&Socket::OnClose, this));
}

void Socket::OnPacket(const Packet& packet) {
  LOG(DEBUG) << "Socket::OnPacket - type: " << packet.GetType();
  int packet_type = packet.GetType();
  switch (packet_type) {
    case Packet::kPacketClose: {
      ForceClose();
      break;
    }
    case Packet::kPacketPing: {
      OnPingPacket(packet);
      break;
    }
    case Packet::kPacketMessage: {
      OnMessagePacket(packet);
      break;
    } 
  }
}

void Socket::OnPacketWhenUpgrading(const Packet& packet) {
  if (Packet::kPacketPing == packet.GetType() &&
      "probe" == packet.GetBody()) {
    Packet packet;
    packet.SetType(Packet::kPacketPong);
    packet.SetBody("probe");
    vector<Packet> packets;
    packets.push_back(packet);
    upgrading_transport_->SendPackets(packets);
  
    CreateAndSendPacket(Packet::kPacketNoop, "");
    //TODO reset timer
  } else if (Packet::kPacketUpgrade == packet.GetType() &&
             state_ != kStateClose) {
    upgraded_ = true;
    transport_ = upgrading_transport_;
    SetTransport(transport_);
  } else {
    upgrading_transport_->ForceClose();
    upgrading_transport_.reset();
  }
  return;
}

bool Socket::MaybeUpgrade(const HTTPHandlerPtr& handler,
                          const HTTPRequest& req,
                          HTTPResponse& resp) {
  LOG(DEBUG) << "Socket::MaybeUpgrade";
  //if (upgraded_) return false;
  //TODO set upgrade timeout
  string sid, transport;
  req.GetGETParams("sid", sid);
  req.GetGETParams("transport", transport);
   
  // Get server transports
  Transports transports;
  BaseTransportFactoryPtr factory = transports.GetFactory(transport);
  BaseTransportPtr tran(factory->Create(handler, req, resp));
  tran->HandleRequest(handler, req, resp);
  upgrading_transport_ = tran;
  upgrading_transport_->SetPacketCompleteCallback(
      boost::bind(&Socket::OnPacketWhenUpgrading, this, _1));

  return true;
}

void Socket::SendOpenPacket(const std::string& data) {
  CreateAndSendPacket(Packet::kPacketOpen, data);
}

void Socket::SendMessage(const std::string& data) {
  CreateAndSendPacket(Packet::kPacketMessage, data);
}

void Socket::ForceClose() {
  if (kStateOpen == state_) {
    state_ = kStateClosing;
    transport_->ForceClose();
  }
}

void Socket::OnClose() {
  LOG(DEBUG) << "Socket::OnClose.";
  if (close_callback_) {
    LOG(DEBUG) << "Socket::OnClose - without this.";
    close_callback_();
  }
  if (close_callback_with_this_) {
    LOG(DEBUG) << "Socket::OnClose - with this.";
    close_callback_with_this_(shared_from_this());
  }
}

void Socket::OnPingPacket(const Packet& packet) {
  CreateAndSendPacket(Packet::kPacketPong, "");
  ping_callback_(shared_from_this(), packet.GetBody());
}

void Socket::OnMessagePacket(const Packet& packet) {
  message_callback_(shared_from_this(), packet.GetBody());
}

void Socket::OnUpgradePacket(const Packet& packet) {
}

void Socket::CreateAndSendPacket(int packet_type, const string& data) {
  LOG(DEBUG) << "Socket::CreateAndSendPacket, packet_type: " << packet_type;
  Packet packet;
  // TODO unnecessary ?
  if (!packet.SetType(packet_type)) {
    LOG(ERROR) << "Socket::SendPacket - "
              << " Unknown packet type : " << packet_type;
    return;
  }
  packet.SetBody(data);
  // LOCK
  write_buffer_.push_back(packet);
  // unlock
  Flush();
}

void Socket::Flush() {
  // TODO multiple threads unsafe?
    if (kStateClose != state_ && transport_->IsWritable() &&
      !write_buffer_.empty()) {
    LOG(DEBUG) << "Socket::Flush - Successfully flush.";
    // Lock
    vector<Packet> packets = write_buffer_;
    write_buffer_.clear();
    // unlock
    transport_->SendPackets(packets);
  } else {
    LOG(DEBUG) << "Socket::Flush - No ready for flushing.";
  }
}

