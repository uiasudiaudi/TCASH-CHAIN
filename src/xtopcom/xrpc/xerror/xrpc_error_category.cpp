// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include "xrpc_error_category.h"
#include "xrpc_error_code.h"
#include <string>
NS_BEG2(tcash, xrpc)
using std::string;

static string xcodec_errc_map(int const errc) noexcept {
    auto const ec = static_cast<enum_xrpc_error_code>(errc);
    switch (ec) {
        case enum_xrpc_error_code::rpc_param_json_parser_error:
            return "rpc param json parser error";
        case enum_xrpc_error_code::rpc_param_param_lack:
            return "rpc param not exist or empty";
        case enum_xrpc_error_code::rpc_shard_exec_error:
            return "rpc_shard_exec_error";
        default:
            return string("Unknown code " + std::to_string(errc));
    }
}

class xrpc_category final : public std::error_category
{
    const char* name() const noexcept override {
        return "xrpc::error";
    }

    std::string message(int errc) const override {
        return xcodec_errc_map(errc);
    }
};

const std::error_category& xrpc_get_category() {
    static xrpc_category category{};
    return category;
}
NS_END2
