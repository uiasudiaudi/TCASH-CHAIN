// Copyright (c) 2017-2019 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xgossip/gossip_interface.h"
#include "xtransport/transport.h"

namespace tcash {

namespace gossip {

class GossipDispatcher : public GossipInterface {
public:
    explicit GossipDispatcher(transport::TransportPtr transport_ptr);
    virtual ~GossipDispatcher();
    void Broadcast(transport::protobuf::RoutingMessage & message, kadmlia::ElectRoutingTablePtr const & routing_table) override;

private:
    void GenerateDispatchInfos(transport::protobuf::RoutingMessage & message,
                               kadmlia::ElectRoutingTablePtr const & routing_table,
                               std::vector<DispatchInfos> & select_nodes,
                               bool is_broadcast_height);
};

}  // namespace gossip

}  // namespace tcash