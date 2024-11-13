// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xconfig/xpredefined_configurations.h"
#include "xtcashcl/include/tcashcl.h"
#include "xtcashcl/include/user_info.h"

#include <string>

#if defined(XBUILD_DEV) || defined(XBUILD_CI)
#   define SEED_URL "http://unreachable.org/"
#   define SERVER_HOST_PORT_HTTP "127.0.0.1:19081"
#   define SERVER_HOST_PORT_WS "127.0.0.1:19085"
#elif defined(XBUILD_GALILEO)
#   define SEED_URL "http://galileo.edge.tcashnetwork.org"
#   define SERVER_HOST_PORT_HTTP "206.189.201.14:19081"
#   define SERVER_HOST_PORT_WS "206.189.201.14:19085"
#elif defined(XBUILD_BOUNTY)
#   define SEED_URL "http://bounty.edge.tcashnetwork.org"
#   define SERVER_HOST_PORT_HTTP "161.35.98.159:19081"
#   define SERVER_HOST_PORT_WS "161.35.98.159:19085"
#else
#   define SEED_URL "http://mainnet.edge.tcashnetwork.org/"
#   define SERVER_HOST_PORT_HTTP "206.189.227.204:19081"
#   define SERVER_HOST_PORT_WS "206.189.227.204:19085"
#endif

#define SERVER_ERROR "Server Connection Error"

constexpr uint64_t MinDeposit = 1;
constexpr uint64_t MinTxDeposit = 100000;
//constexpr uint64_t tcash_UNIT = 1e6;  // 1tcash = 1e6 utcash
//#define ASSET_tcash(num) ((uint64_t)((num)*tcash_UNIT))

extern xChainSDK::user_info g_userinfo;
extern xChainSDK::user_info copy_g_userinfo;
extern std::string g_keystore_dir;
extern std::string g_data_dir;
extern std::string g_pw_hint;
extern std::string g_server_host_port;
extern std::string g_edge_domain;
extern std::atomic_bool auto_query;
