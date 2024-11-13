#pragma once

#include <string>
#include <json/json.h>
// #include "xbase/xobject.h"
// #include "xcodec/xmsgpack_codec.hpp"
// #include "xdata/xcodec/xmsgpack/xelection_association_result_store_codec.hpp"
// #include "xdata/xelection/xelection_association_result_store.h"
// #include "xdata/xelection/xelection_cluster_result.h"
// #include "xdata/xelection/xelection_result_store.h"
// #include "xdata/xelection/xstandby_result_store.h"
#include "xdata/xunit_bstate.h"
// #include "xgrpcservice/xgrpc_service.h"

// #include "xvledger/xvtxstore.h"
// #include "xvledger/xvledger.h"
// #include "xrpc/xjson_proc.h"

NS_BEG2(tcash, xrpc)
enum class xtcash_enum_full_node_compatible_mode {
    incompatible,
    compatible,
};
using xfull_node_compatible_mode_t = xtcash_enum_full_node_compatible_mode;

class xrpc_query_func {
public:
    xrpc_query_func() {}
    bool is_prop_name_already_set_property(const std::string & prop_name);
    bool is_prop_name_not_set_property(const std::string & prop_name);
    bool query_special_property(Json::Value & jph, const std::string & owner, const std::string & prop_name, data::xunitstate_ptr_t unitstate, bool compatible_mode);
    void query_account_property_base(Json::Value & jph, const std::string & owner, const std::string & prop_name, data::xunitstate_ptr_t unitstate, bool compatible_mode);
    void query_account_property(Json::Value & jph, const std::string & owner, const std::string & prop_name, xfull_node_compatible_mode_t compatible_mode);
    void query_account_property(Json::Value & jph, const std::string & owner, const std::string & prop_name, const uint64_t height, xfull_node_compatible_mode_t compatible_mode);
};
NS_END2