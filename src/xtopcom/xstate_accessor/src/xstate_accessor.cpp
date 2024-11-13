// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xstate_accessor/xstate_accessor.h"

#include "xbasic/xutility.h"
#include "xconfig/xconfig_register.h"
#include "xconfig/xpredefined_configurations.h"
#include "xcommon/common_data.h"
#include "xmetrics/xmetrics.h"
#include "xstate_accessor/xerror/xerror.h"
#include "xvledger/xvledger.h"
#include "xstatestore/xstatestore_face.h"

#include <cassert>
#include <cinttypes>

NS_BEG2(tcash, state_accessor)

constexpr size_t xtcash_state_accessor::property_name_max_length;
constexpr size_t xtcash_state_accessor::property_name_min_length;

static xobject_ptr_t<base::xvbstate_t> state(common::xaccount_address_t const & address) {

#if defined(XENABLE_TESTS)
    // auto * bstate = new base::xvbstate_t{address.to_string(), 0, 0, "", "", 0, 0, 0};
    xobject_ptr_t<base::xvbstate_t> address_bstate = make_object_ptr<base::xvbstate_t>(address.to_string(),
                                                                                       static_cast<uint64_t>(0),
                                                                                       static_cast<uint64_t>(0),
                                                                                       std::string{},
                                                                                       std::string{},
                                                                                       static_cast<uint64_t>(0),
                                                                                       static_cast<uint32_t>(0),
                                                                                       static_cast<uint16_t>(0));
    return address_bstate;
#else
    data::xunitstate_ptr_t unitstate = nullptr;//statestore::xstatestore_hub_t::instance()->get_unitstate(LatestConnectBlock, address); // TODO(jimmy)
    if (unitstate == nullptr) {
        xerror("[state_accessor::state] get latest connectted state none, account=%s", address.to_string().c_str());
        tcash::error::throw_error(error::xenum_errc::load_account_state_failed);
        return nullptr;
    }
    xdbg("[state_accessor::state] get latest connectted state success, account=%s, height=%ld", address.to_string().c_str(), unitstate->height());
    return unitstate->get_bstate();
#endif
}

static xobject_ptr_t<base::xvbstate_t> state(common::xaccount_address_t const & address, uint64_t const height) {
    data::xunitstate_ptr_t unitstate = nullptr;//statestore::xstatestore_hub_t::instance()->get_unit_committed_state(address, height); // TODO(jimmy)
    if (unitstate == nullptr) {
        xerror("[xtcash_state_accessor::get_property] get committed state none at height %" PRIu64 ", account=%s", height, address.to_string().c_str());
        tcash::error::throw_error(error::xenum_errc::load_account_state_failed);
        return nullptr;
    }
    xdbg("[xtcash_state_accessor::get_property] get latest connectted state success, account=%s, height=%ld", address.to_string().c_str(), unitstate->height());
    return unitstate->get_bstate();
}

xtcash_state_accessor::xtcash_state_accessor(tcash::observer_ptr<tcash::base::xvbstate_t> const & bstate, xstate_access_control_data_t ac_data)
  : bstate_{bstate}, canvas_{make_object_ptr<base::xvcanvas_t>()}, ac_data_{std::move(ac_data)} {
    if (bstate == nullptr) {
        assert(false);
        tcash::error::throw_error({error::xerrc_t::invalid_state_backend});
    }
}

xtcash_state_accessor::xtcash_state_accessor(tcash::observer_ptr<tcash::base::xvbstate_t> const & bstate, tcash::xobject_ptr_t<tcash::base::xvcanvas_t> const & canvas)
  : bstate_{bstate}, canvas_{canvas}, ac_data_{} {
    if (bstate == nullptr) {
        assert(false);
        tcash::error::throw_error({error::xerrc_t::invalid_state_backend});
    }
}

xtcash_state_accessor::xtcash_state_accessor(common::xaccount_address_t const & account_address)
  : bstate_owned_{tcash::state_accessor::state(account_address)}, bstate_{make_observer(bstate_owned_.get())}, canvas_{make_object_ptr<base::xvcanvas_t>()} {
}

xtcash_state_accessor::xtcash_state_accessor(common::xaccount_address_t const & account_address, uint64_t const height)
  : bstate_owned_{tcash::state_accessor::state(account_address, height)}, bstate_{make_observer(bstate_owned_.get())}, canvas_{make_object_ptr<base::xvcanvas_t>()} {
}

std::unique_ptr<xtcash_state_accessor> xtcash_state_accessor::build_from(common::xaccount_address_t const & account_address) {
    auto * state_accessor = new xtcash_state_accessor{account_address};
    return std::unique_ptr<xtcash_state_accessor>(state_accessor);
}

std::unique_ptr<xtcash_state_accessor> xtcash_state_accessor::build_from(common::xaccount_address_t const & account_address, std::error_code & ec) {
    assert(!ec);

    try {
        return build_from(account_address);
    } catch (tcash::error::xtcash_error_t const & eh) {
        ec = eh.code();
    }

    return {};
}

