// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#pragma once

#include "xbase/xns_macro.h"

#include <functional>
#include <map>
#include <memory>
#include <string>

NS_BEG2(tcash, xhttp)

// fwd:
class HttpClientWrapper;

/// @brief blocking http client base
class xtcash_http_client_base {
public:
    xtcash_http_client_base() = delete;
    virtual ~xtcash_http_client_base();

    xtcash_http_client_base(std::string const & url);
    xtcash_http_client_base(std::pair<std::string, uint16_t> const & remote_host_port);
    xtcash_http_client_base(std::string const & remote_ip, uint16_t remote_port);

public:
    std::string percent_encode(std::string const & data);

protected:
    std::string request_get();

    std::string request_get(std::string const & path);

    std::string request_post_string(std::string const & path, std::string const & request);

    std::string request_post_json(std::string const & path, std::string const & json_request);

private:
    std::shared_ptr<HttpClientWrapper> m_client;

    std::string default_request_path{""};
};

using xhttp_client_base_t = xtcash_http_client_base;

/// @brief async http client base
class xtcash_http_client_async_base {
    // todo could add more interface && base request api
public:
    xtcash_http_client_async_base() = delete;
    virtual ~xtcash_http_client_async_base();

    xtcash_http_client_async_base(std::string const & ip_port);

protected:
    void request_post_string(std::string const & path, std::string const & request, std::function<void(std::string const &)> callback);
    void run_io_service();

private:
    std::shared_ptr<HttpClientWrapper> m_client;
};
using xhttp_client_async_base_t = xtcash_http_client_async_base;

NS_END2