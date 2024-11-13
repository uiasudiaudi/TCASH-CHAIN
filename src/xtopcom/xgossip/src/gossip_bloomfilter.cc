// Copyright (c) 2017-2019 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xgossip/include/gossip_bloomfilter.h"

#include "xbase/xbase.h"
#include "xbase/xcontext.h"
#include "xbase/xhash.h"
#include "xgossip/include/block_sync_manager.h"
#include "xgossip/include/gossip_utils.h"
#include "xgossip/include/mesages_with_bloomfilter.h"
#include "xpbase/base/tcash_log.h"
#include "xpbase/base/tcash_utils.h"
#include "xpbase/base/uint64_bloomfilter.h"

namespace tcash {

namespace gossip {

GossipBloomfilter::GossipBloomfilter(transport::TransportPtr transport_ptr) : GossipInterface(transport_ptr) {
}

GossipBloomfilter::~GossipBloomfilter() {
}

void GossipBloomfilter::Broadcast(uint64_t local_hash64, transport::protobuf::RoutingMessage & message, std::shared_ptr<std::vector<kadmlia::NodeInfoPtr>> prt_neighbors) {
    auto neighbors = *prt_neighbors;

    // BlockSyncManager::Instance()->NewBroadcastMessage(message);
    auto gossip_max_hop_num = kGossipDefaultMaxHopNum;
    if (message.gossip().max_hop_num() > 0) {
        gossip_max_hop_num = message.gossip().max_hop_num();
    }
    if (gossip_max_hop_num <= message.hop_num()) {
        tcash_INFO("message.type(%d) hop_num(%d) larger than gossip_max_hop_num(%d)", message.type(), message.hop_num(), gossip_max_hop_num);
        return;
    }

    MessageKey msg_key(message.msg_hash());
    if (MessageWithBloomfilter::Instance()->StcashGossip(msg_key, message.gossip().stcash_times())) {
        tcash_DEBUG("stcash gossip for message.type(%d) hop_num(%d)", message.type(), message.gossip().stcash_times());
        return;
    }
    auto bloomfilter = MessageWithBloomfilter::Instance()->GetMessageBloomfilter(message);

    assert(bloomfilter);
    if (!bloomfilter) {
        tcash_WARN2("bloomfilter invalid");
        return;
    }
    bloomfilter->Add(local_hash64);

    std::vector<kadmlia::NodeInfoPtr> tmp_neighbors;
    uint32_t filtered = 0;
    for (auto iter = neighbors.begin(); iter != neighbors.end(); ++iter) {
        if ((*iter)->hash64 == 0) {
            tcash_WARN("node:%s hash64 empty, invalid", HexEncode((*iter)->node_id).c_str());
            continue;
        }

        if (bloomfilter->Contain((*iter)->hash64)) {
            ++filtered;
            continue;
        }

        tmp_neighbors.push_back(*iter);
    }

    std::vector<kadmlia::NodeInfoPtr> rest_random_neighbors;
    rest_random_neighbors = GetRandomNodes(tmp_neighbors, GetNeighborCount(message));
    if (rest_random_neighbors.empty()) {
        tcash_DEBUG("stcash Broadcast, rest_random_neighbors empty, broadcast failed, msg.hop_num(%d), msg.type(%d)", message.hop_num(), message.type());
        return;
    }

    if (message.hop_num() > kGossipBloomfilterIgnoreLevel) {
        for (auto iter = rest_random_neighbors.begin(); iter != rest_random_neighbors.end(); ++iter) {
            bloomfilter->Add((*iter)->hash64);
        }
    }

    const std::vector<uint64_t> & bloomfilter_vec = bloomfilter->Uint64Vector();
    message.clear_bloomfilter();
    for (uint32_t i = 0; i < bloomfilter_vec.size(); ++i) {
        message.add_bloomfilter(bloomfilter_vec[i]);
    }

    tcash_DEBUG("GossipBloomfilter Broadcast finally %d neighbors", rest_random_neighbors.size());
    if (message.has_is_root() && message.is_root()) {
        MutableSend(message, rest_random_neighbors);
        tcash_DEBUG("send gossip of kRoot or super broadcast");
    } else {
        Send(message, rest_random_neighbors);
    }
}

}  // namespace gossip

}  // namespace tcash
