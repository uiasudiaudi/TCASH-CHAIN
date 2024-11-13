// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"

NS_BEG2(tcash, election)

class xtcash_data_accessor_face {
public:
    xtcash_data_accessor_face()                                            = default;
    xtcash_data_accessor_face(xtcash_data_accessor_face const &)             = delete;
    xtcash_data_accessor_face & operator=(xtcash_data_accessor_face const &) = delete;
    xtcash_data_accessor_face(xtcash_data_accessor_face &&)                  = default;
    xtcash_data_accessor_face & operator=(xtcash_data_accessor_face &&)      = default;
    virtual ~xtcash_data_accessor_face()                                   = default;


};
using xdata_accessor_face_t = xtcash_data_accessor_face;

NS_END2
