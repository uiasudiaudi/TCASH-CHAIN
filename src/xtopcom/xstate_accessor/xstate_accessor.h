// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xbyte_buffer.h"
#include "xbasic/xmemory.hpp"
#include "xcommon/xaddress.h"
#include "xcommon/xsymbol.h"
#include "xcommon/xtoken.h"
#include "xstate_accessor/xaccess_control_data.h"
#include "xstate_accessor/xerror/xerror.h"
#include "xstate_accessor/xproperties/xproperty_identifier.h"
#include "xvledger/xvcanvas.h"
#include "xvledger/xvstate.h"

#include <memory>
#include <string>
#include <system_error>
#include <type_traits>

NS_BEG2(tcash, state_accessor)

class xtcash_state_accessor {
public:
    static constexpr size_t property_name_max_length{ 17 };
    static constexpr size_t property_name_min_length{ 2 };

private:
    tcash::xobject_ptr_t<tcash::base::xvbstate_t> bstate_owned_{nullptr};
    tcash::observer_ptr<tcash::base::xvbstate_t> bstate_;
    tcash::xobject_ptr_t<tcash::base::xvcanvas_t> canvas_;
    xstate_access_control_data_t ac_data_{};

public:
    xtcash_state_accessor(xtcash_state_accessor const &) = delete;
    xtcash_state_accessor & operator=(xtcash_state_accessor const &) = delete;
    xtcash_state_accessor(xtcash_state_accessor &&) = default;
    xtcash_state_accessor & operator=(xtcash_state_accessor &&) = default;
    ~xtcash_state_accessor() = default;

    xtcash_state_accessor(tcash::observer_ptr<tcash::base::xvbstate_t> const & bstate, xstate_access_control_data_t ac_data);
    xtcash_state_accessor(tcash::observer_ptr<tcash::base::xvbstate_t> const & bstate, tcash::xobject_ptr_t<tcash::base::xvcanvas_t> const & canvas);

private:
    explicit xtcash_state_accessor(common::xaccount_address_t const & account_address);
    xtcash_state_accessor(common::xaccount_address_t const & account_address, uint64_t const height);

public:
    /// @brief Construct an xstate_accessor_t object against the specified account. Throws xtcash_error_t when any error occurs.
    /// @param account_address The account address the state accessor object associated.
    /// @return The state accessor object.
    static std::unique_ptr<xtcash_state_accessor> build_from(common::xaccount_address_t const & account_address);

    /// @brief Construct an xstate_accessor_t object against the specified account.
    /// @param account_address The account address the state accessor object associated.
    /// @param ec Log the error code.
    /// @return The state accessor object.
    static std::unique_ptr<xtcash_state_accessor> build_from(common::xaccount_address_t const & account_address, std::error_code & ec);

    /// @brief Construct an xstate_accessor_t object against the specified account. Throws xtcash_error_t when any error occurs.
    /// @param account_address The account address the state accessor object associated.
    /// @param height The state at the specified height.
    /// @return The state accessor object.
    static std::unique_ptr<xtcash_state_accessor> build_from(common::xaccount_address_t const & account_address, std::uint64_t const height);

    /// @brief Construct an xstate_accessor_t object against the specified account.
    /// @param account_address The account address the state accessor object associated.
    /// @param height The state at the specified height.
    /// @param ec Log the error code.
    /// @return The state accessor object.
    static std::unique_ptr<xtcash_state_accessor> build_from(common::xaccount_address_t const & account_address, std::uint64_t const height, std::error_code & ec);

    /// @brief Withdraw token.
    /// @param property_id Property ID.
    /// @param symbol Token symbol.
    /// @param amount Amount to withdraw.
    /// @param ec Log the error code in the operation.
    /// @return Amount of token withdrew.
    common::xtoken_t withdraw(properties::xproperty_identifier_t const & property_id, common::xsymbol_t const & symbol, evm_common::u256 amount, std::error_code & ec);

    /// @brief Deposit token.
    /// @param property_id Property ID.
    /// @param amount Amount to deposit.
    /// @param ec Log the error code in the operation.
    void deposit(properties::xproperty_identifier_t const & property_id, common::xtoken_t amount, std::error_code & ec);

    /// @brief Get balance.
    /// @param property_id Name of balance property.
    /// @param ec Log the error code in the operation.
    /// @return The balance.
    evm_common::u256 balance(properties::xproperty_identifier_t const & property_id, std::error_code & ec) const;