std::unique_ptr<xtcash_state_accessor> xtcash_state_accessor::build_from(common::xaccount_address_t const & account_address, uint64_t const height) {
    auto * state_accessor = new xtcash_state_accessor{account_address, height};
    return std::unique_ptr<xtcash_state_accessor>(state_accessor);
}

std::unique_ptr<xtcash_state_accessor> xtcash_state_accessor::build_from(common::xaccount_address_t const & account_address, uint64_t const height, std::error_code & ec) {
    assert(!ec);

    try {
        return build_from(account_address, height);
    } catch (tcash::error::xtcash_error_t const & eh) {
        ec = eh.code();
    }

    return {};
}

uint64_t xtcash_state_accessor::nonce(properties::xproperty_identifier_t const & property_id, std::error_code & ec) const {
    assert(!ec);
    assert(bstate_ != nullptr);

    auto const & property_name = property_id.full_name();
    xobject_ptr_t<base::xnoncevar_t> nonce_property = bstate_->load_nonce_var(property_name);
    if (nonce_property == nullptr) {
        if (system_property(property_id)) {
            nonce_property = bstate_->new_nonce_var(property_name, canvas_.get());
        } else {
            ec = error::xerrc_t::property_not_exist;
            return 0;
        }
    }
    assert(nonce_property != nullptr);
    return nonce_property->get_nonce();
}

static std::string token_property_name(properties::xproperty_identifier_t const & property_id, common::xsymbol_t const & symbol) {
    // return property_id.full_name() + "_" + symbol.to_string();
    return property_id.full_name();
}

common::xtoken_t xtcash_state_accessor::withdraw(properties::xproperty_identifier_t const & property_id, common::xsymbol_t const & symbol, evm_common::u256 const amount, std::error_code & ec) {
    assert(!ec);
    assert(bstate_ != nullptr);

    if (property_id.type() != properties::xproperty_type_t::token) {
        ec = error::xerrc_t::invalid_property_type;
        return common::xtoken_t{};
    }

    if (!write_permitted(property_id)) {
        ec = error::xerrc_t::property_access_denied;
        return common::xtoken_t{};
    }

    auto const & property_name = token_property_name(property_id, symbol);

    if (!bstate_->find_property(property_name)) {
        ec = error::xerrc_t::token_insufficient;
        return common::xtoken_t{ symbol };
    }

    auto token_property = bstate_->load_string_var(property_name);
    if (token_property == nullptr) {
        ec = error::xerrc_t::load_property_failed;
        return common::xtoken_t{ symbol };
    }

    auto const balance_string = token_property->query();
    evm_common::u256 balance = evm_common::fromBigEndian<evm_common::u256>(balance_string);
    evm_common::u256 withdraw_amount = amount;

    if (amount > balance) {
        ec = error::xerrc_t::token_insufficient;
        return common::xtoken_t{symbol};
    }

    auto const new_balance = balance - withdraw_amount;
    if (token_property->reset(evm_common::toBigEndianString(new_balance), canvas_.get()) == false) {
        ec = error::xerrc_t::update_property_failed;
        return common::xtoken_t{symbol};
    }

    assert(new_balance <= balance); // allow with zero
    return common::xtoken_t{ amount, symbol };
}

void xtcash_state_accessor::deposit(properties::xproperty_identifier_t const & property_id, common::xtoken_t token, std::error_code & ec) {
    assert(!ec);

    if (property_id.type() != properties::xproperty_type_t::token) {
        ec = error::xerrc_t::invalid_property_type;
        return;
    }

    if (!write_permitted(property_id)) {
        ec = error::xerrc_t::property_access_denied;
        return;
    }

    auto const & property_name = token_property_name(property_id, token.symbol());

    xobject_ptr_t<base::xstringvar_t> token_property{ nullptr };
    if (!bstate_->find_property(property_name)) {
        if (properties::system_property(property_id)) {
            token_property = bstate_->new_string_var(property_name, canvas_.get());
        } else {
            ec = error::xerrc_t::property_not_exist;
            return;
        }
    } else {
        token_property = bstate_->load_string_var(property_name);
    }

    if (token_property == nullptr) {
        ec = error::xerrc_t::load_property_failed;
        return;
    }

    auto const balance_string = token_property->query();
    evm_common::u256 balance = evm_common::fromBigEndian<evm_common::u256>(balance_string);
    auto new_balance = balance + token.amount();
    if (token_property->reset(evm_common::toBigEndianString(balance), canvas_.get()) == false) {
        ec = error::xerrc_t::update_property_failed;
        return;
    }
    // auto const new_balance = token_property->deposit(token.amount().convert_to<tcash::base::vtoken_t>(), canvas_.get());
    if (new_balance < balance) {
        ec = error::xerrc_t::property_value_out_of_range;
        return;
    }

    token.clear();
}

evm_common::u256 xtcash_state_accessor::balance(properties::xproperty_identifier_t const & property_id, std::error_code & ec) const {
    assert(!ec);

    if (property_id.type() != properties::xproperty_type_t::token) {
        ec = error::xerrc_t::invalid_property_type;
        return 0 ;
    }

    if (!read_permitted(property_id)) {
        ec = error::xerrc_t::property_access_denied;
        return 0;
    }

    auto const & property_name = property_id.full_name();
    if (!bstate_->find_property(property_name)) {
        return 0;
    }

    auto const token_property = bstate_->load_token_var(property_name);
    if (token_property == nullptr) {
        ec = error::xerrc_t::load_property_failed;
        return 0;
    }

    return evm_common::u256{token_property->get_balance()};
}

