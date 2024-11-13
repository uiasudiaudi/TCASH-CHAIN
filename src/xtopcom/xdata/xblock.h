// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <json/json.h>

#if defined(__clang__)

#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wpedantic"

#elif defined(__GNUC__)

#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wpedantic"

#elif defined(_MSC_VER)

#    pragma warning(push, 0)

#endif

#include "xvledger/xvblock.h"
#include "xvledger/xvblockstore.h"
#include "xvledger/xaccountindex.h"
#include "xvledger/xreceiptid.h"
#include "xvledger/xmerkle.hpp"

#if defined(__clang__)
#    pragma clang diagnostic pop
#elif defined(__GNUC__)
#    pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#    pragma warning(pop)
#endif

#include "xbase/xobject_ptr.h"
#include "xdata/xcons_transaction.h"
#include "xdata/xdata_common.h"
#include "xdata/xlightunit_info.h"
#include "xcommon/xaccount_address.h"
#include "xcommon/common.h"

NS_BEG2(tcash, data)

using base::xaccount_index_t;
using xvheader_ptr_t = xobject_ptr_t<base::xvheader_t>;

class xblock_t : public base::xvblock_t {
 public:
    static std::string get_block_base_path(base::xvblock_t* block) {return block->get_account() + ':' + std::to_string(block->get_height());}
    static xobject_ptr_t<xblock_t> raw_vblock_to_object_ptr(base::xvblock_t* block);
    static void  txs_to_receiptids(const std::vector<xlightunit_tx_info_ptr_t> & txs_info, base::xreceiptid_check_t & receiptid_check);
    static void  register_object(base::xcontext_t & _context);
public:
    xblock_t(enum_xdata_type type);
    xblock_t(base::xvheader_t & header, base::xvqcert_t & cert, enum_xdata_type type);
    xblock_t(base::xvheader_t & header, base::xvqcert_t & cert, base::xvinput_t* input, base::xvoutput_t* output, enum_xdata_type type);

#ifdef XENABLE_PSTACK  // tracking memory
    virtual int32_t add_ref() override;
    virtual int32_t release_ref() override;
#endif

 protected:
    virtual ~xblock_t();
 private:
    xblock_t(const xblock_t &);
    xblock_t & operator = (const xblock_t &);

 public:
    virtual int32_t     full_block_serialize_to(base::xstream_t & stream);  // for block sync
    static  base::xvblock_t*    full_block_read_from(base::xstream_t & stream);  // for block sync
    virtual void parse_to_json(Json::Value & root, const std::string & rpc_version) {};
 public:
    inline base::enum_xvblock_level get_block_level() const {return get_header()->get_block_level();}
    inline base::enum_xvblock_class get_block_class() const {return get_header()->get_block_class();}
    inline bool     is_unitblock() const {return get_block_level() == base::enum_xvblock_level_unit;}
    inline bool     is_tableblock() const {return get_block_level() == base::enum_xvblock_level_table;}
    inline bool     is_fullblock() const {return get_block_class() == base::enum_xvblock_class_full;}
    inline bool     is_emptyblock() const {return get_block_class() == base::enum_xvblock_class_nil;}
    inline bool     is_fulltable() const {return get_block_level() == base::enum_xvblock_level_table && get_block_class() == base::enum_xvblock_class_full;}
    inline bool     is_lighttable() const {return get_block_level() == base::enum_xvblock_level_table && get_block_class() == base::enum_xvblock_class_light;}
    inline bool     is_emptytable() const {return get_block_level() == base::enum_xvblock_level_table && get_block_class() == base::enum_xvblock_class_nil;}

 public:
    virtual void                        dump_block_data(Json::Value & json) const {return;}
    xtransaction_ptr_t                  query_raw_transaction(const std::string & txhash) const;
    uint32_t                            query_tx_size(const std::string & txhash) const;

 public:
    uint64_t    get_timerblock_height() const {return get_clock();}
    std::string get_block_owner()const {return get_account();}
};

using xblock_ptr_t = xobject_ptr_t<xblock_t>;
using xvblock_ptr_t = xobject_ptr_t<base::xvblock_t>;

NS_END2
