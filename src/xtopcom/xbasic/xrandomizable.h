// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

template <typename T>
struct xtcash_randomizable_face
{
    xtcash_randomizable_face()                                           = default;
    xtcash_randomizable_face(xtcash_randomizable_face const &)             = default;
    xtcash_randomizable_face & operator=(xtcash_randomizable_face const &) = default;
    xtcash_randomizable_face(xtcash_randomizable_face &&)                  = default;
    xtcash_randomizable_face & operator=(xtcash_randomizable_face &&)      = default;
    virtual ~xtcash_randomizable_face()                                  = default;

    virtual
    void
    random() = 0;
};

template <typename T>
using xrandomizable_t = xtcash_randomizable_face<T>;
