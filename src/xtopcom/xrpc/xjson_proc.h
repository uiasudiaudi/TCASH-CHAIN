// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once
#include <string>
#include <memory>
#include <json/json.h>
#include "xbase/xutl.h"
#include "xdata/xtransaction.h"
#include "prerequest/xpre_request_data.h"
#include "xrpc_define.h"

NS_BEG2(tcash, xrpc)
class xjson_proc_t
{
public:
    void parse_json(const xpre_request_data_t& pre_request_data);
    void parse_json(const std::string& content);
    std::string get_response();
    std::string get_request();
public:
    Json::Reader           m_reader;
    Json::FastWriter       m_writer;
    Json::Value            m_request_json;
    Json::Value            m_response_json;
    data::xtransaction_ptr_t      m_tx_ptr;
    enum_xrpc_tx_type       m_tx_type;
    unordered_set<string>   m_account_set{};
};

NS_END2
