// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"

#include <functional>

NS_BEG1(tcash)

class xtcash_scope_executer final
{
    std::function<void()> m_execution_definition;

public:
    xtcash_scope_executer(xtcash_scope_executer const &) = delete;
    xtcash_scope_executer & operator=(xtcash_scope_executer const &) = delete;
    xtcash_scope_executer(xtcash_scope_executer &&) = default;
    xtcash_scope_executer& operator=(xtcash_scope_executer &&) = default;
    ~xtcash_scope_executer() noexcept;

    explicit
    xtcash_scope_executer(std::function<void()> && execution_body) noexcept;
};
using xscope_executer_t = xtcash_scope_executer;

NS_END1
