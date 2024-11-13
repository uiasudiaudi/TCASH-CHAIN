// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xmemory.hpp"
#include "xbasic/xutility.h"
#include "xcommon/xaddress.h"
#include "xcontract_runtime/xblock_sniff_config.h"
#include "xdata/xgenesis_data.h"
#include "xsystem_contract_runtime/xsystem_contract_object_creator.h"
#include "xsystem_contracts/xbasic_system_contract.h"
#include "xvledger/xvblockstore.h"
#include "xvledger/xvledger.h"

#include <system_error>
#include <unordered_map>

NS_BEG3(tcash, contract_runtime, system)

struct xtcash_contract_deployment_data {
    common::xnode_type_t node_type{common::xnode_type_t::invalid};
    xsniff_type_t sniff_type{xsniff_type_t::none};
    xsniff_broadcast_config_t broadcast_config{};
    xsniff_timer_config_t timer_config{};
    xsniff_block_config_t block_config{};

    xtcash_contract_deployment_data() = default;
    xtcash_contract_deployment_data(xtcash_contract_deployment_data const &) = default;
    xtcash_contract_deployment_data & operator=(xtcash_contract_deployment_data const &) = default;
    xtcash_contract_deployment_data(xtcash_contract_deployment_data &&) = default;
    xtcash_contract_deployment_data & operator=(xtcash_contract_deployment_data &&) = default;
    ~xtcash_contract_deployment_data() = default;

    xtcash_contract_deployment_data(common::xnode_type_t node_type,
                                  xsniff_type_t sniff_type,
                                  xsniff_broadcast_config_t broadcast_config,
                                  xsniff_timer_config_t timer_config,
                                  xsniff_block_config_t block_config);
};
using xcontract_deployment_data_t = xtcash_contract_deployment_data;

class xtcash_system_contract_manager {
private:
    std::unordered_map<common::xaccount_address_t, xcontract_deployment_data_t> m_system_contract_deployment_data;
    std::unordered_map<common::xaccount_base_address_t, std::unique_ptr<xcontract_object_creator_t>> m_system_contract_creators;

public:
    xtcash_system_contract_manager() = default;
    xtcash_system_contract_manager(xtcash_system_contract_manager const &) = delete;
    xtcash_system_contract_manager & operator=(xtcash_system_contract_manager const &) = delete;
    xtcash_system_contract_manager(xtcash_system_contract_manager &&) = default;
    xtcash_system_contract_manager & operator=(xtcash_system_contract_manager &&) = default;
    ~xtcash_system_contract_manager() = default;

    /**
     * @brief get an instance
     *
     * @return xtcash_system_contract_manager&
     */
    static xtcash_system_contract_manager * instance() {
        static auto * inst = new xtcash_system_contract_manager();
        return inst;
    }

    // void deploy(observer_ptr<base::xvblockstore_t> const & blockstore);
    std::unordered_map<common::xaccount_address_t, xcontract_deployment_data_t> const & deployment_data() const noexcept;
    std::unique_ptr<contract_common::xbasic_contract_t> system_contract(common::xaccount_address_t const & address, std::error_code & ec) const noexcept;
    std::unique_ptr<contract_common::xbasic_contract_t> system_contract(common::xaccount_address_t const & address) const;

private:
    template <typename system_contract_type>
    void deploy_system_contract(common::xaccount_address_t const & address,
                                common::xnode_type_t node_type,
                                xsniff_type_t sniff_type,
                                xsniff_broadcast_config_t broadcast_config,
                                xsniff_timer_config_t timer_config,
                                xsniff_block_config_t block_config,
                                observer_ptr<base::xvblockstore_t> const & blockstore);

    void init_system_contract(common::xaccount_address_t const & contract_address, observer_ptr<base::xvblockstore_t> const & blockstore);

    bool contains(common::xaccount_address_t const & address) const noexcept;
};
using xsystem_contract_manager_t = xtcash_system_contract_manager;

template <typename system_contract_type>
void xtcash_system_contract_manager::deploy_system_contract(common::xaccount_address_t const & address,
                                                          common::xnode_type_t node_type,
                                                          xsniff_type_t sniff_type,
                                                          xsniff_broadcast_config_t broadcast_config,
                                                          xsniff_timer_config_t timer_config,
                                                          xsniff_block_config_t block_config,
                                                          observer_ptr<base::xvblockstore_t> const & blockstore) {
    // must system contract & not deploy yet
    assert(address.type() == base::enum_vaccount_addr_type_native_contract);
    assert(!contains(address));

    xcontract_deployment_data_t data{node_type, sniff_type, broadcast_config, std::move(timer_config), std::move(block_config)};
#if !defined(NDEBUG)
    auto r1 =
#endif
    m_system_contract_deployment_data.insert(std::make_pair(address, std::move(data)));
    assert(tcash::get<bool>(r1));

    auto sys_contract_creator = tcash::make_unique<xsystem_contract_object_creator_t<system_contract_type>>();
#if !defined(NDEBUG)
    auto r2 =
#endif
    m_system_contract_creators.emplace(address.base_address(), std::move(sys_contract_creator));
    assert(tcash::get<bool>(r2));

    if (address.ledger_id().zone_id() == common::xconsensus_zone_id) {
        for (auto i = 0; i < enum_vbucket_has_tables_count; i++) {
            auto table_address = common::xaccount_address_t{address.base_address(), static_cast<uint16_t>(i)};
            init_system_contract(table_address, blockstore);
        }
    } else {
        init_system_contract(address, blockstore);
    }
}

NS_END3
