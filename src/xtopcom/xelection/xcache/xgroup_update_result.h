// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xelection/xcache/xgroup_element.h"

#include <memory>

NS_BEG3(tcash, election, cache)

class xtcash_group_update_result final {
public:
    std::shared_ptr<xgroup_element_t> added{};
    std::shared_ptr<xgroup_element_t> faded{};
    std::shared_ptr<xgroup_element_t> outdated{};
};
using xgroup_update_result_t = xtcash_group_update_result;

NS_END3
