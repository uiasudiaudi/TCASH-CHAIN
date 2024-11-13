// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xconfig/xconfig_register.h"
#include "xconfig/xchain_names.h"

NS_BEG2(tcash, config)

base::enum_xchain_id to_chainid(std::string const & chain_name) {
    assert(chain_name == chain_name_mainnet || chain_name == chain_name_testnet || chain_name == chain_name_consortium);
    if (chain_name == chain_name_mainnet) {
        return base::enum_main_chain_id;
    } else if (chain_name == chain_name_testnet) {
        return base::enum_test_chain_id;
    }  else if (chain_name == chain_name_consortium) {
        return base::enum_consortium_id; 
    }
    else {
        return base::enum_service_chain_id_start_reserved;
    }
}

NS_END2