evm_common::u256 xtcash_state_accessor::balance(properties::xproperty_identifier_t const & property_id, evm_common::u256 const & new_balance, std::error_code & ec) {
    assert(!ec);

    if (property_id.type() != properties::xproperty_type_t::token) {
        ec = error::xerrc_t::invalid_property_type;
        return 0;
    }

    if (!write_permitted(property_id)) {
        ec = error::xerrc_t::property_access_denied;
        return 0;
    }

    auto const & property_name = property_id.full_name();
    if (!bstate_->find_property(property_name)) {
        if (!properties::system_property(property_id)) {
            ec = error::xerrc_t::invalid_property_type;
            return 0;
        }

        bstate_->new_token_var(property_name, canvas_.get());
    }

    auto const token_property = bstate_->load_token_var(property_name);
    if (token_property == nullptr) {
        ec = error::xerrc_t::load_property_failed;
        return 0;
    }

    auto const new_balance_data = token_property->set(static_cast<base::vtoken_t>(new_balance.convert_to<uint64_t>()), canvas_.get());
    return evm_common::u256{new_balance_data};
}

void xtcash_state_accessor::create_property(properties::xproperty_identifier_t const & property_id, std::error_code & ec) {
    assert(!ec);

    if (properties::system_property(property_id)) {
        ec = error::xerrc_t::property_access_denied;
        return;
    }

    auto const & property_name = property_id.full_name();
    if (property_name.length() < property_name_min_length || property_name.length() >= property_name_max_length) {
        ec = error::xerrc_t::property_name_out_of_range;
        return;
    }

    if (bstate_->find_property(property_name)) {
        ec = error::xerrc_t::property_already_exist;
        return;
    }

    switch (property_id.type()) {
    case properties::xproperty_type_t::bytes:
        XATTRIBUTE_FALLTHROUGH;
    case properties::xproperty_type_t::string:
        do_create_string_property(property_name, ec);
        break;

    case properties::xproperty_type_t::map:
        do_create_map_property(property_name, ec);
        break;

    case properties::xproperty_type_t::deque:
        assert(false);
        break;

    case properties::xproperty_type_t::token:
        do_create_token_property(property_name, ec);
        break;

    default:
        assert(false);
        break;
    }
}

bool xtcash_state_accessor::read_permitted(properties::xproperty_identifier_t const & property_id) const noexcept {
    return true;
}

bool xtcash_state_accessor::write_permitted(properties::xproperty_identifier_t const & property_id) const noexcept {
    return true;
}

bool xtcash_state_accessor::read_permitted(std::string const & property_full_name) const noexcept {
    return true;
}

void xtcash_state_accessor::clear_property(properties::xproperty_identifier_t const & property_id, std::error_code & ec) {
    assert(!ec);
    assert(bstate_ != nullptr);

    if (!write_permitted(property_id)) {
        ec = error::xerrc_t::property_access_denied;
        return;
    }

    switch (property_id.type()) {
    case properties::xproperty_type_t::map:
    {
        auto map_property = bstate_->load_string_map_var(property_id.full_name());
        if (map_property == nullptr) {
            ec = error::xerrc_t::property_not_exist;
            return;
        }

        if (!map_property->clear(canvas_.get())) {
            ec = error::xerrc_t::update_property_failed;
            return;
        }
        break;
    }

    case properties::xproperty_type_t::bytes:
        XATTRIBUTE_FALLTHROUGH;
    case properties::xproperty_type_t::string:
    {
        auto string_property = bstate_->load_string_var(property_id.full_name());
        if (string_property == nullptr) {
            ec = error::xerrc_t::property_not_exist;
            return;
        }

        if (!string_property->clear(canvas_.get())) {
            ec = error::xerrc_t::update_property_failed;
            return;
        }
        break;
    }

    default:
    {
        assert(false);
        // add more clear operation by demand.
        break;
    }
    }
}

size_t xtcash_state_accessor::property_size(properties::xproperty_identifier_t const & property_id, std::error_code & ec) const {
    assert(!ec);
    assert(bstate_ != nullptr);

    switch (property_id.type()) {
    case properties::xproperty_type_t::bytes:
        XATTRIBUTE_FALLTHROUGH;
    case properties::xproperty_type_t::string:
    {
        auto string_property = bstate_->load_string_var(property_id.full_name());
        if (string_property == nullptr) {
            ec = error::xerrc_t::property_not_exist;
            return 0;
        }

        return string_property->query().size();
    }

    case properties::xproperty_type_t::map:
    {
        auto map_property = bstate_->load_string_map_var(property_id.full_name());
        if (map_property == nullptr) {
            ec = error::xerrc_t::property_not_exist;
            return 0;
        }

        return map_property->query().size();
    }

    default:
    {
        assert(false);
        // add more get size oprations by demand.
        return 0;
    }
    }
}

