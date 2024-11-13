﻿// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <string>
#include <vector>
#include "xblockmaker/xblockmaker_face.h"
#include "xvledger/xaccountindex.h"
#include "xdata/xtable_bstate.h"
#include "xdata/xblock_statistics_data.h"
#include "xdata/xblock_statistics_cons_data.h"

NS_BEG2(tcash, blockmaker)

class xlighttable_builder_para_t : public xblock_builder_para_face_t {
 public:
    xlighttable_builder_para_t(const std::vector<xblock_ptr_t> & units, const xblockmaker_resources_ptr_t & resources, const std::vector<data::xlightunit_tx_info_ptr_t> & txs_info)
    : xblock_builder_para_face_t(resources, txs_info), m_batch_units(units) {}
    virtual ~xlighttable_builder_para_t() {}

    const std::vector<xblock_ptr_t> &               get_batch_units() const {return m_batch_units;}

 private:
    std::vector<xblock_ptr_t>                   m_batch_units;
};

class xfulltable_builder_para_t : public xblock_builder_para_face_t {
 public:
    xfulltable_builder_para_t(const data::xtablestate_ptr_t & latest_offstate,
                              const std::vector<xblock_ptr_t> & blocks_from_last_full,
                              const xblockmaker_resources_ptr_t & resources)
    : xblock_builder_para_face_t(resources), m_latest_offstate(latest_offstate), m_blocks_from_last_full(blocks_from_last_full) {}
    virtual ~xfulltable_builder_para_t() {}

    const data::xtablestate_ptr_t & get_latest_offstate() const {
        return m_latest_offstate;
    }
    const std::vector<xblock_ptr_t> &       get_blocks_from_last_full() const {return m_blocks_from_last_full;}
 private:
    data::xtablestate_ptr_t m_latest_offstate;
    std::vector<xblock_ptr_t>       m_blocks_from_last_full;
};

class xfulltable_builder_t : public xblock_builder_face_t {
 public:
    virtual xblock_ptr_t        build_block(const xblock_ptr_t & prev_block,
                                            const xobject_ptr_t<base::xvbstate_t> & prev_bstate,
                                            const data::xblock_consensus_para_t & cs_para,
                                            xblock_builder_para_ptr_t & build_para);
    void                        make_binlog(const base::xauto_ptr<base::xvheader_t> & _temp_header,
                                            const xobject_ptr_t<base::xvbstate_t> & prev_bstate,
                                            std::string & property_binlog,
                                            std::map<std::string, std::string> & property_hashs);

protected:
    data::xstatistics_data_t make_block_statistics(const std::vector<xblock_ptr_t> & blocks);
    data::xstatistics_cons_data_t make_block_statistics_cons(const std::vector<xblock_ptr_t> & blocks);
};
class xemptytable_builder_t : public xblock_builder_face_t {
 public:
    virtual xblock_ptr_t        build_block(const xblock_ptr_t & prev_block,
                                            const xobject_ptr_t<base::xvbstate_t> & prev_bstate,
                                            const data::xblock_consensus_para_t & cs_para,
                                            xblock_builder_para_ptr_t & build_para);
};

class xrelay_block_builder_t : public xblock_builder_face_t {
 public:
    virtual xblock_ptr_t        build_block(const xblock_ptr_t & prev_block,
                                            const xobject_ptr_t<base::xvbstate_t> & prev_bstate,
                                            const data::xblock_consensus_para_t & cs_para,
                                            xblock_builder_para_ptr_t & build_para);
};

NS_END2