    /// @brief Set new balance.
    /// @param property_id Name of balance property.
    /// @param new_balance New balance value.
    /// @param ec Error in the operation if any
    /// @return New balance value.
    evm_common::u256 balance(properties::xproperty_identifier_t const & property_id, evm_common::u256 const & new_balance, std::error_code & ec);

    /// @brief Get nonce.
    /// @param property_id Property ID.
    /// @param ec Log the error code in the operation.
    /// @return Nonce number.
    uint64_t nonce(properties::xproperty_identifier_t const & property_id, std::error_code & ec) const;

    /// @brief Create property.
    /// @param property_id Property ID.
    /// @param ec Log the error code in the operation.
    void create_property(properties::xproperty_identifier_t const & property_id, std::error_code & ec);

    /// @brief Get property.
    /// @param property_id Property ID.
    /// @param ec Log the error code in the operation.
    /// @return Property value.
    template <properties::xproperty_type_t PropertyTypeV>
    typename properties::xtype_of_t<PropertyTypeV>::type get_property(properties::xtypeless_property_identifier_t const & property_id, std::error_code & ec) const;

    /// @brief Get property of other address.
    /// @param property_id Property ID.
    /// @param address The account address which has the specified property.
    /// @param ec Log the error code in the operation.
    /// @return Property value.
    template <properties::xproperty_type_t PropertyTypeV>
    typename properties::xtype_of_t<PropertyTypeV>::type get_property(properties::xtypeless_property_identifier_t const & property_id,
                                                                      common::xaccount_address_t const & address,
                                                                      std::error_code & ec) const;

    xbytes_t get_property_serialized_value(properties::xtypeless_property_identifier_t const & property_id, std::error_code & ec) const;

    /// @brief Set property.
    /// @param property_id Property ID.
    /// @param value Value to be set.
    /// @param ec Log the error code in the operation.
    template <properties::xproperty_type_t PropertyTypeV>
    void set_property(properties::xtypeless_property_identifier_t const & property_id, typename properties::xtype_of_t<PropertyTypeV>::type const & value, std::error_code & ec);

    /// @brief Clear property. This operation liks STL container's clear() API.
    /// @param property_id Property ID.
    /// @param ec Log the error code in the operation.
    void clear_property(properties::xproperty_identifier_t const & property_id, std::error_code & ec);

    void set_property_by_serialized_value(properties::xtypeless_property_identifier_t const & property_id, xbytes_t const & serialized_data, std::error_code & ec);

    /// @brief Update property cell value. Only map and deque are supported.
    /// @param proprty_id Property ID.
    /// @param key Cell position key.
    /// @param value Cell's new value.
    /// @param ec Log the error code in the operation.
    template <properties::xproperty_type_t PropertyTypeV, typename std::enable_if<PropertyTypeV == properties::xproperty_type_t::map ||
                                                                                  PropertyTypeV == properties::xproperty_type_t::deque>::type * = nullptr>
    void set_property_cell_value(properties::xtypeless_property_identifier_t const & proprty_id,
                                 typename properties::xkey_type_of_t<PropertyTypeV>::type const & key,
                                 typename properties::xvalue_type_of_t<PropertyTypeV>::type const & value,
                                 std::error_code & ec);

    /// @brief Get property cell value. Only map and deque are supported.
    /// @param property_id Property ID.
    /// @param key Cell position key.
    /// @param ec Log the error code in the operation.
    /// @return Cell value.
    template <properties::xproperty_type_t PropertyTypeV, typename std::enable_if<PropertyTypeV == properties::xproperty_type_t::map ||
                                                                                  PropertyTypeV == properties::xproperty_type_t::deque>::type * = nullptr>
    typename properties::xvalue_type_of_t<PropertyTypeV>::type get_property_cell_value(properties::xtypeless_property_identifier_t const & property_id,
                                                                                       typename properties::xkey_type_of_t<PropertyTypeV>::type const & key,
                                                                                       std::error_code & ec) const;

    /// @brief Check if property cell key exist. Only map and deque are supported.
    /// @param property_id Property ID.
    /// @param key Cell position key.
    /// @param ec Log the error code in the operation.
    /// @return exist or not.
    template <properties::xproperty_type_t PropertyTypeV, typename std::enable_if<PropertyTypeV == properties::xproperty_type_t::map || 
                                                                                  PropertyTypeV == properties::xproperty_type_t::deque>::type * = nullptr>
    bool exist_property_cell_key(properties::xtypeless_property_identifier_t const & property_id,
                                 typename properties::xkey_type_of_t<PropertyTypeV>::type const & key,
                                 std::error_code & ec) const;