xbyte_buffer_t xtcash_state_accessor::bin_code(properties::xproperty_identifier_t const & property_id, std::error_code & ec) const {
    assert(!ec);
    assert(bstate_ != nullptr);

    if (!read_permitted(property_id)) {
        ec = error::xerrc_t::property_access_denied;
        return {};
    }

    auto const & property_name = property_id.full_name();
    auto code_property = bstate_->load_code_var(property_name);
    if (code_property == nullptr) {
        ec = error::xerrc_t::property_not_exist;
        return {};
    }

    auto const & bin_code = code_property->query();
    return { std::begin(bin_code), std::end(bin_code) };
}

void xtcash_state_accessor::deploy_bin_code(properties::xproperty_identifier_t const & property_id, xbyte_buffer_t const & bin_code, std::error_code & ec) {
    assert(!ec);
    assert(bstate_ != nullptr);

    if (!write_permitted(property_id)) {
        ec = error::xerrc_t::property_access_denied;
        return;
    }

    if (bstate_->find_property(property_id.full_name())) {
        ec = error::xerrc_t::property_already_exist;
        return;
    }

    auto code_property = bstate_->new_code_var(property_id.full_name(), canvas_.get());
    if (code_property == nullptr) {
        ec = error::xerrc_t::create_property_failed;
        return;
    }

    if (!code_property->deploy_code({ std::begin(bin_code), std::end(bin_code) }, canvas_.get())) {
        ec = error::xerrc_t::update_property_failed;
        return;
    }
}

bool xtcash_state_accessor::property_exist(properties::xproperty_identifier_t const & property_id, std::error_code & ec) const {
    assert(!ec);
    if (read_permitted(property_id)) {
        return bstate_->find_property(property_id.full_name());
    } else {
        ec = error::xerrc_t::property_access_denied;
        return false;
    }
}

common::xaccount_address_t xtcash_state_accessor::account_address() const {
    assert(bstate_ != nullptr);
    return common::xaccount_address_t{ bstate_->get_address() };
}

xobject_ptr_t<base::xvbstate_t> xtcash_state_accessor::state(common::xaccount_address_t const & address, std::error_code & ec) const {
    data::xunitstate_ptr_t unitstate = nullptr;// statestore::xstatestore_hub_t::instance()->get_unitstate(LatestConnectBlock, address); // TODO(jimmy)
    if (unitstate == nullptr) {
        xerror("[xtcash_state_accessor::get_property] get latest connectted state none, account=%s", address.to_string().c_str());
        ec = error::xenum_errc::load_account_state_failed;
        return nullptr;
    }
    xdbg("[xtcash_state_accessor::get_property] get latest connectted state success, account=%s, height=%ld", address.to_string().c_str(), unitstate->height());
    return unitstate->get_bstate();
}

uint64_t xtcash_state_accessor::height() const {
    assert(bstate_ != nullptr);
    return bstate_->get_block_height();
}

template <>
properties::xtype_of_t<properties::xproperty_type_t::int64>::type xtcash_state_accessor::get_property<properties::xproperty_type_t::int64>(
    properties::xtypeless_property_identifier_t const & property_id,
    std::error_code & ec) const {
    assert(!ec);
    assert(bstate_ != nullptr);

    auto const & property_name = property_id.full_name();
    if (!bstate_->find_property(property_name)) {
        if (!properties::system_property(property_id)) {
            ec = error::xerrc_t::property_not_exist;
            return {};
        } else {
            bstate_->new_int64_var(property_name, canvas_.get());
            bstate_->load_int64_var(property_name)->set(int64_t(0), canvas_.get());
        }
    }
    auto int_property = bstate_->load_int64_var(property_name);
    assert(int_property != nullptr);

    return int_property->get();
}

template <>
properties::xtype_of_t<properties::xproperty_type_t::uint64>::type xtcash_state_accessor::get_property<properties::xproperty_type_t::uint64>(
    properties::xtypeless_property_identifier_t const & property_id,
    std::error_code & ec) const {
    assert(!ec);
    assert(bstate_ != nullptr);

    auto const & property_name = property_id.full_name();
    if (!bstate_->find_property(property_name)) {
        if (!properties::system_property(property_id)) {
            ec = error::xerrc_t::property_not_exist;
            return {};
        } else {
            bstate_->new_uint64_var(property_name, canvas_.get());
            bstate_->load_uint64_var(property_name)->set(uint64_t(0), canvas_.get());
        }
    }
    auto int_property = bstate_->load_uint64_var(property_name);
    assert(int_property != nullptr);

    return int_property->get();
}

template <>
properties::xtype_of_t<properties::xproperty_type_t::bytes>::type xtcash_state_accessor::get_property<properties::xproperty_type_t::bytes>(
    properties::xtypeless_property_identifier_t const & property_id,
    std::error_code & ec) const {
    auto const & string = get_property<properties::xproperty_type_t::string>(property_id, ec);
    if (ec) {
        return {};
    }

    return {std::begin(string), std::end(string)};
}

