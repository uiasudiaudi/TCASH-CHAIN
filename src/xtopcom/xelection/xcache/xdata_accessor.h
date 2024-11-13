// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xmemory.hpp"
#include "xchain_timer/xchain_timer_face.h"
#include "xcommon/xip.h"
#include "xdata/xelection/xelection_cluster_result.h"
#include "xdata/xelection/xelection_group_result.h"
#include "xelection/xcache/xdata_accessor_face.h"
#include "xelection/xcache/xnetwork_element.h"

#include <memory>
#include <unordered_map>

NS_BEG3(tcash, election, cache)

class xtcash_data_accessor final : public xdata_accessor_face_t {
private:
    std::shared_ptr<xnetwork_element_t> m_network_element;
    observer_ptr<time::xchain_time_face_t> m_logic_timer;

public:
    xtcash_data_accessor(xtcash_data_accessor const &) = delete;
    xtcash_data_accessor & operator=(xtcash_data_accessor const &) = delete;
    xtcash_data_accessor(xtcash_data_accessor &&) = default;
    xtcash_data_accessor & operator=(xtcash_data_accessor &&) = default;
    ~xtcash_data_accessor() override = default;

    xtcash_data_accessor(common::xnetwork_id_t const & network_id, observer_ptr<time::xchain_time_face_t> const & chain_timer);

    common::xnetwork_id_t network_id() const noexcept override;

    std::unordered_map<common::xcluster_address_t, xgroup_update_result_t> update_zone(common::xzone_id_t const & zone_id,
                                                                                       data::election::xelection_result_store_t const & election_result_store,
                                                                                       std::uint64_t const associated_blk_height,
                                                                                       std::error_code & ec) override;

    std::map<common::xslot_id_t, data::xnode_info_t> sharding_nodes(common::xcluster_address_t const & address,
                                                                    common::xelection_round_t const & election_round,
                                                                    std::error_code & ec) const override;

    std::map<common::xslot_id_t, data::xnode_info_t> group_nodes(common::xgroup_address_t const & group_address,
                                                                 common::xlogic_epoch_t const & group_logic_epoch,
                                                                 std::error_code & ec) const override;

    common::xnode_address_t parent_address(common::xgroup_address_t const & child_address,
                                           common::xelection_round_t const & child_election_round,
                                           std::error_code & ec) const noexcept override;

    common::xnode_address_t parent_address(common::xgroup_address_t const & child_address,
                                           common::xlogic_epoch_t const & child_logic_epoch,
                                           std::error_code & ec) const noexcept override;

    std::vector<common::xnode_address_t> child_addresses(common::xgroup_address_t const & parent_group_address,
                                                         common::xlogic_epoch_t const & parent_logic_epoch,
                                                         std::error_code & ec) const noexcept override;

    std::shared_ptr<xnode_element_t> node_element(common::xgroup_address_t const & address,
                                                  common::xlogic_epoch_t const & logic_epoch,
                                                  common::xslot_id_t const & slot_id,
                                                  std::error_code & ec) const override;

    common::xaccount_address_t account_address_from(common::xip2_t const & xip2, std::error_code & ec) const override;

    std::shared_ptr<xgroup_element_t> group_element(common::xgroup_address_t const & sharding_address, common::xelection_round_t const & election_round, std::error_code & ec) const override;
    std::shared_ptr<xgroup_element_t> group_element(common::xgroup_address_t const & group_address,
                                                    common::xlogic_epoch_t const & logic_epoch,
                                                    std::error_code & ec) const override;

    std::shared_ptr<xgroup_element_t> group_element_by_height(common::xgroup_address_t const & group_address,
                                                              uint64_t const election_blk_height,
                                                              std::error_code & ec) const override;

    std::shared_ptr<xgroup_element_t> group_element_by_logic_time(common::xgroup_address_t const & sharding_address,
                                                                  common::xlogic_time_t const logic_time,
                                                                  std::error_code & ec) const override;

    std::shared_ptr<xgroup_element_t> parent_group_element(common::xgroup_address_t const & child_sharding_address,
                                                           common::xelection_round_t const & child_group_election_round,
                                                           std::error_code & ec) const override;

    std::shared_ptr<xgroup_element_t> parent_group_element(common::xgroup_address_t const & child_group_address,
                                                           common::xlogic_epoch_t const & child_logic_epoch,
                                                           std::error_code & ec) const override;

    common::xelection_round_t election_epoch_from(common::xip2_t const & xip2, std::error_code & ec) const override;

private:
    std::unordered_map<common::xcluster_address_t, xgroup_update_result_t> update_zone(std::shared_ptr<xzone_element_t> const & zone_element,
                                                                                       data::election::xelection_result_store_t const & election_result_store,
                                                                                       std::uint64_t const associated_blk_height,
                                                                                       std::error_code & ec);

    std::unordered_map<common::xgroup_address_t, xgroup_update_result_t> update_committee_zone(std::shared_ptr<xzone_element_t> const & zone_element,
                                                                                                  data::election::xelection_result_store_t const & election_result_store,
                                                                                                  std::uint64_t const associated_blk_height,
                                                                                                  std::error_code & ec);

