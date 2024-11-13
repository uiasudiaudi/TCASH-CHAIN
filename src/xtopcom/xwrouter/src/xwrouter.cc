// Copyright (c) 2017-2019 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xwrouter/xwrouter.h"

#include "xbase/xutl.h"
#include "xgossip/include/gossip_bloomfilter.h"
#include "xgossip/include/gossip_dispatcher.h"
#include "xgossip/include/gossip_filter.h"
#include "xgossip/include/gossip_rrs.h"
#include "xgossip/include/gossip_utils.h"
#include "xkad/routing_table/routing_utils.h"
#include "xmetrics/xmetrics.h"
#include "xpbase/base/kad_key/kadmlia_key.h"
#include "xpbase/base/uint64_bloomfilter.h"
#include "xtransport/udp_transport/transport_util.h"
#include "xtransport/utils/transport_utils.h"
#include "xwrouter/message_handler/wrouter_message_handler.h"
#include "xwrouter/message_handler/xwrouter_xid_handler.h"

#include <algorithm>
#include <cinttypes>

namespace tcash {

using namespace kadmlia;
using namespace gossip;

namespace wrouter {

Wrouter::Wrouter() : wxid_handler_(nullptr) /*, wxip_handler_(nullptr)*/ {
}

Wrouter::~Wrouter() {
}

Wrouter * Wrouter::Instance() {
    static Wrouter ins;
    return &ins;
}

void Wrouter::Init(transport::TransportPtr transport_ptr) {
    assert(transport_ptr);
    auto bloom_gossip_ptr = std::make_shared<GossipBloomfilter>(transport_ptr);
    auto bloom_layer_gossip_ptr = nullptr;
    auto gossip_rrs_ptr = std::make_shared<GossipRRS>(transport_ptr);
    auto gossip_dispatcher_ptr = std::make_shared<GossipDispatcher>(transport_ptr);
    wxid_handler_ = std::make_shared<WrouterXidHandler>(transport_ptr, bloom_gossip_ptr, bloom_layer_gossip_ptr, gossip_rrs_ptr, gossip_dispatcher_ptr);

    // GossipFilter for global
    gossip::GossipFilter::Instance()->Init();
}

#define IS_BROADCAST(message) (message.broadcast())
#define IS_RRS_GOSSIP_MESSAGE(message) (message.is_root() && message.broadcast() && message.gossip().gossip_type() == kGossipRRS)
#define MESSAGE_BASIC_INFO(message) "src_node_id", (message.src_node_id()), "dst_node_id", (message.des_node_id()), "hop_num", message.hop_num()
#define MESSAGE_RRS_FEATURE(message) "gossip_header_hash", std::stol(message.gossip().header_hash()), "gossip_block_size", message.gossip().block().size()
#define MESSAGE_FEATURE(message)                                                                                                                                                   \
    "msg_hash", message.gossip().header_hash().empty() ? std::to_string(message.msg_hash()) : message.gossip().header_hash(), "msg_size", message.gossip().block().size()
#define IS_ROOT_BROADCAST(message) "is_root", message.is_root(), "is_broadcast", message.broadcast()
#define PACKET_SIZE(packet) "packet_size", packet.get_size()
#define NOW_TIME "timestamp", GetCurrentTimeMsec()

void Wrouter::send(transport::protobuf::RoutingMessage & message, std::error_code & ec) {
    // if (message.has_broadcast() && message.broadcast()) {
    if (!message.has_msg_hash()) {
        auto gossip = message.mutable_gossip();
        std::string bin_data = message.data();
        if (gossip->has_block()) {
            bin_data = gossip->block();
        }
        if (!gossip->has_block() && gossip->has_header_hash()) {
            bin_data = gossip->header_hash();
        }
        uint32_t msg_hash = base::xhash32_t::digest(std::to_string(message.id()) + bin_data);
        message.set_msg_hash(msg_hash);
    }
    if (IS_BROADCAST(message)) {
#ifdef XENABLE_P2P_BENDWIDTH
        XMETRICS_PACKET_INFO("p2pbroadcast_wroutersend_info", MESSAGE_BASIC_INFO(message), MESSAGE_FEATURE(message), IS_ROOT_BROADCAST(message), NOW_TIME);
#else
        xinfo("p2pbroadcast_wroutersend_info src:%s dst:%s hop_num:%d msg_hash:%" PRIu32 " msg_size:%zu is_root:%d is_broadcast:%d timestamp:%" PRIu64,
              message.src_node_id().c_str(),
              message.des_node_id().c_str(),
              message.hop_num(),
              message.msg_hash(),
              message.gossip().block().size(),
              message.is_root(),
              message.broadcast(),
              GetCurrentTimeMsec());
#endif
    }
    wxid_handler_->SendPacket(message, ec);
    return;
}

/// register callback at `bool MultilayerNetwork::Init`
/// which is called at `ThreadHandler::fired_packet` && `xquic_node_t::on_quic_message_ready`
int32_t Wrouter::recv(transport::protobuf::RoutingMessage & message, base::xpacket_t & packet) {
    if (message.hop_num() >= kHopToLive) {
        xwarn(
            "stcash send msg because hop to live is max: %d [%s] des[%s] "
            "message_type[%d]",
            kHopToLive,
            HexSubstr(message.src_node_id()).c_str(),
            HexSubstr(message.des_node_id()).c_str(),
            message.type());
        return enum_xerror_code_fail;
    }

    int32_t rcode = wxid_handler_->RecvPacket(message, packet);
    if (rcode == kRecvOwn) {
        return HandleOwnPacket(message, packet);
    }
    return rcode;
}

#undef IS_BROADCAST
#undef IS_RRS_GOSSIP_MESSAGE
#undef MESSAGE_BASIC_INFO
#undef MESSAGE_RRS_FEATURE
#undef MESSAGE_FEATURE
#undef IS_ROOT_BROADCAST
#undef PACKET_SIZE
#undef NOW_TIME

int32_t Wrouter::HandleOwnPacket(transport::protobuf::RoutingMessage & message, base::xpacket_t & packet) {
    WrouterMessageHandler::Instance()->HandleMessage(message, packet);
    return enum_xcode_successful;
}

int32_t Wrouter::HandleOwnSyncPacket(transport::protobuf::RoutingMessage & message, base::xpacket_t & packet) {
    if (gossip::GossipFilter::Instance()->FilterMessage(message)) {
        tcash_NETWORK_DEBUG_FOR_PROTOMESSAGE("wrouter sync filtered", message);
        return enum_xcode_successful;
    }

    WrouterMessageHandler::Instance()->HandleSyncMessage(message, packet);
    return enum_xcode_successful;
}

void Wrouter::update_rrs_params(uint32_t t, uint32_t k) {
    return wxid_handler_->update_rrs_params(t, k);
}

}  // namespace wrouter

}  // namespace tcash
