// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <string>
#include "xbase/xutl.h"
// TODO(jimmy) #include "xbase/xvledger.h"
#include "xbasic/xutility.h"
#include "xdata/xfull_tableblock.h"
#include "xdata/xrootblock.h"

NS_BEG2(tcash, data)

xfulltable_block_para_t::xfulltable_block_para_t(const std::string & snapshot, const xstatistics_data_t & statistics_data, const int64_t tgas_balance_change) {
    m_snapshot = snapshot;
    m_block_statistics_data = statistics_data;
    m_tgas_balance_change = tgas_balance_change;
}

xfulltable_block_para_t::xfulltable_block_para_t(const std::string & snapshot, const xstatistics_cons_data_t & statistics_data, const int64_t tgas_balance_change) {
    m_snapshot = snapshot;
    m_block_statistics_cons_data = statistics_data;
}

xfull_tableblock_t::xfull_tableblock_t(base::xvheader_t & header, base::xvqcert_t & cert, base::xvinput_t* input, base::xvoutput_t* output)
: xblock_t(header, cert, input, output, (enum_xdata_type)object_type_value) {
    XMETRICS_GAUGE_DATAOBJECT(metrics::dataobject_block_fulltable, 1);
}
xfull_tableblock_t::xfull_tableblock_t()
: xblock_t((enum_xdata_type)object_type_value) {
    XMETRICS_GAUGE_DATAOBJECT(metrics::dataobject_block_fulltable, 1);
}
xfull_tableblock_t::~xfull_tableblock_t() {
    XMETRICS_GAUGE_DATAOBJECT(metrics::dataobject_block_fulltable, -1);
}

base::xobject_t * xfull_tableblock_t::create_object(int type) {
    (void)type;
    return new xfull_tableblock_t;
}

void * xfull_tableblock_t::query_interface(const int32_t _enum_xobject_type_) {
    if (object_type_value == _enum_xobject_type_)
        return this;
    return xvblock_t::query_interface(_enum_xobject_type_);
}

void xfull_tableblock_t::parse_to_json(Json::Value & root, const std::string & rpc_version) {

#ifndef  XBUILD_CONSORTIUM
     root["statistics"] = get_table_statistics().to_json_object<Json::Value>();
#else 
     root["statistics"] = get_table_statistics_cons_data().to_json_object<Json::Value>();
#endif 
}

xstatistics_data_t xfull_tableblock_t::get_table_statistics() const {
    std::string resource_str = query_input_resource(base::xvinput_t::RESOURCE_NODE_SIGN_STATISTICS);
    xassert(!resource_str.empty());
    xstatistics_data_t statistics_data;
    statistics_data.deserialize_based_on<base::xstream_t>({ std::begin(resource_str), std::end(resource_str) });
    return statistics_data;
}

std::string xfull_tableblock_t::get_table_statistics_string() const {
    std::string resource_str = query_input_resource(base::xvinput_t::RESOURCE_NODE_SIGN_STATISTICS);
    xassert(!resource_str.empty());
    return resource_str;
}

xstatistics_cons_data_t xfull_tableblock_t::get_table_statistics_cons_data() const {
    std::string resource_str = query_input_resource(base::xvinput_t::RESOURCE_NODE_SIGN_STATISTICS);
    xassert(!resource_str.empty());
    xstatistics_cons_data_t statistics_data;
    statistics_data.deserialize_based_on<base::xstream_t>({ std::begin(resource_str), std::end(resource_str) });
    return statistics_data;
}

NS_END2
