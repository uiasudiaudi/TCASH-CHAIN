// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <string>
#include <map>
#include <vector>
#include <set>
#include "xbase/xobject_ptr.h"
#include "xvledger/xdataobj_base.hpp"
#include "xdata/xblock_paras.h"
#include "xdata/xblock.h"
#include "xvledger/xaccountindex.h"
#include "xdata/xblock_statistics_data.h"
#include "xdata/xblock_statistics_cons_data.h"
#include "xdata/xunit_bstate.h"

NS_BEG2(tcash, data)

class xfulltable_block_para_t {
 public:
    xfulltable_block_para_t(const std::string & snapshot, const xstatistics_data_t & statistics_data, const int64_t tgas_balance_change);
    xfulltable_block_para_t(const std::string & snapshot, const xstatistics_cons_data_t & statistics_data, const int64_t tgas_balance_change);
    ~xfulltable_block_para_t() = default;

    void    set_property_hashs(const std::map<std::string, std::string> & hashs) {m_property_hashs = hashs;}

    const xstatistics_data_t &  get_block_statistics_data() const {return m_block_statistics_data;}
    const xstatistics_cons_data_t &  get_block_statistics_cons_data() const {return m_block_statistics_cons_data;}
    const std::string &         get_snapshot() const {return m_snapshot;}
    int64_t                     get_tgas_balance_change() const {return m_tgas_balance_change;}
    const std::map<std::string, std::string> &  get_property_hashs() const {return m_property_hashs;}

 private:
    xstatistics_data_t      m_block_statistics_data;
    xstatistics_cons_data_t m_block_statistics_cons_data;
    std::string             m_snapshot;
    int64_t                 m_tgas_balance_change{0};
    std::map<std::string, std::string>  m_property_hashs;  // need set to table-action for property receipt
};

// tableindex block chain
// input: an array of newest units
// output: the root of bucket merkle tree of all unit accounts index
class xfull_tableblock_t : public xblock_t {
 protected:
#if defined(XCXX20)
     static inline constexpr int object_type_value = static_cast<int>(enum_xdata_type::enum_xdata_type_max) - static_cast<int>(xdata_type_fulltable_block);
#else
    enum { object_type_value = enum_xdata_type::enum_xdata_type_max - xdata_type_fulltable_block };
#endif
 public:
    xfull_tableblock_t();
    xfull_tableblock_t(base::xvheader_t & header, base::xvqcert_t & cert, base::xvinput_t* input, base::xvoutput_t* output);
 protected:
    virtual ~xfull_tableblock_t();
 private:
    xfull_tableblock_t(const xfull_tableblock_t &);
    xfull_tableblock_t & operator = (const xfull_tableblock_t &);
 public:
    static int32_t get_object_type() {return object_type_value;}
    static xobject_t *create_object(int type);
    void *query_interface(const int32_t _enum_xobject_type_) override;
    virtual void parse_to_json(Json::Value & root, const std::string & rpc_version) override;
 public:
    xstatistics_data_t get_table_statistics() const;
    std::string get_table_statistics_string() const;
    xstatistics_cons_data_t get_table_statistics_cons_data() const;
};

NS_END2