template <>
properties::xtype_of_t<properties::xproperty_type_t::string>::type xtcash_state_accessor::get_property<properties::xproperty_type_t::string>(
    properties::xtypeless_property_identifier_t const & property_id,
    std::error_code & ec) const {
    assert(!ec);
    assert(bstate_ != nullptr);

    auto const & property_name = property_id.full_name();
    if (!bstate_->find_property(property_name)) {
        if (!properties::system_property(property_id)) {
            ec = error::xerrc_t::property_not_exist;
            return {};
        } else {
            bstate_->new_string_var(property_name, canvas_.get());
            bstate_->load_string_var(property_name)->reset(std::string{}, canvas_.get());
        }
    }
    auto string_property = bstate_->load_string_var(property_name);
    assert(string_property != nullptr);

    return string_property->query();
}

template <>
properties::xtype_of_t<properties::xproperty_type_t::map>::type xtcash_state_accessor::get_property<properties::xproperty_type_t::map>(
    properties::xtypeless_property_identifier_t const & property_id,
    std::error_code & ec) const {
    assert(!ec);
    assert(bstate_ != nullptr);

    auto const & property_name = property_id.full_name();
    if (!bstate_->find_property(property_name)) {
        if (!properties::system_property(property_id)) {
            ec = error::xerrc_t::property_not_exist;
            return {};
        } else {
            bstate_->new_string_map_var(property_name, canvas_.get());
        }
    }
    auto map_property = bstate_->load_string_map_var(property_name);
    assert(map_property != nullptr);

    auto map = map_property->query();
    properties::xtype_of_t<properties::xproperty_type_t::map>::type ret;
    for (auto & pair : map) {
        ret.insert({std::move(pair.first), {std::begin(pair.second), std::end(pair.second)}});
    }
    return ret;
}

template <>
properties::xtype_of_t<properties::xproperty_type_t::deque>::type xtcash_state_accessor::get_property<properties::xproperty_type_t::deque>(
    properties::xtypeless_property_identifier_t const & property_id,
    std::error_code & ec) const {
    assert(!ec);
    assert(bstate_ != nullptr);

    auto const & property_name = property_id.full_name();
    if (!bstate_->find_property(property_name)) {
        if (!properties::system_property(property_id)) {
            ec = error::xerrc_t::property_not_exist;
            return {};
        } else {
            bstate_->new_string_deque_var(property_name, canvas_.get());
        }
    }
    auto deque_property = bstate_->load_string_deque_var(property_name);
    assert(deque_property != nullptr);

    auto deque = deque_property->query();
    properties::xtype_of_t<properties::xproperty_type_t::deque>::type ret;
    ret.resize(deque.size());
    for (auto i = 0u; i < deque.size(); ++i) {
        ret[i] = {std::begin(deque[i]), std::end(deque[i])};
    }
    return ret;
}

template <>
properties::xtype_of_t<properties::xproperty_type_t::bytes>::type xtcash_state_accessor::get_property<properties::xproperty_type_t::bytes>(
    properties::xtypeless_property_identifier_t const & property_id,
    common::xaccount_address_t const & address,
    std::error_code & ec) const {
    auto const & string = get_property<properties::xproperty_type_t::string>(property_id, address, ec);
    if (ec) {
        return {};
    }

    return {std::begin(string), std::end(string)};
}

template <>
properties::xtype_of_t<properties::xproperty_type_t::string>::type xtcash_state_accessor::get_property<properties::xproperty_type_t::string>(
    properties::xtypeless_property_identifier_t const & property_id,
    common::xaccount_address_t const & address,
    std::error_code & ec) const {
    assert(!ec);
    //auto const & address_state = state(address, ec);
    //if (ec) {
    //    return {};
    //}

    //assert(address_state != nullptr);
    auto const & property_name = property_id.full_name();
    if (!bstate_->find_property(property_name)) {
        if (!properties::system_property(property_id)) {
            ec = error::xerrc_t::property_not_exist;
            return {};
        } else {
            bstate_->new_string_var(property_name, canvas_.get());
            bstate_->load_string_var(property_name)->reset(std::string{}, canvas_.get());
        }
    }
    auto const string_property = bstate_->load_string_var(property_name);
    assert(string_property != nullptr);

    return string_property->query();
}

template <>
properties::xtype_of_t<properties::xproperty_type_t::map>::type xtcash_state_accessor::get_property<properties::xproperty_type_t::map>(
    properties::xtypeless_property_identifier_t const & property_id,
    common::xaccount_address_t const & address,
    std::error_code & ec) const {
    assert(!ec);
    //auto const & address_state = state(address, ec);
    //if (ec) {
    //    return {};
    //}

    //assert(address_state != nullptr);
    auto const & property_name = property_id.full_name();
    if (!bstate_->find_property(property_name)) {
        if (!properties::system_property(property_id)) {
            ec = error::xerrc_t::property_not_exist;
            return {};
        } else {
            bstate_->new_string_map_var(property_name, canvas_.get());
        }
    }
    auto const map_property = bstate_->load_string_map_var(property_name);
    assert(map_property != nullptr);

    auto map = map_property->query();
    properties::xtype_of_t<properties::xproperty_type_t::map>::type ret;
    for (auto & pair : map) {
        ret.insert({pair.first, {std::begin(pair.second), std::end(pair.second)}});
    }
    return ret;
}