    /// @brief Remove property cell at the position key. Only map and deque are supported.
    /// @param property_id Property ID.
    /// @param key Cell position key.
    /// @param ec Log the error code in the operation.
    template <properties::xproperty_type_t PropertyTypeV, typename std::enable_if<PropertyTypeV == properties::xproperty_type_t::map ||
                                                                                  PropertyTypeV == properties::xproperty_type_t::deque>::type * = nullptr>
    void remove_property_cell(properties::xtypeless_property_identifier_t const & property_id, typename properties::xkey_type_of_t<PropertyTypeV>::type const & key, std::error_code & ec);

    /// @brief Get property size.
    /// @param property_id Property ID.
    /// @param ec Log error code in the operation.
    /// @return Property size.
    size_t property_size(properties::xproperty_identifier_t const & property_id, std::error_code & ec) const;

    /// @brief Check if the property identified by the property ID exists or not.
    /// @param property_id Property ID.
    /// @param ec Log the error code in the operation.
    /// @return 'true' if property exists; otherwise 'false'.
    bool property_exist(properties::xproperty_identifier_t const & property_id, std::error_code & ec) const;

    /// @brief Get byte-code.
    /// @param property_id Property ID.
    /// @param ec Log the error code in the operation.
    /// @return Byte-code.
    xbyte_buffer_t bin_code(properties::xproperty_identifier_t const & property_id, std::error_code & ec) const;

    /// @brief Deploy byte-code.
    /// @param property_id
    /// @param bin_code Byte-code.
    /// @param ec Log the error code in the operation.
    void deploy_bin_code(properties::xproperty_identifier_t const & property_id, xbyte_buffer_t const & bin_code, std::error_code & ec);

    /// @brief Get the state change binlog.
    /// @param ec Log the error code in getting binlog.
    /// @return The binlog data stored in string object.
    std::string binlog(std::error_code & ec) const;

    /// @brief Get the size of state change binlog.
    /// @param ec Log the error code in getting binlog.
    /// @return The binlog data size.
    size_t binlog_size(std::error_code & ec) const;

    /// @brief Get the fullstate binary data.
    /// @param ec Log the error code in getting the data.
    /// @return The full state binary data stored in the string object.
    std::string fullstate_bin(std::error_code & ec) const;

    /// @brief Get state associated account address.
    /// @return The state associated account address.
    common::xaccount_address_t account_address() const;

    /// @brief Get the state associated chain height.
    /// @return The state associated chain height.
    uint64_t height() const;

private:
    bool read_permitted(properties::xproperty_identifier_t const & property_id) const noexcept;
    bool write_permitted(properties::xproperty_identifier_t const & property_id) const noexcept;
    bool read_permitted(std::string const & property_full_name) const noexcept;

    /// @brief Create bytes property.
    /// @param property_name Property name.
    /// @param ec Log error in the process.
    void do_create_bytes_property(std::string const & property_name, std::error_code & ec);

    /// @brief Create string property.
    /// @param property_name Property name.
    /// @param ec Log error in the process.
    void do_create_string_property(std::string const & property_name, std::error_code & ec);

    /// @brief Create map property.
    /// @param property_name Property name.
    /// @param ec Log error in the process.
    void do_create_map_property(std::string const & property_name, std::error_code & ec);

    /// @brief Create integer property.
    /// @param property_name Property name.
    /// @param ec Log error in the process.
    template <properties::xproperty_type_t PropertyTypeV>
    void do_create_int_property(std::string const & property_name, std::error_code & ec);

    /// @brief Create token property.
    /// @param property_name The property name.
    /// @param ec Store the error code in the creation process.
    void do_create_token_property(std::string const & property_name, std::error_code & ec);

    /// @brief Get the state of specific address.
    /// @param address Address to get.
    /// @param ec Log the error code in getting the data.
    /// @return The state of address.
    xobject_ptr_t<base::xvbstate_t> state(common::xaccount_address_t const & address, std::error_code & ec) const;
};
using xstate_accessor_t = xtcash_state_accessor;

#include "xstate_accessor/xstate_accessor_impl.h"

NS_END2
