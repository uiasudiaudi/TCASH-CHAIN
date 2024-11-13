
// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#pragma once

#include "xcommon/xbloom9.h"
#include "xcommon/xtcash_log.h"

NS_BEG2(tcash, data)

class xtcash_store_receipt_t {
public:
    xtcash_store_receipt_t() = default;
    xtcash_store_receipt_t(const common::xtcash_logs_t & _logs): m_logs(_logs){};
    ~xtcash_store_receipt_t() = default;

    xbytes_t encodeBytes() const;
    void decodeBytes(xbytes_t const & _d, std::error_code & ec);

public:
    void create_bloom();
    const evm_common::xbloom9_t & get_logsBloom() const {
        return m_logsBloom;
    }
    void set_logs(common::xtcash_logs_t const & logs) {
        m_logs = logs;
    }
    const common::xtcash_logs_t & get_logs() const {
        return m_logs;
    }

protected:
    void streamRLP(evm_common::RLPStream & _s) const;
    void decodeRLP(evm_common::RLP const & _r, std::error_code & ec);

private:
    evm_common::xbloom9_t m_logsBloom;
    common::xtcash_logs_t m_logs;
};

NS_END2