// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xstate_accessor/xstate_face.h"

#include <cassert>

namespace tcash {
namespace state_accessor {

xtcash_state_face::xtcash_state_face(std::unique_ptr<xstate_accessor_t> state_accessor, xstate_type_t const type) noexcept
    : m_state_accessor{ std::move(state_accessor) }, m_state_type{ type } {
    assert(type != xstate_type_t::invalid);
}

xstate_type_t xtcash_state_face::type() const noexcept {
    return m_state_type;
}

}
}