xbytes_t xtcash_state_accessor::get_property_serialized_value(properties::xtypeless_property_identifier_t const & property_id, std::error_code & ec) const {
    assert(!ec);
    assert(bstate_ != nullptr);
    assert(canvas_ != nullptr);

    auto const & property_name = property_id.full_name();

    if (!bstate_->find_property(property_id.full_name())) {
        ec = error::xerrc_t::property_not_exist;
        return {};
    }

    return bstate_->get_property_value_in_bytes(property_name);
}

template <>
void xtcash_state_accessor::set_property<properties::xproperty_type_t::int64>(properties::xtypeless_property_identifier_t const & property_id,
                                                                            properties::xtype_of_t<properties::xproperty_type_t::int64>::type const & value,
                                                                            std::error_code & ec) {
    assert(!ec);
    assert(bstate_ != nullptr);
    assert(canvas_ != nullptr);

    auto const & property_name = property_id.full_name();

    if (!bstate_->find_property(property_name)) {
        if (!properties::system_property(property_id)) {
            ec = error::xerrc_t::property_not_exist;
            return;
        } else {
            bstate_->new_int64_var(property_name, canvas_.get());
            bstate_->load_int64_var(property_name)->set(int64_t(0), canvas_.get());
        }
    }
    auto const int_property = bstate_->load_int64_var(property_name);
    assert(int_property != nullptr);

    if (!int_property->set(value, canvas_.get())) {
        ec = error::xerrc_t::update_property_failed;
        return;
    }
}

template <>
void xtcash_state_accessor::set_property<properties::xproperty_type_t::uint64>(properties::xtypeless_property_identifier_t const & property_id,
                                                                             properties::xtype_of_t<properties::xproperty_type_t::uint64>::type const & value,
                                                                             std::error_code & ec) {
    assert(!ec);
    assert(bstate_ != nullptr);
    assert(canvas_ != nullptr);

    auto const & property_name = property_id.full_name();
    if (!bstate_->find_property(property_name)) {
        if (!properties::system_property(property_id)) {
            ec = error::xerrc_t::property_not_exist;
            return;
        } else {
            bstate_->new_uint64_var(property_name, canvas_.get());
            bstate_->load_uint64_var(property_name)->set(uint64_t(0), canvas_.get());
        }
    }
    auto const int_property = bstate_->load_uint64_var(property_name);
    assert(int_property != nullptr);

    if (!int_property->set(value, canvas_.get())) {
        ec = error::xerrc_t::update_property_failed;
        return;
    }
}

template <>
void xtcash_state_accessor::set_property<properties::xproperty_type_t::bytes>(properties::xtypeless_property_identifier_t const & property_id,
                                                                            properties::xtype_of_t<properties::xproperty_type_t::bytes>::type const & value,
                                                                            std::error_code & ec) {
    set_property<properties::xproperty_type_t::string>(property_id, {std::begin(value), std::end(value)}, ec);
}

template <>
void xtcash_state_accessor::set_property<properties::xproperty_type_t::string>(properties::xtypeless_property_identifier_t const & property_id,
                                                                             properties::xtype_of_t<properties::xproperty_type_t::string>::type const & value,
                                                                             std::error_code & ec) {
    assert(!ec);
    assert(bstate_ != nullptr);
    assert(canvas_ != nullptr);

    auto const & property_name = property_id.full_name();
    if (!bstate_->find_property(property_name)) {
        if (!properties::system_property(property_id)) {
            ec = error::xerrc_t::property_not_exist;
            return;
        } else {
            bstate_->new_string_var(property_name, canvas_.get());
            bstate_->load_string_var(property_name)->reset(std::string{}, canvas_.get());
        }
    }
    auto const string_property = bstate_->load_string_var(property_name);
    assert(string_property != nullptr);

    if (!string_property->reset(value, canvas_.get())) {
        ec = error::xerrc_t::update_property_failed;
        return;
    }
}

void xtcash_state_accessor::set_property_by_serialized_value(properties::xtypeless_property_identifier_t const & property_id,
                                                           xbytes_t const & serialized_data,
                                                           std::error_code & ec) {
    assert(!ec);
    assert(bstate_ != nullptr);
    assert(canvas_ != nullptr);

    auto const & property_name = property_id.full_name();
    if (!bstate_->find_property(property_name)) {
        ec = error::xerrc_t::property_not_exist;
        return;
    }

    bstate_->set_property_value_from_bytes(property_name, serialized_data, canvas_.get());
}

template <>
void xtcash_state_accessor::set_property_cell_value<properties::xproperty_type_t::map>(properties::xtypeless_property_identifier_t const & property_id,
                                                                                     properties::xkey_type_of_t<properties::xproperty_type_t::map>::type const & key,
                                                                                     properties::xvalue_type_of_t<properties::xproperty_type_t::map>::type const & value,
                                                                                     std::error_code & ec) {
    assert(!ec);
    assert(bstate_ != nullptr);
    assert(canvas_ != nullptr);

    auto const & property_name = property_id.full_name();
    if (!bstate_->find_property(property_name)) {
        if (!properties::system_property(property_id)) {
            ec = error::xerrc_t::property_not_exist;
            return;
        }

        bstate_->new_string_map_var(property_name, canvas_.get());
    }
    auto const map_property = bstate_->load_string_map_var(property_name);
    xassert(map_property != nullptr);

    if (!map_property->insert(key, {std::begin(value), std::end(value)}, canvas_.get())) {
        ec = error::xerrc_t::update_property_failed;
        return;
    }
}

