// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xobject_ptr.h"
#include "xbase/xobject.h"
#include "xcommon/xlogic_time.h"
#include "xdata/xtcash_action_fwd.h"
#include "xdata/xtcash_action_type.h"

#include <cstdint>

NS_BEG2(tcash, data)

class xtcash_basic_tcash_action {
protected:
    xobject_ptr_t<base::xobject_t> m_action_src{ nullptr };
    common::xlogic_time_t m_expiration_time{ 0 };

public:
    xtcash_basic_tcash_action() = default;
    xtcash_basic_tcash_action(xtcash_basic_tcash_action const &) = default;
    xtcash_basic_tcash_action & operator=(xtcash_basic_tcash_action const &) = default;
    xtcash_basic_tcash_action(xtcash_basic_tcash_action &&) = default;
    xtcash_basic_tcash_action & operator=(xtcash_basic_tcash_action &&) = default;
    virtual ~xtcash_basic_tcash_action() = default;

protected:
    xtcash_basic_tcash_action(xobject_ptr_t<base::xobject_t> action_src, common::xlogic_time_t expiration_time) noexcept;

public:
    virtual xtcash_action_type_t type() const noexcept;
};
using xbasic_tcash_action_t = xtcash_basic_tcash_action;

template <xtcash_action_type_t ActionTypeV>
struct xtcash_tcash_action : public xbasic_tcash_action_t {
public:
    xtcash_tcash_action(xtcash_tcash_action const &) = default;
    xtcash_tcash_action & operator=(xtcash_tcash_action const &) = default;
    xtcash_tcash_action(xtcash_tcash_action &&) = default;
    xtcash_tcash_action & operator=(xtcash_tcash_action &&) = default;
    ~xtcash_tcash_action() override = default;

    xtcash_tcash_action(xobject_ptr_t<base::xobject_t> action_src, common::xlogic_time_t expiration_time) noexcept : xbasic_tcash_action_t{ std::move(action_src), expiration_time } {
    }

    xtcash_action_type_t type() const noexcept override final {
        return ActionTypeV;
    }
};

template <xtcash_action_type_t ActionTypeV>
using xtcash_action_t = xtcash_tcash_action<ActionTypeV>;

NS_END2
