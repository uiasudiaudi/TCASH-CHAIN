// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xstate_accessor/xstate_accessor.h"

namespace tcash {
namespace state_accessor {

enum class xenum_state_type {
    invalid,
    unit,
    table
};
using xstate_type_t = xenum_state_type;

class xtcash_state_face {
protected:
    std::unique_ptr<xstate_accessor_t> m_state_accessor{ nullptr };
    xstate_type_t m_state_type{ xstate_type_t::invalid };
public:
    xtcash_state_face() = default;
    xtcash_state_face(xtcash_state_face const &) = delete;
    xtcash_state_face & operator=(xtcash_state_face const &) = delete;
    xtcash_state_face(xtcash_state_face &&) = default;
    xtcash_state_face & operator=(xtcash_state_face &&) = default;
    virtual ~xtcash_state_face() = default;

protected:
    xtcash_state_face(std::unique_ptr<xstate_accessor_t> state_accessor, xstate_type_t const type) noexcept;

public:
    xstate_type_t type() const noexcept;

    virtual uint64_t balance(std::string const & symbol) const noexcept = 0;
    virtual uint64_t balance() const noexcept = 0;
};
using xstate_face_t = xtcash_state_face;

}
}
