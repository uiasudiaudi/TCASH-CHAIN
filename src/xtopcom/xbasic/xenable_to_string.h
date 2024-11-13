// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"

#include <string>
#include <system_error>

NS_BEG1(tcash)

template <typename T>
class xtcash_enable_to_string {
public:
    xtcash_enable_to_string()                                          = default;
    xtcash_enable_to_string(xtcash_enable_to_string const &)             = default;
    xtcash_enable_to_string & operator=(xtcash_enable_to_string const &) = default;
    xtcash_enable_to_string(xtcash_enable_to_string &&)                  = default;
    xtcash_enable_to_string & operator=(xtcash_enable_to_string &&)      = default;
    virtual ~xtcash_enable_to_string()                                 = default;

    virtual std::string to_string() const = 0;
    virtual void from_string(std::string const & s, std::error_code & ec) = 0;
};
template <typename T>
using xenable_to_string_t = xtcash_enable_to_string<T>;

NS_END1
