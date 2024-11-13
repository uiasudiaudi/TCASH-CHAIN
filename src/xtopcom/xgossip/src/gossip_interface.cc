// Copyright (c) 2017-2019 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xgossip/gossip_interface.h"

#include "xgossip/include/gossip_utils.h"
#include "xpbase/base/kad_key/kadmlia_key.h"
#include "xpbase/base/tcash_log.h"
#include "xpbase/base/tcash_utils.h"
#include "xpbase/base/uint64_bloomfilter.h"

#include <cinttypes>
#include <unordered_set>

#ifdef XENABLE_P2P_TEST
#    include "xmetrics/xmetrics.h"

#    define METRICS_SEND_RECORD(message)                                                                                                                                           \
        XMETRICS_PACKET_INFO("p2ptest_send_record",                                                                                                                                \
                             "src_node_id",                                                                                                                                        \
                             message.src_node_id(),                                                                                                                                \
                             "dst_node_id",                                                                                                                                        \
                             message.des_node_id(),                                                                                                                                \
                             "dst_ip_port",                                                                                                                                        \
                             node_info_ptr->public_ip + ":" + std::to_string(node_info_ptr->public_port),                                                                          \
                             "hop_num",                                                                                                                                            \
                             message.hop_num(),                                                                                                                                    \
                             "msg_hash",                                                                                                                                           \
                             message.gossip().header_hash().empty() ? std::to_string(message.msg_hash()) : message.gossip().header_hash(),                                         \
                             "msg_size",                                                                                                                                           \
                             message.data().size(),                                                                                                                                \
                             "is_root",                                                                                                                                            \
                             message.is_root(),                                                                                                                                    \
                             "is_broadcast",                                                                                                                                       \
                             message.broadcast(),                                                                                                                                  \
                             "timestamp",                                                                                                                                          \
                             GetCurrentTimeMsec());

#else

#    define METRICS_SEND_RECORD(message)

#endif

