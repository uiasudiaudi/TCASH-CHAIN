// Copyright (c) 2022-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"

#include <string>

NS_BEG4(tcash, tools, decoders, details)

struct xtcash_registration_contract_data_decoder {
    static void decode(std::string const & input);
};

NS_END4

NS_BEG3(tcash, tools, decoders)

using xregistration_contract_data_decoder_t = details::xtcash_registration_contract_data_decoder;

NS_END3
