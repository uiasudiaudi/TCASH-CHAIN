// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"

#include <type_traits>
#include <utility>

NS_BEG1(tcash)

template <typename T>
class xtcash_range final {
public:
    T begin{};
    T end{};

    constexpr xtcash_range()                     = default;
    xtcash_range(xtcash_range const &)             = default;
    xtcash_range & operator=(xtcash_range const &) = default;
    xtcash_range(xtcash_range &&)                  = default;
    xtcash_range & operator=(xtcash_range &&)      = default;
    ~xtcash_range()                              = default;

    constexpr
    xtcash_range(T const & b, T const & e) noexcept(std::is_nothrow_copy_constructible<T>::value)
        : begin{ b }, end{ e } {
    }

    constexpr
    xtcash_range(T && b, T && e) noexcept(std::is_nothrow_move_constructible<T>::value)
        : begin{ std::move(b) }, end{ std::move(e) } {
    }

    constexpr
    bool
    empty() const noexcept {
        return begin == end;
    }

    bool
    operator==(xtcash_range const & other) const noexcept {
        return begin == other.begin && end == other.end;
    }

    bool
    operator!=(xtcash_range const & other) const noexcept {
        return !(*this == other);
    }

    void
    clear() noexcept {
        begin = T{};
        end = T{};
    }
};

template <typename T>
using xrange_t = xtcash_range<T>;

NS_END1
