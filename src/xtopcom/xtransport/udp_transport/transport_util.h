// Copyright (c) 2017-2019 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xpbase/base/tcash_string_util.h"
#if defined(XCXX20)
#include "xtransport/proto/ubuntu/transport.pb.h"
#else
#include "xtransport/proto/centos/transport.pb.h"
#endif

namespace tcash {
namespace transport {

inline std::string FormatMsgid(const protobuf::RoutingMessage & message) {
    return base::StringUtil::str_fmt("msgid(%u)", message.id());
}

}  // namespace transport
}  // namespace tcash

#ifndef NDEBUG
   // reduce hot log
#    define tcash_NETWORK_DEBUG_FOR_PROTOMESSAGE(pre, message)                                                                                                                       \
        do {                                                                                                                                                                       \
            xdbg("%s %s: %s [hop_num:%d] [type:%d]",                                                                                                                               \
                 tcash::transport::FormatMsgid(message).c_str(),                                                                                                                     \
                 std::string(pre).c_str(),                                                                                                                                         \
                 message.debug().c_str(),                                                                                                                                          \
                 message.hop_num(),                                                                                                                                                \
                 message.type());                                                                                                                                                  \
        } while (0)

/*
#define tcash_NETWORK_DEBUG_FOR_PROTOMESSAGE2(pre, message, thread_id) \
    do { \
        tcash_DEBUG("%s %s: %s [hop_num:%d] [type:%d] at thread(%d)", \
                tcash::transport::FormatMsgid(message).c_str(), \
                std::string(pre).c_str(), message.debug().c_str(), message.hop_num(), message.type(),thread_id); \
    } while (0)
    */

//#define tcash_NETWORK_DEBUG_FOR_PROTOMESSAGE(pre, message)
#    define tcash_NETWORK_DEBUG_FOR_PROTOMESSAGE2(pre, message, thread_id)

#else
#    define tcash_NETWORK_DEBUG_FOR_PROTOMESSAGE(pre, message)
#    define tcash_NETWORK_DEBUG_FOR_PROTOMESSAGE2(pre, message, thread_id)
#endif
