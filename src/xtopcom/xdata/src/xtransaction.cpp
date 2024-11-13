// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xdata/xtransaction.h"

#include "xbase/xutl.h"
#include "xbasic/xhex.h"
#include "xcommon/xerror/xerror.h"
#include "xconfig/xconfig_register.h"
#include "xconfig/xpredefined_configurations.h"

#include <nlohmann/json.hpp>

#include <cinttypes>
namespace tcash { namespace data {

bool xtransaction_t::transaction_type_check() const {
    switch (get_tx_type()) {
#ifdef ENABLE_CREATE_USER  // debug use
        case xtransaction_type_create_user_account:
#endif
            case xtransaction_type_deploy_evm_contract:
            case xtransaction_type_run_contract:
            case xtransaction_type_transfer:
                return true;
#if !defined(XBUILD_CONSORTIUM)
            case xtransaction_type_vote:
            case xtransaction_type_abolish_vote:
            case xtransaction_type_pledge_token_vote:
            case xtransaction_type_redeem_token_vote:
                return true;
#endif
            case xtransaction_type_pledge_token_tgas:
            case xtransaction_type_redeem_token_tgas: {
                if (XGET_CONFIG(enable_free_tgas)) {
                    return true;
                } else {
                    return false;
                }
            } break;
            default:
                return false;
    }
}

std::string xtransaction_t::transaction_type_to_string(uint16_t type) {
    switch (type) {
        case xtransaction_type_deploy_evm_contract: return "evm_contract";
        case xtransaction_type_create_user_account: return "create_user";
        case xtransaction_type_run_contract:        return "run_contract";
        case xtransaction_type_transfer:            return "transfer";
        case xtransaction_type_vote:                return "vote";
        case xtransaction_type_abolish_vote:        return "abolist_vote";
        case xtransaction_type_pledge_token_tgas:   return "pldge_tgas";
        case xtransaction_type_redeem_token_tgas:   return "redeem_tgas";
        case xtransaction_type_pledge_token_vote:   return "pledge_vote";
        case xtransaction_type_redeem_token_vote:   return "redeem_vote";
        default:
            xassert(false);
            return "invalid";
    }
}

bool xtransaction_t::set_tx_by_serialized_data(xtransaction_ptr_t & tx_ptr, const std::string & data) {
    try {
        base::xdataunit_t * raw_tx = base::xdataunit_t::read_from(data);
        if (nullptr == raw_tx) {
            xwarn("xtransaction_t::set_tx_by_serialized_data fail-tx content read from fail.");
            return false;
        }

        auto tx = dynamic_cast<xtransaction_t *>(raw_tx);
        tx_ptr.attach(tx);
        return true;
    } catch (tcash::error::xtcash_error_t const & eh) {
        xwarn("set_tx_by_serialized_data: invalid xtransaction_t serialized data. category: %s; ec: %" PRIi32 "; msg:%s", eh.code().category().name(), eh.code().value(), eh.what());
        return false;
    } catch (std::exception const & eh) {
        xwarn("set_tx_by_serialized_data: invalid xtransaction_t serialized data. unknown error. msg %s", eh.what());
        return false;
    }

    assert(false);
    xerror("set_tx_by_serialized_data: fatal error: should not reach");
    return false;
}

uint64_t xtransaction_t::get_gmttime_s() {
    struct timeval val;
    base::xtime_utl::gettimeofday(&val);
    return static_cast<uint64_t>(val.tv_sec);
}

void xtransaction_t::set_action_type_by_tx_type(const enum_xtransaction_type tx_type) {
    switch (tx_type)
    {
    case xtransaction_type_create_user_account:
        set_source_action_type(xaction_type_source_null);
        set_target_action_type(xaction_type_create_user_account);
        break;

    case xtransaction_type_run_contract:
        set_source_action_type(xaction_type_asset_out);
        set_target_action_type(xaction_type_run_contract);
        break;

    case xtransaction_type_transfer:
        set_source_action_type(xaction_type_asset_out);
        set_target_action_type(xaction_type_asset_in);
        break;

    case xtransaction_type_vote:
        set_source_action_type(xaction_type_source_null);
        set_target_action_type(xaction_type_run_contract);
        break;

    case xtransaction_type_abolish_vote:
        set_source_action_type(xaction_type_source_null);
        set_target_action_type(xaction_type_run_contract);
        break;

    case xtransaction_type_pledge_token_tgas:
        set_source_action_type(xaction_type_source_null);
        set_target_action_type(xaction_type_pledge_token);
        break;

    case xtransaction_type_redeem_token_tgas:
        set_source_action_type(xaction_type_source_null);
        set_target_action_type(xaction_type_redeem_token);
        break;

    case xtransaction_type_pledge_token_vote:
        set_source_action_type(xaction_type_source_null);
        set_target_action_type(xaction_type_pledge_token_vote);
        break;

    case xtransaction_type_redeem_token_vote:
        set_source_action_type(xaction_type_source_null);
        set_target_action_type(xaction_type_redeem_token_vote);
        break;

    default:
        break;
    }
}

std::string xtransaction_t::tx_exec_status_to_str(uint8_t exec_status) {
    if (exec_status == enum_xunit_tx_exec_status_success) {
        return "success";
    } else {
        return "failure";
    }
}

xeth_transaction_t xtransaction_t::to_eth_tx(std::error_code & ec) const {
    ec = common::error::xerrc_t::invalid_eth_tx;
    xerror("xtransaction_t::to_eth_tx fail-invalid");
    return {};
}

}  // namespace data
}  // namespace tcash
