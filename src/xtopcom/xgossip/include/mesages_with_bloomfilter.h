// Copyright (c) 2017-2019 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbasic/xlru_cache_specialize.h"
#include "xpbase/base/kad_key/kadmlia_key.h"
#include "xpbase/base/tcash_timer.h"
#include "xpbase/base/uint64_bloomfilter.h"
#if defined(XCXX20)
#include "xtransport/proto/ubuntu/transport.pb.h"
#else
#include "xtransport/proto/centos/transport.pb.h"
#endif

#include <memory>

namespace tcash {

namespace gossip {

struct MessageKey {
public:
    MessageKey() {
    }
    MessageKey(uint32_t hash) : msg_hash(hash) {
    }
    MessageKey(std::string hash) : msg_hash(std::atoi(hash.c_str())){}
    ~MessageKey() {
    }

    bool operator==(const MessageKey & key) const {
        return msg_hash == key.msg_hash;
    }

    bool operator<(const MessageKey & key) const {
        if (msg_hash < key.msg_hash) {
            return true;
        } else if (msg_hash == key.msg_hash) {
            return true;
        }
        return false;
    }

    uint32_t msg_hash;
};
}  // namespace gossip
}  // namespace tcash

namespace std {
template <>
struct hash<tcash::gossip::MessageKey> {
    size_t operator()(const tcash::gossip::MessageKey & k) const {
        return static_cast<size_t>(k.msg_hash);
    }
};
}  // namespace std

namespace tcash {
namespace gossip {
// struct MessageHash {
//     size_t operator()(const MessageKey & key) const {
//         return std::hash<uint64_t>()(key.msg_hash);
//     }
// };

class MessageWithBloomfilter {
public:
    static MessageWithBloomfilter * Instance();
    std::shared_ptr<base::Uint64BloomFilter> GetMessageBloomfilter(transport::protobuf::RoutingMessage & message);
    bool StcashGossip(const MessageKey &, uint32_t);

    void UpdateHandle(MessageKey const & message_key, uint64_t & sit1, uint64_t & sit2);

private:
    MessageWithBloomfilter() {
    }
    ~MessageWithBloomfilter() {
    }

    std::shared_ptr<base::Uint64BloomFilter> MergeBloomfilter(const uint32_t &,
                                                              std::shared_ptr<base::Uint64BloomFilter> & bloomfilter,
                                                              const uint32_t & stcash_times,
                                                              bool & stcash_gossip);

    // static const uint32_t kMaxMessageQueueSize = 1048576u;
    static const uint32_t kMaxMessageQueueSize = 10000u;

    basic::xlru_cache_specialize<MessageKey, uint8_t> message_bloomfilter_map_{kMaxMessageQueueSize};

    basic::xlru_cache_specialize<MessageKey,std::pair<uint64_t,uint64_t>> message_dispatch_map_{kMaxMessageQueueSize};

    DISALLOW_COPY_AND_ASSIGN(MessageWithBloomfilter);
};

}  // namespace gossip

}  // namespace tcash