template <>
void xtcash_state_accessor::set_property_cell_value<properties::xproperty_type_t::deque>(properties::xtypeless_property_identifier_t const & property_id,
                                                                                       properties::xkey_type_of_t<properties::xproperty_type_t::deque>::type const & key,
                                                                                       properties::xvalue_type_of_t<properties::xproperty_type_t::deque>::type const & value,
                                                                                       std::error_code & ec) {
    assert(!ec);
    assert(bstate_ != nullptr);
    assert(canvas_ != nullptr);

    auto const & property_name = property_id.full_name();
    if (!bstate_->find_property(property_name)) {
        if (!properties::system_property(property_id)) {
            ec = error::xerrc_t::property_not_exist;
            return;
        } else {
            bstate_->new_string_deque_var(property_name, canvas_.get());
        }
    }
    auto deque_property = bstate_->load_string_deque_var(property_name);
    xassert(deque_property != nullptr);

    if (!deque_property->update(key, {std::begin(value), std::end(value)}, canvas_.get())) {
        ec = error::xerrc_t::update_property_failed;
        return;
    }
}

template <>
properties::xvalue_type_of_t<properties::xproperty_type_t::map>::type xtcash_state_accessor::get_property_cell_value<properties::xproperty_type_t::map>(
    properties::xtypeless_property_identifier_t const & property_id,
    properties::xkey_type_of_t<properties::xproperty_type_t::map>::type const & key,
    std::error_code & ec) const {
    assert(!ec);
    assert(bstate_ != nullptr);
    assert(canvas_ != nullptr);

    auto const & property_name = property_id.full_name();
    if (!bstate_->find_property(property_name)) {
        if (!properties::system_property(property_id)) {
            ec = error::xerrc_t::property_not_exist;
            return {};
        } else {
            bstate_->new_string_map_var(property_name, canvas_.get());
        }
    }
    auto map_property = bstate_->load_string_map_var(property_name);
    assert(map_property != nullptr);

    auto string = bstate_->load_string_map_var(property_name)->query(key);
    if (string.empty()) {
        return {};
    }

    return {std::begin(string), std::end(string)};
}

template <>
properties::xvalue_type_of_t<properties::xproperty_type_t::deque>::type xtcash_state_accessor::get_property_cell_value<properties::xproperty_type_t::deque>(
    properties::xtypeless_property_identifier_t const & property_id,
    properties::xkey_type_of_t<properties::xproperty_type_t::deque>::type const & key,
    std::error_code & ec) const {
    assert(!ec);
    assert(bstate_ != nullptr);
    assert(canvas_ != nullptr);

    auto const & property_name = property_id.full_name();
    if (!bstate_->find_property(property_name)) {
        if (!properties::system_property(property_id)) {
            ec = error::xerrc_t::property_not_exist;
            return {};
        } else {
            bstate_->new_string_deque_var(property_name, canvas_.get());
        }
    }
    auto deque_property = bstate_->load_string_deque_var(property_name);
    xassert(deque_property != nullptr);

    if (key >= deque_property->query().size()) {
        ec = error::xerrc_t::property_key_not_exist;
        return {};
    }

    auto string = deque_property->query(key);
    return {std::begin(string), std::end(string)};
}

template <>
bool xtcash_state_accessor::exist_property_cell_key<properties::xproperty_type_t::map>(properties::xtypeless_property_identifier_t const & property_id,
                                                                                     properties::xkey_type_of_t<properties::xproperty_type_t::map>::type const & key,
                                                                                     std::error_code & ec) const {
    assert(!ec);
    assert(bstate_ != nullptr);
    assert(canvas_ != nullptr);

    assert(!properties::system_property(property_id));

    auto const & property_name = property_id.full_name();
    auto map_property = bstate_->load_string_map_var(property_name);
    if (map_property == nullptr) {
        ec = error::xerrc_t::property_not_exist;
        return false;
    }

    assert(map_property != nullptr);
    if (!map_property->find(key)) {
        return false;
    }

    return true;
}

template <>
bool xtcash_state_accessor::exist_property_cell_key<properties::xproperty_type_t::deque>(properties::xtypeless_property_identifier_t const & property_id,
                                                                                       properties::xkey_type_of_t<properties::xproperty_type_t::deque>::type const & key,
                                                                                       std::error_code & ec) const {
    assert(!ec);
    assert(bstate_ != nullptr);
    assert(canvas_ != nullptr);

    assert(!properties::system_property(property_id));

    auto const & property_name = property_id.full_name();
    auto deque_property = bstate_->load_string_deque_var(property_name);
    if (deque_property == nullptr) {
        ec = error::xerrc_t::property_not_exist;
        return false;
    }

    assert(deque_property != nullptr);
    if (key >= deque_property->query().size()) {
        return false;
    }

    return true;
}