    std::unordered_map<common::xgroup_address_t, xgroup_update_result_t> update_consensus_zone(std::shared_ptr<xzone_element_t> const & zone_element,
                                                                                                  data::election::xelection_result_store_t const & election_result_store,
                                                                                                  std::uint64_t const associated_blk_height,
                                                                                                  std::error_code & ec);

    std::unordered_map<common::xgroup_address_t, xgroup_update_result_t> update_edge_zone(std::shared_ptr<xzone_element_t> const & zone_element,
                                                                                             data::election::xelection_result_store_t const & election_result_store,
                                                                                             std::uint64_t const associated_blk_height,
                                                                                             std::error_code & ec);

    std::unordered_map<common::xgroup_address_t, xgroup_update_result_t> update_storage_zone(std::shared_ptr<xzone_element_t> const & zone_element,
                                                                                                data::election::xelection_result_store_t const & election_result_store,
                                                                                                std::uint64_t const associated_blk_height,
                                                                                                std::error_code & ec);

    std::unordered_map<common::xgroup_address_t, xgroup_update_result_t> update_fullnode_zone(std::shared_ptr<xzone_element_t> const & zone_element,
                                                                                                 data::election::xelection_result_store_t const & election_result_store,
                                                                                                 std::uint64_t const associated_blk_height,
                                                                                                 std::error_code & ec);

    std::unordered_map<common::xgroup_address_t, xgroup_update_result_t> update_zec_zone(std::shared_ptr<xzone_element_t> const & zone_element,
                                                                                            data::election::xelection_result_store_t const & election_result_store,
                                                                                            std::uint64_t const associated_blk_height,
                                                                                            std::error_code & ec);

    std::unordered_map<common::xgroup_address_t, xgroup_update_result_t> update_frozen_zone(std::shared_ptr<xzone_element_t> const & zone_element,
                                                                                               data::election::xelection_result_store_t const & election_result_store,
                                                                                               std::uint64_t const associated_blk_height,
                                                                                               std::error_code & ec);

    std::unordered_map<common::xgroup_address_t, xgroup_update_result_t> update_eth_zone(std::shared_ptr<xzone_element_t> const & zone_element,
                                                                                         data::election::xelection_result_store_t const & election_result_store,
                                                                                         std::uint64_t const associated_blk_height,
                                                                                         std::error_code & ec);

    std::unordered_map<common::xgroup_address_t, xgroup_update_result_t> update_relay_zone(std::shared_ptr<xzone_element_t> const & zone_element,
                                                                                           data::election::xelection_result_store_t const & election_result_store,
                                                                                           std::uint64_t const associated_blk_height,
                                                                                           std::error_code & ec);

    std::unordered_map<common::xcluster_address_t, xgroup_update_result_t> update_cluster(std::shared_ptr<xzone_element_t> const & zone_element,
                                                                                          std::shared_ptr<xcluster_element_t> const & cluster_element,
                                                                                          data::election::xelection_cluster_result_t const & cluster_result,
                                                                                          std::uint64_t const associated_blk_height,
                                                                                          std::error_code & ec);

    void update_group(std::shared_ptr<xzone_element_t> const & zone_element,
                      std::shared_ptr<xcluster_element_t> const & cluster_element,
                      std::shared_ptr<xgroup_element_t> const & group_element,
                      data::election::xelection_group_result_t const & group_result,
                      std::error_code & ec);

    std::shared_ptr<xgroup_element_t> group_element(common::xnetwork_id_t const & network_id,
                                                    common::xzone_id_t const & zone_id,
                                                    common::xcluster_id_t const & cluster_id,
                                                    common::xgroup_id_t const & group_id,
                                                    common::xelection_round_t const & election_round,
                                                    std::error_code & ec) const;

    std::shared_ptr<xgroup_element_t> group_element(common::xnetwork_id_t const & network_id,
                                                    common::xzone_id_t const & zone_id,
                                                    common::xcluster_id_t const & cluster_id,
                                                    common::xgroup_id_t const & group_id,
                                                    common::xlogic_epoch_t const & logic_epoch,
                                                    std::error_code & ec) const;

    std::shared_ptr<xgroup_element_t> group_element_by_height(common::xnetwork_id_t const & network_id,
                                                              common::xzone_id_t const & zone_id,
                                                              common::xcluster_id_t const & cluster_id,
                                                              common::xgroup_id_t const & group_id,
                                                              uint64_t const election_block_height,
                                                              std::error_code & ec) const;

    std::shared_ptr<xgroup_element_t> group_element_by_logic_time(common::xnetwork_id_t const & network_id,
                                                                  common::xzone_id_t const & zone_id,
                                                                  common::xcluster_id_t const & cluster_id,
                                                                  common::xgroup_id_t const & group_id,
                                                                  common::xlogic_time_t const logic_time,
                                                                  std::error_code & ec) const;

    std::shared_ptr<xcluster_element_t> cluster_element(common::xnetwork_id_t const & network_id,
                                                        common::xzone_id_t const & zone_id,
                                                        common::xcluster_id_t const & cluster_id,
                                                        std::error_code & ec) const;
};
using xdata_accessor_t = xtcash_data_accessor;

NS_END3
