// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#pragma once

#include "xbase/xns_macro.h"

#include <map>
#include <memory>
#include <string>

NS_BEG2(tcash, xhttp)

// fwd:
class HttpsClientWrapper;

/// @brief blocking https client base
class xtcash_https_client_base {
public:
    xtcash_https_client_base() = delete;
    virtual ~xtcash_https_client_base();

    xtcash_https_client_base(std::string const & url);

protected:
    std::string request_get();

    std::string request_get(std::string const & path);

    std::string request_post_string(std::string const & path, std::string const & request);

    std::string request_post_json(std::string const & path, std::string const & json_request);

private:
    std::shared_ptr<HttpsClientWrapper> m_client;

    std::string default_request_path{""};
};

using xhttps_client_base_t = xtcash_https_client_base;

NS_END2