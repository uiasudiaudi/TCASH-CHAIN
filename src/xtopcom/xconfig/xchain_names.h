// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xvledger/xvaccount.h"

#include <string>

NS_BEG2(tcash, config)

XINLINE_CONSTEXPR char const * chain_name_mainnet{"new_horizons"};
XINLINE_CONSTEXPR char const * chain_name_testnet{"galileo"};
XINLINE_CONSTEXPR char const * chain_name_consortium{"consortium"};

base::enum_xchain_id to_chainid(std::string const & chain_name);

NS_END2
