// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"
#include "xbasic/xbyte_buffer.h"
#include "xbasic/xserializable_based_on.h"

#include <map>

NS_BEG2(tcash, data)

class xtcash_receipt_data_store: public xserializable_based_on<void>  {
public:
    xtcash_receipt_data_store() =  default;
    xtcash_receipt_data_store(xtcash_receipt_data_store const&) = default;
    xtcash_receipt_data_store& operator=(xtcash_receipt_data_store const&) = default;
    xtcash_receipt_data_store(xtcash_receipt_data_store&&) = default;
    xtcash_receipt_data_store& operator=(xtcash_receipt_data_store&&) = default;
    ~xtcash_receipt_data_store() =  default;

    void receipt_data(std::map<std::string, xbyte_buffer_t> const& receipt_data);
    // std::map<std::string, xbyte_buffer_t>  receipt_data() const;

    xbyte_buffer_t  receipt_data_item(std::string const& key) const;
    void remove_item(std::string const& key);
    void add_item(std::string const& key, xbyte_buffer_t value);

    bool  item_exist(std::string const& key) const;
    bool  empty() const;

private:
    int32_t do_read(base::xstream_t & stream) override;
    int32_t do_write(base::xstream_t & stream) const override;


private:
    std::map<std::string, xbyte_buffer_t> m_receipt_data;
};

using xreceipt_data_store_t = xtcash_receipt_data_store;

NS_END2
