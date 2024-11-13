// Copyright (c) 2017-2022 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xgasfee/xgas_estimate.h"

#include "xconfig/xconfig_register.h"
#include "xconfig/xpredefined_configurations.h"
#include "xbase/xlog.h"
#include <stdint.h>

namespace tcash {
namespace gasfee {

#define tcash_eth_nominal_min (200)
#define tcash_eth_nominal_max (1000)

evm_common::u256 xgas_estimate::base_price() {
    return XGET_ONCHAIN_GOVERNANCE_PARAMETER(tcash_eth_base_price);
}

evm_common::u256 xgas_estimate::flexible_price(const xobject_ptr_t<data::xcons_transaction_t> tx, const evm_common::u256 evm_gas) {
    evm_common::u256 total_utcash = tx->get_current_used_deposit();
    evm_common::u256 total_wei = xtcash_gas_tx_operator::utcash_to_wei(total_utcash);
    evm_common::u256 flexible_price = total_wei / evm_gas;
    return flexible_price;
}

evm_common::u256 xgas_estimate::get_nominal_balance(const evm_common::u256& tcash_balance, const evm_common::u256& eth_balance)
{
    evm_common::u256 nominal_balance { 0 };
    /*if eth balance = 0 and tcash balance < 200; nominal_balance = 0
    if eth balance = 0 and 1000 > tcash balance >= 200; nominal_balance = tcash balance/tcash_to_eth_exchange_ratio
    if eth balance = 0 and tcash balance >= 1000; nominal_balance = 1000/tcash_to_eth_exchange_ratio
    if eth balance > 0 ,return eth balance */

    if (eth_balance == 0) {
        if (tcash_balance < ASSET_tcash(tcash_eth_nominal_min)) {
            nominal_balance = 0;
        } else if (tcash_balance < ASSET_tcash(tcash_eth_nominal_max)) {
            nominal_balance = xtcash_gas_tx_operator::utcash_to_wei(tcash_balance);
        } else {
            nominal_balance = xtcash_gas_tx_operator::utcash_to_wei(ASSET_tcash(tcash_eth_nominal_max));
        }
    } else {
        nominal_balance = eth_balance;
    }
    xdbg("xgas_estimate::get_nominal_balance tcash_balance %s, eth_balance %s, nominal_balance %s",
        tcash_balance.str().c_str(),
        eth_balance.str().c_str(),
        nominal_balance.str().c_str());
    return nominal_balance;
}

}  // namespace gasfee
}  // namespace tcash