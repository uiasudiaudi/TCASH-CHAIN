// Copyright (c) 2018-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xdata/xrelayblock_build.h"
#include "xcommon/xerror/xerror.h"

namespace tcash {
namespace data {

xrelayblock_crosstx_info_t::xrelayblock_crosstx_info_t(xeth_transaction_t const& _tx, xeth_receipt_t const& _receipt)
: tx(_tx), receipt(_receipt), speed_type(0), chain_bit(0) {
}

xrelayblock_crosstx_info_t::xrelayblock_crosstx_info_t(xeth_transaction_t const& _tx, xeth_receipt_t const& _receipt, const uint8_t _type, const evm_common::u256& _chain_bit)
: tx(_tx), receipt(_receipt), speed_type(_type), chain_bit(_chain_bit) {

}
void    xrelayblock_crosstx_info_t::streamRLP(evm_common::RLPStream& _s) const {
    _s.appendList(4);
    _s << tx.encodeBytes();
    _s << receipt.encodeBytes();
    _s << (uint8_t)speed_type;
    _s << chain_bit;
}

void    xrelayblock_crosstx_info_t::decodeRLP(evm_common::RLP const& _r, std::error_code & ec) {
    if (!_r.isList() || _r.itemCount() != 4) {
        ec = common::error::xerrc_t::invalid_rlp_stream;
        xerror("xrelayblock_crosstx_info_t::decodeRLP fail item count,%d", _r.itemCount());
        return;
    }
    xbytes_t tx_bytes = _r[0].toBytes();
    tx.decodeBytes(tx_bytes, ec);
    if (ec) {
        return;
    }
    xbytes_t receipt_bytes = _r[1].toBytes();
    receipt.decodeBytes(receipt_bytes, ec);
    speed_type = (uint8_t)_r[2];
    chain_bit = _r[3].toInt<evm_common::u256>();
}

xbytes_t xrelayblock_crosstx_info_t::encodeBytes() const {
    evm_common::RLPStream _s;
    streamRLP(_s);
    return _s.out();
}
void xrelayblock_crosstx_info_t::decodeBytes(xbytes_t const& _d, std::error_code & ec) {
    evm_common::RLP _r(_d);
    decodeRLP(_r, ec);
}

void xrelayblock_crosstx_infos_t::streamRLP(evm_common::RLPStream& _s) const {
    _s.appendList(tx_infos.size());
    for (auto & v : tx_infos) {
        v.streamRLP(_s);
    }
}

void xrelayblock_crosstx_infos_t::decodeRLP(evm_common::RLP const& _r, std::error_code & ec) {
    if (!_r.isList() || _r.itemCount() == 0) {
        ec = common::error::xerrc_t::invalid_rlp_stream;
        xerror("xrelayblock_crosstx_infos_t::decodeRLP fail item count,%d", _r.itemCount());
        return;
    }
    for (size_t i = 0; i < _r.itemCount(); i++) {
        evm_common::RLP _r_i = _r[i];
        xrelayblock_crosstx_info_t info;
        info.decodeRLP(_r_i, ec);
        if (ec) {
            return;
        }
        tx_infos.push_back(info);
    }
}

xbytes_t xrelayblock_crosstx_infos_t::encodeBytes() const {
    evm_common::RLPStream _s;
    streamRLP(_s);
    return _s.out();
}
void xrelayblock_crosstx_infos_t::decodeBytes(xbytes_t const& _d, std::error_code & ec) {
    evm_common::RLP _r(_d);
    decodeRLP(_r, ec);
}
std::string xrelayblock_crosstx_infos_t::serialize_to_string() const {
    xbytes_t _bs = encodeBytes();
    return tcash::to_string(_bs);
}
void xrelayblock_crosstx_infos_t::serialize_from_string(const std::string & bin_data, std::error_code & ec) {
    xbytes_t _bs = tcash::to_bytes(bin_data);
    decodeBytes(_bs, ec);
}

} // namespace data
} // namespace tcash
