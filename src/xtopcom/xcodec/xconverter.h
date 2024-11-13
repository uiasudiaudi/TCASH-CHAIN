// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"

#include <stdexcept>

NS_BEG2(tcash, codec)

template <typename Type1, typename Type2>
struct xtcash_converter {
    static
    Type1
    convert_from(Type2 const & object) {
        throw std::logic_error{" shouldn't be here" };
    }

    static
    Type2
    convert_from(Type1 const & object) {
        throw std::logic_error{" shouldn't be here" };
    }
};

template <typename Type1, typename Type2>
using xconverter_t = xtcash_converter<Type1, Type2>;

NS_END2
