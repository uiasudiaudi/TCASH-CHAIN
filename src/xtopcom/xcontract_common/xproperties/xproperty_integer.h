// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcontract_common/xproperties/xbasic_property.h"
#include "xcontract_common/xcontract_face.h"
#include "xcontract_common/xcontract_state.h"

#include <string>

NS_BEG3(tcash, contract_common, properties)

#define DECLARE_INT_PROPERTY(INT_TYPE)                                                                                                                                             \
    class xtcash_##INT_TYPE##_property : public xbasic_property_t {                                                                                                                  \
    public:                                                                                                                                                                        \
        xtcash_##INT_TYPE##_property() = default;                                                                                                                                    \
        xtcash_##INT_TYPE##_property(xtcash_##INT_TYPE##_property const &) = delete;                                                                                                   \
        xtcash_##INT_TYPE##_property & operator=(xtcash_##INT_TYPE##_property const &) = delete;                                                                                       \
        xtcash_##INT_TYPE##_property(xtcash_##INT_TYPE##_property &&) = delete;                                                                                                        \
        xtcash_##INT_TYPE##_property & operator=(xtcash_##INT_TYPE##_property &&) = delete;                                                                                            \
        ~xtcash_##INT_TYPE##_property() override = default;                                                                                                                          \
                                                                                                                                                                                   \
        explicit xtcash_##INT_TYPE##_property(std::string const & name, xcontract_face_t *);                                                                                         \
                                                                                                                                                                                   \
        void set(INT_TYPE##_t value);                                                                                                                                              \
        void clear();                                                                                                                                                              \
        INT_TYPE##_t value() const;                                                                                                                                                \
    };                                                                                                                                                                             \
    using x##INT_TYPE##_property_t = xtcash_##INT_TYPE##_property

DECLARE_INT_PROPERTY(uint64);
DECLARE_INT_PROPERTY(int64);

#undef DECLARE_INT_PROPERTY

NS_END3
