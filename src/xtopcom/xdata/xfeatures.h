// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <stdint.h>
#include "xbase/xns_macro.h"

NS_BEG2(tcash, data)

struct xfeatures_t {
    uint64_t feature_bits;
};

NS_END2