namespace tcash {

namespace gossip {

void GossipInterface::CheckDiffNetwork(transport::protobuf::RoutingMessage & message) {
    if (message.gossip().diff_net()) {
        auto gossip_param = message.mutable_gossip();
        gossip_param->set_diff_net(false);
        // message.clear_bloomfilter();
        message.set_hop_num(message.hop_num() - 1);
        tcash_DEBUG("message from diff network and arrive the des network at the first time. %s", message.debug().c_str());
    }
}

uint64_t GossipInterface::GetDistance(const std::string & src, const std::string & des) {
    assert(src.size() >= sizeof(uint64_t));
    assert(des.size() >= sizeof(uint64_t));
    assert(src.size() == des.size());
    uint64_t dis = 0;
    uint32_t index = src.size() - 1;
    uint32_t rollleft_num = 56;
    for (uint32_t i = 0; i < 8; ++i) {
        dis += (static_cast<uint64_t>(static_cast<uint8_t>(src[index]) ^ static_cast<uint8_t>(des[index])) << rollleft_num);
        --index;
        rollleft_num -= 8;
    }
    return dis;
}

void GossipInterface::Send(transport::protobuf::RoutingMessage & message, const std::vector<kadmlia::NodeInfoPtr> & nodes) {
    std::string data;
    if (!message.SerializeToString(&data)) {
        tcash_WARN2("wrouter message SerializeToString failed");
        return;
    }
    auto each_call = [this, &data, &message](kadmlia::NodeInfoPtr node_info_ptr) {
        if (!node_info_ptr) {
            tcash_WARN2("kadmlia::NodeInfoPtr null");
            return false;
        }

        METRICS_SEND_RECORD(message);

        if (kadmlia::kKadSuccess != transport_ptr_->SendDataWithProp(data, node_info_ptr->public_ip, node_info_ptr->public_port, node_info_ptr->udp_property)) {
            tcash_WARN2("SendData to  endpoint(%s:%d) failed", node_info_ptr->public_ip.c_str(), node_info_ptr->public_port);
            return false;
        }

        return true;
    };

    std::for_each(nodes.begin(), nodes.end(), each_call);
}

// usually for root-broadcast
void GossipInterface::MutableSend(transport::protobuf::RoutingMessage & message, const std::vector<kadmlia::NodeInfoPtr> & nodes) {
    auto each_call = [this, &message](kadmlia::NodeInfoPtr node_info_ptr) {
        if (!node_info_ptr) {
            tcash_WARN2("kadmlia::NodeInfoPtr null");
            return false;
        }

        message.set_des_node_id(node_info_ptr->node_id);
        std::string data;
        if (!message.SerializeToString(&data)) {
            tcash_WARN2("wrouter message SerializeToString failed");
            return false;
        }

        METRICS_SEND_RECORD(message);

        if (kadmlia::kKadSuccess != transport_ptr_->SendDataWithProp(data, node_info_ptr->public_ip, node_info_ptr->public_port, node_info_ptr->udp_property)) {
            tcash_WARN2("SendData to  endpoint(%s:%d) failed", node_info_ptr->public_ip.c_str(), node_info_ptr->public_port);
            return false;
        }

        return true;
    };

    std::for_each(nodes.begin(), nodes.end(), each_call);
}

// usually for root-broadcast hash
// minimal packet size.
void GossipInterface::MutableSendHash(transport::protobuf::RoutingMessage & message, const std::vector<kadmlia::NodeInfoPtr> & nodes) {
    std::string data;
    if (!message.SerializeToString(&data)) {
        tcash_WARN2("wrouter message SerializeToString failed");
        return;
    }

    auto each_call = [this, &data, &message](kadmlia::NodeInfoPtr node_info_ptr) {
        if (!node_info_ptr) {
            tcash_WARN2("kadmlia::NodeInfoPtr null");
            return false;
        }

        METRICS_SEND_RECORD(message);

        if (kadmlia::kKadSuccess != transport_ptr_->SendDataWithProp(data, node_info_ptr->public_ip, node_info_ptr->public_port, node_info_ptr->udp_property)) {
            tcash_WARN2("SendData to  endpoint(%s:%d) failed", node_info_ptr->public_ip.c_str(), node_info_ptr->public_port);
            return false;
        }

        return true;
    };

    std::for_each(nodes.begin(), nodes.end(), each_call);
}

// todo next version delete GetNeighborCount.(only used in old broadcast)
uint32_t GossipInterface::GetNeighborCount(transport::protobuf::RoutingMessage & message) {
    if (message.gossip().neighber_count() > 0) {
        return message.gossip().neighber_count();
    }
    return 3;
}

std::vector<kadmlia::NodeInfoPtr> GossipInterface::GetRandomNodes(std::vector<kadmlia::NodeInfoPtr> & neighbors, uint32_t number_to_get) const {
    if (neighbors.size() <= number_to_get) {
        return neighbors;
    }
    std::random_shuffle(neighbors.begin(), neighbors.end());
    return std::vector<kadmlia::NodeInfoPtr>{neighbors.begin(), neighbors.begin() + number_to_get};
}

void GossipInterface::SendDispatch(transport::protobuf::RoutingMessage & message, const std::vector<gossip::DispatchInfos> & dispatch_nodes) {
    for (uint32_t i = 0; i < dispatch_nodes.size(); ++i) {
        auto node_info_ptr = dispatch_nodes[i].nodes;
        auto gossip = message.mutable_gossip();

        gossip->set_sit1(dispatch_nodes[i].sit1);
        gossip->set_sit2(dispatch_nodes[i].sit2);
        xdbg("[debug] send to %s:%d % " PRIu64 " % " PRIu64, node_info_ptr->public_ip.c_str(), node_info_ptr->public_port, dispatch_nodes[i].sit1, dispatch_nodes[i].sit2);

        std::string data;
        if (!message.SerializeToString(&data)) {
            xwarn("wrouter message SerializeToString failed");
            return;
        }

        METRICS_SEND_RECORD(message);

        if (kadmlia::kKadSuccess != transport_ptr_->SendDataWithProp(data, node_info_ptr->public_ip, node_info_ptr->public_port, node_info_ptr->udp_property)) {
            xinfo("SendDispatch send to (%s:%d) failed % " PRIu64 " % " PRIu64,
                  node_info_ptr->public_ip.c_str(),
                  node_info_ptr->public_port,
                  dispatch_nodes[i].sit1,
                  dispatch_nodes[i].sit2);
            continue;
        }
    };
}

}  // namespace gossip

}  // namespace tcash
