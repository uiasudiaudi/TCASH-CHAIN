// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xgossip/gossip_interface.h"
#include "xtransport/transport.h"

#include <atomic>

namespace tcash {

namespace gossip {

class GossipRRS : public GossipInterface {
public:
    explicit GossipRRS(transport::TransportPtr transport_ptr);
    virtual ~GossipRRS();
    void Broadcast(uint64_t local_hash64, transport::protobuf::RoutingMessage & message, std::shared_ptr<std::vector<kadmlia::NodeInfoPtr>> neighbors) override;

    void BroadcastHash(transport::protobuf::RoutingMessage & message, std::vector<kadmlia::NodeInfoPtr> & neighbors);

    void update_params(uint32_t t, uint32_t k);

private:
    std::atomic<uint32_t> rrs_params_neighbour_num;  // kGossipRRSNeighborNum
    std::atomic<uint32_t> rrs_params_switch_hash_hop_num;  // kGossipRRSSwitchLayerHopNum
};

}  // namespace gossip
}  // namespace tcash