// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"

#include <functional>

NS_BEG1(tcash)

template <typename RetT, typename ... ArgsT>
class xtcash_action final
{
private:
    std::function<RetT(ArgsT...)> m_actor;

public:
    xtcash_action(xtcash_action const &)             = default;
    xtcash_action & operator=(xtcash_action const &) = default;
    xtcash_action(xtcash_action &&)                  = default;
    xtcash_action & operator=(xtcash_action &&)      = default;
    ~xtcash_action()                               = default;

    xtcash_action(std::function<RetT(ArgsT...)> && actor)
        : m_actor{ std::move(actor) }
    {
    }

    xtcash_action(std::function<RetT(ArgsT...)> const & actor)
        : m_actor{ actor }
    {
    }

    RetT
    operator()(ArgsT ... args) const {
        return m_actor(std::forward<ArgsT>(args)...);
    }
};

template <typename RetT, typename ... ArgsT>
using xaction_t = xtcash_action<RetT, ArgsT...>;

NS_END1
