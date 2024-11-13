// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <string>
#include <map>
#include "xvledger/xdataobj_base.hpp"
#include "xvledger/xvblock.h"
#include "xdata/xdata_common.h"
#include "xdata/xdatautil.h"
#include "xdata/xblock.h"

NS_BEG2(tcash, data)

class xemptyblock_t : public xblock_t {
 protected:
#if defined(XCXX20)
     static inline constexpr int object_type_value = static_cast<int>(enum_xdata_type::enum_xdata_type_max) - static_cast<int>(xdata_type_empty_block);
#else
    enum { object_type_value = enum_xdata_type::enum_xdata_type_max - xdata_type_empty_block };
#endif

public:
    xemptyblock_t();
    xemptyblock_t(base::xvheader_t & header, base::xvqcert_t & cert);
    virtual ~xemptyblock_t();
 private:
    xemptyblock_t(const xemptyblock_t &);
    xemptyblock_t & operator = (const xemptyblock_t &);
 public:
    static int32_t get_object_type() {return object_type_value;}
    static xobject_t *create_object(int type);
    void *query_interface(const int32_t _enum_xobject_type_) override;
    virtual void parse_to_json(Json::Value & root, const std::string & rpc_version) override;
};


NS_END2
