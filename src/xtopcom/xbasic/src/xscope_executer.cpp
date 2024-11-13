// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xbasic/xscope_executer.h"

NS_BEG1(tcash)

xtcash_scope_executer::xtcash_scope_executer(std::function<void()> && execution_body) noexcept
    : m_execution_definition{ std::move(execution_body) }
{
}

xtcash_scope_executer::~xtcash_scope_executer() noexcept {
    m_execution_definition();
}

NS_END1
