// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xgossip/include/gossip_dispatcher.h"

#include "xgossip/include/gossip_utils.h"
#include "xgossip/include/mesages_with_bloomfilter.h"
#include "xpbase/base/kad_key/kadmlia_key.h"

#include <cinttypes>

namespace tcash {
namespace gossip {

GossipDispatcher::GossipDispatcher(transport::TransportPtr transport_ptr) : GossipInterface{transport_ptr} {
}
GossipDispatcher::~GossipDispatcher() {
}

inline bool IS_INDEX_SENT(std::size_t node_index, uint64_t s1, uint64_t s2) {
    // if (node_index > 64) {
    //     return ((s2 >> (node_index - 65)) & 0x01) == 0x01 ? true : false;
    // } else {
    //     return ((s1 >> (node_index - 1)) & 0x01) == 0x01 ? true : false;
    // }
    return node_index > 64 ? (((s2 >> (node_index - 65)) & 0x01) == 0x01 ? true : false) : (((s1 >> (node_index - 1)) & 0x01) == 0x01 ? true : false);
}

inline void SET_INDEX_SENT(std::size_t node_index, uint64_t & s1, uint64_t & s2) {
    if (node_index > 64) {
        s2 |= ((uint64_t)0x01 << (node_index - 65));
    } else {
        s1 |= ((uint64_t)0x01 << (node_index - 1));
    }
}

void GossipDispatcher::Broadcast(transport::protobuf::RoutingMessage & message, kadmlia::ElectRoutingTablePtr const & routing_table) {
    CheckDiffNetwork(message);

    base::KadmliaKeyPtr kad_key_ptr = base::GetKadmliaKey(message.des_node_id());
    base::ServiceType service_type = kad_key_ptr->GetServiceType();
    bool is_broadcast_height = service_type.IsBroadcastService();
    xdbg("[GossipDispatcher::Broadcast] is_broadcast_height %s %s %s", kad_key_ptr->Get().c_str(), service_type.info().c_str(), is_broadcast_height ? "true" : "false");

    auto gossip_max_hop_num = kGossipDefaultMaxHopNum;
    if (message.gossip().max_hop_num() > 0) {
        gossip_max_hop_num = message.gossip().max_hop_num();
    }
    if (gossip_max_hop_num <= message.hop_num()) {
        xdbg("msg_hash:%u msg_type:%d hop_num:%d larger than gossip_max_hop_num:%d", message.msg_hash(), message.type(), message.hop_num(), gossip_max_hop_num);
        return;
    }

    std::vector<gossip::DispatchInfos> select_nodes;
    GenerateDispatchInfos(message, routing_table, select_nodes, is_broadcast_height);
    if (select_nodes.empty()) {
        xdbg("stcash broadcast, select_nodes empty,msg_hash:%u msg_type:%d hop_num:%d", message.msg_hash(), message.type(), message.hop_num());
        return;
    }

    SendDispatch(message, select_nodes);
}

void GossipDispatcher::GenerateDispatchInfos(transport::protobuf::RoutingMessage & message,
                                             kadmlia::ElectRoutingTablePtr const & routing_table,
                                             std::vector<DispatchInfos> & select_nodes,
                                             bool is_broadcast_height) {
    uint64_t sit1 = message.gossip().sit1();
    uint64_t sit2 = message.gossip().sit2();
    xdbg("[GossipDispatcher::GenerateDispatchInfos] got % " PRIu64 " % " PRIu64 ":", sit1, sit2);

    MessageKey msg_key(message.msg_hash());
    MessageWithBloomfilter::Instance()->UpdateHandle(msg_key, sit1, sit2);

    xdbg("[GossipDispatcher::GenerateDispatchInfos] after update % " PRIu64 " % " PRIu64 ":", sit1, sit2);

    uint32_t overlap = message.gossip().overlap_rate();

    std::unordered_map<std::string, kadmlia::NodeInfoPtr> const nodes_map = routing_table->nodes(is_broadcast_height);
    std::unordered_map<std::string, std::size_t> const index_map = routing_table->index_map(is_broadcast_height);
    std::vector<std::string> const xip2_for_shuffle = routing_table->get_shuffled_xip2(is_broadcast_height);

    std::size_t const current_round_size = routing_table->nodes_size();

    SET_INDEX_SENT(routing_table->get_self_index(), sit1, sit2);
    std::size_t index_i;
    std::vector<std::string> select_xip2;
    for (index_i = 0; index_i < xip2_for_shuffle.size(); ++index_i) {
        if (index_map.find(xip2_for_shuffle[index_i]) != index_map.end()) {
            std::size_t node_index = index_map.at(xip2_for_shuffle[index_i]);
            if (message.hop_num() == 0 && node_index >= current_round_size)
                continue;  // should not select last round nodes at first round since there might not be able to spread this.
            if (IS_INDEX_SENT(node_index, sit1, sit2))
                continue;
            SET_INDEX_SENT(node_index, sit1, sit2);
            select_xip2.push_back(xip2_for_shuffle[index_i]);
            if (select_xip2.size() > kGossipLayerNeighborNum)
                break;
        } else {
            // actually won't triggle this line when 
            xwarn("[GossipDispatcher::GenerateDispatchInfos] routing table still uncomplete , missing: %s", xip2_for_shuffle[index_i].c_str());
        }
    }
    for (auto _xip2 : select_xip2) {
        // nodes might not be online
        if (nodes_map.at(_xip2) != nullptr) {
            select_nodes.push_back(DispatchInfos(nodes_map.at(_xip2), sit1, sit2, index_map.at(_xip2) >= current_round_size));
        } else {
            xwarn("[GossipDispatcher::GenerateDispatchInfos] routing table still uncomplete , missing: %s", _xip2.c_str());
        }
    }
    if (select_nodes.empty())
        return;

    std::size_t this_round_node_count = 0;
    std::for_each(select_nodes.begin(), select_nodes.end(), [&select_nodes, &this_round_node_count](DispatchInfos const & info) {
        if (!info.is_last_round_node)
            this_round_node_count++;
    });
    std::sort(select_nodes.begin(), select_nodes.end(), [](DispatchInfos const & a, DispatchInfos const & b) -> bool { return a.is_last_round_node < b.is_last_round_node; });
    if (this_round_node_count == 0)
        this_round_node_count = select_nodes.size(); // all nodes are last round . special case.

    for (std::size_t _index = 0; _index < select_nodes.size(); ++_index) {
        xdbg("[GossipDispatcher::GenerateDispatchInfos] before % " PRIu64 " % " PRIu64 ":", select_nodes[_index].sit1, select_nodes[_index].sit2);
    }

    for (; index_i < xip2_for_shuffle.size(); ++index_i) {
        if (index_map.find(xip2_for_shuffle[index_i]) != index_map.end()) {
            std::size_t node_index = index_map.at(xip2_for_shuffle[index_i]);
            if (IS_INDEX_SENT(node_index, sit1, sit2))
                continue;
            static uint32_t seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            static std::mt19937 rng(seed);
            std::size_t send_node_index = rng() % this_round_node_count;
            for (std::size_t _index = 0; _index < select_nodes.size(); ++_index) {
                if ((_index != send_node_index && (rng() % overlap))) {
                    SET_INDEX_SENT(node_index, select_nodes[_index].get_sit1(), select_nodes[_index].get_sit2());
                }
            }
        } else {
            xwarn("[GossipDispatcher::GenerateDispatchInfos] routing table still uncomplete , missing: %s", xip2_for_shuffle[index_i].c_str());
        }
    }

    for (std::size_t _index = 0; _index < select_nodes.size(); ++_index) {
        xdbg("[GossipDispatcher::GenerateDispatchInfos] after % " PRIu64 " % " PRIu64 ":", select_nodes[_index].sit1, select_nodes[_index].sit2);
    }

    return;
}

}  // namespace gossip
}  // namespace tcash