template <>
void xtcash_state_accessor::remove_property_cell<properties::xproperty_type_t::map>(properties::xtypeless_property_identifier_t const & property_id, typename properties::xkey_type_of_t<properties::xproperty_type_t::map>::type const & key, std::error_code & ec) {
    assert(!ec);
    assert(bstate_ != nullptr);

    if (!write_permitted({ property_id, properties::xproperty_type_t::map })) {
        ec = error::xerrc_t::property_access_denied;
        return;
    }

    auto map_property = bstate_->load_string_map_var(property_id.full_name());
    if (map_property == nullptr) {
        ec = error::xerrc_t::property_not_exist;
        return;
    }

    if (!map_property->find(key)) {
        ec = error::xerrc_t::property_key_not_exist;
        return;
    }

    if (!map_property->erase(key, canvas_.get())) {
        ec = error::xerrc_t::update_property_failed;
        return;
    }
}

template <>
void xtcash_state_accessor::remove_property_cell<properties::xproperty_type_t::deque>(properties::xtypeless_property_identifier_t const & property_id, typename properties::xkey_type_of_t<properties::xproperty_type_t::deque>::type const & key, std::error_code & ec) {
    assert(!ec);
    assert(bstate_ != nullptr);

    if (!write_permitted({ property_id, properties::xproperty_type_t::map })) {
        ec = error::xerrc_t::property_access_denied;
        return;
    }

    auto deque_property = bstate_->load_string_deque_var(property_id.full_name());
    if (deque_property == nullptr) {
        ec = error::xerrc_t::property_not_exist;
        return;
    }

    auto const size = deque_property->query().size();
    if (size == 0) {
        ec = error::xerrc_t::property_key_out_of_range;
        return;
    }

    if (key == 0) {
        if (!deque_property->pop_front(canvas_.get())) {
            ec = error::xerrc_t::update_property_failed;
            return;
        }
    } else if (key >= size - 1) {
        if (!deque_property->pop_back(canvas_.get())) {
            ec = error::xerrc_t::update_property_failed;
            return;
        }
    } else {
        ec = error::xerrc_t::property_key_out_of_range;
        return;
    }
}

std::string xtcash_state_accessor::binlog(std::error_code & ec) const {
    std::string r;
    assert(canvas_ != nullptr);
    if (canvas_->encode(r) < 0) {
        ec = error::xerrc_t::get_binlog_failed;
    }

    return r;
}

size_t xtcash_state_accessor::binlog_size(std::error_code & ec) const {
    assert(!ec);
    assert(canvas_ != nullptr);

    // size_t r;
    return canvas_.get()->get_op_records_size();
}

std::string xtcash_state_accessor::fullstate_bin(std::error_code & ec) const {
    assert(!ec);
    assert(bstate_ != nullptr);
    std::string fullstate_bin;
    bstate_->take_snapshot(fullstate_bin);

    return fullstate_bin;
}

void xtcash_state_accessor::do_create_string_property(std::string const & property_name, std::error_code & ec) {
    assert(!ec);
    assert(property_name_min_length <= property_name.length() && property_name.length() < property_name_max_length);

    auto const string_property = bstate_->new_string_var(property_name, canvas_.get());
    if (string_property == nullptr) {
        ec = error::xerrc_t::create_property_failed;
        return;
    }
}

void xtcash_state_accessor::do_create_map_property(std::string const & property_name, std::error_code & ec) {
    assert(!ec);
    assert(property_name_min_length <= property_name.length() && property_name.length() < property_name_max_length);

    auto const map_property = bstate_->new_string_map_var(property_name, canvas_.get());
    if (map_property == nullptr) {
        ec = error::xerrc_t::create_property_failed;
        return;
    }
}

#define CREATE_INT_PROPERTY(INT_TYPE)                                                                                                                   \
    template <>                                                                                                                                         \
    void xtcash_state_accessor::do_create_int_property<properties::xproperty_type_t::INT_TYPE>(std::string const & property_name, std::error_code & ec) { \
        assert(!ec);                                                                                                                                    \
        assert(property_name_min_length <= property_name.length() && property_name.length() < property_name_max_length);                                \
        auto const int_property = bstate_->new_##INT_TYPE##_var(property_name, canvas_.get());                                                          \
        if (int_property == nullptr) {                                                                                                                  \
            ec = error::xerrc_t::create_property_failed;                                                                                                \
            return;                                                                                                                                     \
        }                                                                                                                                               \
    }

CREATE_INT_PROPERTY(int64)
CREATE_INT_PROPERTY(uint64)

void xtcash_state_accessor::do_create_token_property(std::string const & property_name, std::error_code & ec) {
    assert(!ec);
    assert(property_name_min_length <= property_name.length() && property_name.length() < property_name_max_length);

    assert(false);

    auto const token_property = bstate_->new_token_var(property_name, canvas_.get());
    if (token_property == nullptr) {
        ec = error::xerrc_t::create_property_failed;
        return;
    }
}

#undef CREATE_INT_PROPERTY

NS_END2
