// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "xdata/xtransaction_v2.h"

#include "xbase/xmem.h"
#include "xbase/xutl.h"
#include "xcrypto/xckey.h"
#include "xbase/xcontext.h"
#include "xdata/xdata_error.h"
#include "xbasic/xmodule_type.h"
#include "xdata/xaction_parse.h"
#include "xdata/xdata_defines.h"
#include "xdata/xgenesis_data.h"
#include "xdata/xmemcheck_dbg.h"
#include "xstatistic/xbasic_size.hpp"
#include "xdata/xnative_contract_address.h"

#include <cinttypes>
// #include <iostream>

using namespace tcash::base;

NS_BEG3(tcash, data, details)

xtcash_target_address::xtcash_target_address(common::xaccount_address_t original) noexcept : original_{std::move(original)} {
#if !defined(NDEBUG)
    if (common::is_eoa_address(original_)) {
        assert(adjusted_.empty());
        assert(!original_.has_assigned_table_id());
    } else {
        // Note: block hole address is not eoa account, since its private key should be unknown.
        assert(adjusted_.has_assigned_table_id() || original_.has_assigned_table_id() || original_ == common::black_hole_system_address);
    }
#endif
}

xtcash_target_address::xtcash_target_address(common::xaccount_address_t original, common::xtable_id_t const optional_table_id)
  : original_{std::move(original)}, adjusted_{common::append_table_id(original_.base_address(), optional_table_id)} {
    assert(!original_.has_assigned_table_id());
    assert(adjusted_.has_assigned_table_id());
    assert(!optional_table_id.empty());
}

xtcash_target_address xtcash_target_address::build_from(common::xaccount_address_t const & src, common::xaccount_address_t dst) {
    if (common::is_t2_address(dst) && common::address_belongs_to_zone(dst, common::xconsensus_zone_id)) {
        if (!dst.has_assigned_table_id()) {
            return xtcash_target_address{std::move(dst), src.table_id()};
        }

        return xtcash_target_address{std::move(dst)};
    }

    return xtcash_target_address{std::move(dst)};
}

common::xaccount_address_t const & xtcash_target_address::original() const noexcept {
    return original_;
}

common::xaccount_address_t const & xtcash_target_address::adjusted() const noexcept {
    return adjusted_;
}

common::xaccount_address_t const & xtcash_target_address::address() const noexcept {
#if !defined(NDEBUG)
    if (common::is_eoa_address(original_)) {
        assert(adjusted_.empty());
        assert(!original_.has_assigned_table_id());
    } else {
        assert(adjusted_.has_assigned_table_id() || original_.has_assigned_table_id() || original_ == common::black_hole_system_address);
    }
#endif
    return !adjusted_.empty() ? adjusted_ : original_;
}

NS_END3

namespace tcash { namespace data {

xtransaction_v2_t::xtransaction_v2_t() : xstatistic::xstatistic_obj_face_t(xstatistic::enum_statistic_tx_v2) {
    MEMCHECK_ADD_TRACE(this, "tx_create");
    XMETRICS_GAUGE_DATAOBJECT(metrics::dataobject_xtransaction_t, 1);
}

xtransaction_v2_t::~xtransaction_v2_t() {
    statistic_del();
    MEMCHECK_REMOVE_TRACE(this);
    XMETRICS_GAUGE_DATAOBJECT(metrics::dataobject_xtransaction_t, -1);
}

void xtransaction_v2_t::construct_tx(enum_xtransaction_type tx_type, const uint16_t expire_duration, const uint32_t deposit, const uint32_t nonce, const std::string & memo, const xtx_action_info & info) {
    set_tx_type(tx_type);
    set_expire_duration(expire_duration);
    set_deposit(deposit);
    set_premium_price(0);
    set_last_nonce(nonce);
    set_fire_timestamp(get_gmttime_s());

    set_source(info.m_source_addr, info.m_source_action_name, info.m_source_action_para);
    set_target(info.m_target_addr, info.m_target_action_name, info.m_target_action_para);
}

int32_t xtransaction_v2_t::do_write_without_hash_signature(base::xstream_t & out) const {
    const int32_t begin_pos = out.size();
    out.write_compact_var(static_cast<uint16_t>(m_transaction_type));
    out.write_compact_var(m_expire_duration);
    out.write_compact_var(m_fire_timestamp);

    std::string compact_source_account = xvaccount_t::compact_address_to(m_source_addr.to_string());
    out << compact_source_account;
    std::string compact_target_account = xvaccount_t::compact_address_to(target_address_.original().to_string());
    out << compact_target_account;
    out.write_compact_var(m_edge_nodeid);

    out.write_compact_var(m_amount);
    out.write_compact_var(m_token_name);
    out.write_compact_var(m_last_trans_nonce);
    out.write_compact_var(m_deposit);
    out.write_compact_var(m_premium_price);
    out.write_compact_var(m_memo);
    out.write_compact_var(m_ext);

    if(m_transaction_type != xtransaction_type_transfer) {
        out.write_compact_var(m_source_action_name);
        out.write_compact_var(m_source_action_para);
        out.write_compact_var(m_target_action_name);
        out.write_compact_var(m_target_action_para);
    }

    const int32_t end_pos = out.size();
    return (end_pos - begin_pos);
}

int32_t xtransaction_v2_t::do_uncompact_write_without_hash_signature(base::xstream_t & stream) const {
    const int32_t begin_pos = stream.size();
    stream << static_cast<uint16_t>(m_transaction_type);
    stream << m_expire_duration;
    stream << m_fire_timestamp;
    stream << m_source_addr;
    stream << target_address_.original();
    stream << m_edge_nodeid;

    stream << m_amount;
    stream << m_token_name;
    stream << m_last_trans_nonce;
    stream << m_deposit;
    stream << m_premium_price;
    stream << m_memo;
    stream << m_ext;

    if(m_transaction_type != xtransaction_type_transfer) {
        stream << m_source_action_name;
        stream << m_source_action_para;
        stream << m_target_action_name;
        stream << m_target_action_para;
    }

    const int32_t end_pos = stream.size();
    return (end_pos - begin_pos);
}

int32_t xtransaction_v2_t::do_read_without_hash_signature(base::xstream_t & in) {
    const int32_t begin_pos = in.size();
    uint16_t tmp_transaction_type;
    in.read_compact_var(tmp_transaction_type);
    set_tx_type(static_cast<enum_xtransaction_type>(tmp_transaction_type));
    in.read_compact_var(m_expire_duration);
    in.read_compact_var(m_fire_timestamp);

    std::string compact_source_account;
    in >> compact_source_account;
    std::string const src_address_string = xvaccount_t::compact_address_from(compact_source_account);
    m_source_addr = common::xaccount_address_t::build_from(src_address_string);
    std::string compact_target_account;
    in >> compact_target_account;
    std::string const unadjusted_target_address_string = xvaccount_t::compact_address_from(compact_target_account);
    target_address_ = xtarget_address_t::build_from(m_source_addr, common::xaccount_address_t::build_from(unadjusted_target_address_string));
    in.read_compact_var(m_edge_nodeid);

    in.read_compact_var(m_amount);
    in.read_compact_var(m_token_name);
    in.read_compact_var(m_last_trans_nonce);
    in.read_compact_var(m_deposit);
    in.read_compact_var(m_premium_price);
    in.read_compact_var(m_memo);
    in.read_compact_var(m_ext);

    if(m_transaction_type != xtransaction_type_transfer) {
        in.read_compact_var(m_source_action_name);
        in.read_compact_var(m_source_action_para);
        in.read_compact_var(m_target_action_name);
        in.read_compact_var(m_target_action_para);
    }

    const int32_t end_pos = in.size();
    return (begin_pos - end_pos);
}

int xtransaction_v2_t::do_write(base::xstream_t & out) {
    const int32_t begin_pos = out.size();
    do_write_without_hash_signature(out);
    out.write_compact_var(m_authorization);
    const int32_t end_pos = out.size();
    return (end_pos - begin_pos);
}

int xtransaction_v2_t::do_read(base::xstream_t & in) {
    const int32_t begin_pos = in.size();
    do_read_without_hash_signature(in);
    in.read_compact_var(m_authorization);
    set_digest();
    set_tx_len(begin_pos - in.size());
    if (get_tx_type() == xtransaction_type_transfer) {
        data::xproperty_asset asset_out{m_token_name, m_amount};
        data::xaction_t action;
        xaction_asset_out::serialze_to(action, asset_out);
        m_source_action_para = action.get_action_param();
        m_target_action_para = action.get_action_param();
    }
    //std::error_code ec;
    //auto const & target_account_address = target_address();
    //if (ec) {
    //    throw enum_xerror_code_bad_transaction;
    //}
    //auto const & source_account_address = source_address();
    //if (ec) {
    //    throw enum_xerror_code_bad_transaction;
    //}

    //if (is_sys_sharding_contract_address(target_account_address)) {
    //    // auto tableid = data::account_map_to_table_id(source_account_address);
    //    adjust_target_address(source_account_address.table_id());
    //}
    const int32_t end_pos = in.size();
    return (begin_pos - end_pos);
}

#ifdef XENABLE_PSTACK  // tracking memory
int32_t xtransaction_v2_t::add_ref() {
    MEMCHECK_ADD_TRACE(this, "tx_add");
    return base::xdataunit_t::add_ref();
}
int32_t xtransaction_v2_t::release_ref() {
    MEMCHECK_ADD_TRACE(this, "tx_release");
    xdbgassert(get_refcount() > 0);
    return base::xdataunit_t::release_ref();
}
#endif

void xtransaction_v2_t::adjust_target_address(common::xtable_id_t const table_id) {
    //if (m_adjust_target_addr.empty()) {
    //    m_adjust_target_addr =
    //        m_unadjusted_target_addr.has_assigned_table_id() ? m_unadjusted_target_addr : common::xaccount_address_t::build_from(m_unadjusted_target_addr.base_address(), table_id);

    //    xdbg("xtransaction_v2_t::adjust_target_address hash=%s,origin_addr=%s,new_addr=%s",
    //        get_digest_hex_str().c_str(), m_unadjusted_target_addr.to_string().c_str(), m_adjust_target_addr.to_string().c_str());
    //}
}

void xtransaction_v2_t::set_digest() {
    if (m_transaction_hash == uint256_t()) {
        base::xautostream_t<1024> stream(base::xcontext_t::instance());
        do_uncompact_write_without_hash_signature(stream);
        m_transaction_hash = utl::xsha2_256_t::digest((const char*)stream.data(), stream.size());
        XMETRICS_GAUGE(metrics::cpu_hash_256_xtransaction_v2_calc, 1);
    }
}

bool xtransaction_v2_t::unuse_member_check() const {
    if (m_transaction_type == xtransaction_type_transfer && m_token_name != "" && m_token_name != XPROPERTY_ASSET_tcash) {
        return false;
    }
    return true;
}

bool xtransaction_v2_t::transaction_len_check() const {
    if (get_memo().size() > MAX_TRANSACTION_MEMO_SIZE) {
        xwarn("xtransaction_v2_t::transaction_len_check memo size too long.size:%d", get_memo().size());
        return false;
    }

    base::xautostream_t<1024> stream(base::xcontext_t::instance());
    const int32_t begin_pos = stream.size();
    do_write_without_hash_signature(stream);
    stream.write_compact_var(m_authorization);

    uint16_t len = stream.size() - begin_pos;
    if (len != get_tx_len()) {
        xwarn("xtransaction_v2_t::transaction_len_check tx length not match. %d %d", len, get_tx_len());
        return false;
    }
    return true;
}

//int32_t xtransaction_v2_t::make_tx_create_user_account(const std::string & addr) {
//    set_tx_type(xtransaction_type_create_user_account);
//    m_unadjusted_target_addr = common::xaccount_address_t::build_from(addr);
//    return xsuccess;
//}

int32_t xtransaction_v2_t::make_tx_transfer(const data::xproperty_asset & asset) {
    set_tx_type(xtransaction_type_transfer);
    m_token_name = asset.token_symbol();
    set_amount(asset.m_amount);
    return xsuccess;
}

int32_t xtransaction_v2_t::make_tx_run_contract(const data::xproperty_asset & asset_out, const std::string& function_name, const std::string& para) {
    set_tx_type(xtransaction_type_run_contract);

    base::xautostream_t<1024> stream(xcontext_t::instance());
    stream << asset_out.m_token_name;
    stream << asset_out.m_amount;
    m_source_action_para = std::string((char *)stream.data(), stream.size());

    m_target_action_name = function_name;
    m_target_action_para = para;
    return xsuccess;
}

int32_t xtransaction_v2_t::make_tx_run_contract(std::string const & function_name, std::string const & param) {
    data::xproperty_asset asset_out{0};
    return make_tx_run_contract(asset_out, function_name, param);
}

int32_t xtransaction_v2_t::set_different_source_target_address(const std::string & src_addr, const std::string & dst_addr) {
    m_source_addr = common::xaccount_address_t::build_from(src_addr);
    target_address_ = xtarget_address_t::build_from(m_source_addr, common::xaccount_address_t(dst_addr));
    return xsuccess;
}

int32_t xtransaction_v2_t::set_same_source_target_address(const std::string & addr) {
    return set_different_source_target_address(addr, addr);
}

void xtransaction_v2_t::set_last_trans_hash_and_nonce(uint256_t last_hash, uint64_t last_nonce) {
    set_last_nonce(last_nonce);
}

void xtransaction_v2_t::set_source(const std::string & addr, const std::string & action_name, const std::string & para) {
    m_source_addr = common::xaccount_address_t(addr);
    m_source_action_name = action_name;
    m_source_action_para = para;
}

void xtransaction_v2_t::set_target(const std::string & addr, const std::string & action_name, const std::string & para) {
    target_address_ = xtarget_address_t::build_from(m_source_addr, common::xaccount_address_t(addr));
    m_target_action_name = action_name;
    m_target_action_para = para;
}

void xtransaction_v2_t::set_len() {
    base::xautostream_t<1024> stream(base::xcontext_t::instance());
    const int32_t begin_pos = stream.size();
    do_write_without_hash_signature(stream);
    stream.write_compact_var(m_authorization);
    set_tx_len(stream.size() - begin_pos);
}

bool xtransaction_v2_t::digest_check() const {
    return true;
}

std::string xtransaction_v2_t::get_digest_hex_str() const {
    if (m_transaction_hash_str.empty()) {
        m_transaction_hash_str = to_hex_str(m_transaction_hash);
    }
    return m_transaction_hash_str;
}

bool xtransaction_v2_t::check_last_nonce(uint64_t account_nonce) {
    return (get_last_nonce() == account_nonce);
}

void xtransaction_v2_t::set_fire_and_expire_time(uint16_t expire_duration) {
    struct timeval val;
    base::xtime_utl::gettimeofday(&val);
    set_fire_timestamp((uint64_t)val.tv_sec);
    set_expire_duration(expire_duration);
}

bool xtransaction_v2_t::sign_check() const {
    auto const & base_address = source_address().base_address();
    utl::xkeyaddress_t key_address(base_address.to_string());
    uint256_t checkHash = m_transaction_hash;

    if (get_ext() == "0x01") {
        std::string messagePreNum = "\x19";  //"Ethereum Signed Message:\n";
        std::string messagePreInfo = "Ethereum Signed Message:\n";
        std::string txHash = "0x" + to_hex_str(m_transaction_hash);
        std::string personMessage = messagePreNum + messagePreInfo + base::xstring_utl::tostring(txHash.length()) + txHash;
        checkHash = utl::xkeccak256_t::digest((const char *)personMessage.data(), personMessage.size());
    }
    utl::xecdsasig_t signature_obj((uint8_t *)m_authorization.c_str());
    return key_address.verify_signature(signature_obj, checkHash);
}

bool xtransaction_v2_t::pub_key_sign_check(xpublic_key_t const & pub_key) const {
    auto pub_data = base::xstring_utl::base64_decode(pub_key.to_string());
    xdbg("xtransaction_v2_t::pub_key_sign_check pub_key: %s , decode.size():%d,m_transaction_hash:%s", pub_key.to_string().c_str(), pub_data.size(),get_digest_hex_str().c_str());
    utl::xecdsasig_t signature_obj((uint8_t *)m_authorization.data());
    uint8_t out_publickey_data[utl::UNCOMPRESSED_PUBLICKEY_SIZE];
    memcpy(out_publickey_data, pub_data.data(), (size_t)std::min(utl::UNCOMPRESSED_PUBLICKEY_SIZE, (int)pub_data.size()));

    uint256_t checkHash = m_transaction_hash;
    if (get_ext() == "0x01") {
        std::string  messagePreNum = "\x19";//"Ethereum Signed Message:\n";
        std::string  messagePreInfo = "Ethereum Signed Message:\n";
        std::string  txHash = "0x" + to_hex_str(m_transaction_hash);
        std::string personMessage = messagePreNum +  messagePreInfo + base::xstring_utl::tostring(txHash.length()) + txHash;
        checkHash = utl::xkeccak256_t::digest((const char*)personMessage.data(), personMessage.size());
    }
    return utl::xsecp256k1_t::verify_signature(signature_obj, checkHash, out_publickey_data, false);
}

std::string xtransaction_v2_t::dump() const {
    char local_param_buf[256];
    xprintf(local_param_buf,    sizeof(local_param_buf),
    "{transaction v2:hash=%s,type=%u,from=%s,to=%s,nonce=%" PRIu64 ",m_token_name:%s,refcount=%d,this=%p}",
    get_digest_hex_str().c_str(), (uint32_t)get_tx_type(), source_address().to_string().c_str(), target_address().to_string().c_str(),
    get_tx_nonce(), m_token_name.c_str(), get_refcount(), this);
    return std::string(local_param_buf);
}

std::string xtransaction_v2_t::get_source_action_str() const {
    std::string str;
    str += m_source_addr.to_string();
    str += m_source_action_name;
    str += to_hex_str(m_source_action_para);
    return str;
}

std::string xtransaction_v2_t::get_target_action_str() const {
    std::string str;
    str += target_address_.original().to_string();
    str += m_target_action_name;
    str += to_hex_str(m_target_action_para);
    return str;
}

void xtransaction_v2_t::set_action_type() {
    xtransaction_t::set_action_type_by_tx_type(m_transaction_type);
}

void xtransaction_v2_t::parse_to_json(Json::Value& result_json, const std::string & tx_version) const {
    result_json["tx_structure_version"] = xtransaction_version_2;
    result_json["tx_deposit"] = get_deposit();
    result_json["tx_type"] = get_tx_type();
    result_json["tx_len"] = get_tx_len();
    result_json["tx_hash"] = data::uint_to_str(digest().data(), digest().size());
    result_json["tx_expire_duration"] = get_expire_duration();
    result_json["send_timestamp"] = static_cast<Json::UInt64>(get_fire_timestamp());
    result_json["premium_price"] = get_premium_price();
    result_json["last_tx_nonce"] = static_cast<Json::UInt64>(get_last_nonce());
    result_json["note"] = get_memo();
    result_json["ext"] = data::uint_to_str(get_ext().data(), get_ext().size());
    result_json["authorization"] = get_authorization();

    if (tx_version == RPC_VERSION_V2) {
        result_json["sender_account"] = m_source_addr.to_string();
        result_json["receiver_account"] = target_address_.original().to_string();
        result_json["amount"] = static_cast<Json::UInt64>(m_amount);
        result_json["token_name"] = m_token_name;
        result_json["edge_nodeid"] = m_edge_nodeid;

        result_json["sender_action_name"] = m_source_action_name;
        result_json["sender_action_param"] = data::uint_to_str(m_source_action_para.data(), m_source_action_para.size());
        result_json["receiver_action_name"] = m_target_action_name;
        result_json["receiver_action_param"] = data::uint_to_str(m_target_action_para.data(), m_target_action_para.size());
    } else {
        result_json["to_ledger_id"] = 0;
        result_json["from_ledger_id"] = 0;
        result_json["tx_random_nonce"] = 0;
        result_json["last_tx_hash"] = "";
        result_json["challenge_proof"] = "";

        std::string source_action_para = m_source_action_para;
        std::string target_action_para = m_target_action_para;
        if (get_tx_type() == xtransaction_type_transfer) {
            data::xproperty_asset asset_out{m_token_name, m_amount};
            data::xaction_t action;
            xaction_asset_out::serialze_to(action, asset_out);
            source_action_para = action.get_action_param();
            target_action_para = action.get_action_param();
        }
        Json::Value& s_action_json = result_json["sender_action"];
        s_action_json["action_hash"] = "";
        s_action_json["action_type"] = m_source_action_type;
        s_action_json["action_size"] = 0;
        s_action_json["tx_sender_account_addr"] = m_source_addr.to_string();
        s_action_json["action_name"] = m_source_action_name;
        s_action_json["action_param"] = data::uint_to_str(source_action_para.data(), source_action_para.size());
        s_action_json["action_ext"] = "";
        s_action_json["action_authorization"] = "";

        Json::Value& t_action_json = result_json["receiver_action"];
        t_action_json["action_hash"] = "";
        t_action_json["action_type"] = m_target_action_type;
        t_action_json["action_size"] = 0;
        t_action_json["tx_receiver_account_addr"] = target_address_.original().to_string();
        t_action_json["action_name"] = m_target_action_name;
        t_action_json["action_param"] = data::uint_to_str(target_action_para.data(), target_action_para.size());
        t_action_json["action_ext"] = "";
        t_action_json["action_authorization"] = "";
    }
}

void xtransaction_v2_t::construct_from_json(Json::Value& request) {
    set_amount(request["amount"].asUInt64());
    m_token_name = request["token_name"].asString();

    set_tx_type(static_cast<uint16_t>(request["tx_type"].asUInt()));
    set_expire_duration(static_cast<uint16_t>(request["tx_expire_duration"].asUInt()));
    set_deposit(request["tx_deposit"].asUInt());
    set_premium_price(request["premium_price"].asUInt());

    set_last_nonce(request["last_tx_nonce"].asUInt64());
    set_fire_timestamp(request["send_timestamp"].asUInt64());
    set_memo(request["note"].asString());
    set_ext(request["ext"].asString());
    m_edge_nodeid = request["edge_nodeid"].asString();

    auto source_param_vec = hex_to_uint(request["sender_action_param"].asString());
    auto source_para = std::string((char *)source_param_vec.data(), source_param_vec.size());
    set_source(request["sender_account"].asString(), request["sender_action_name"].asString(), source_para);

    auto target_param_vec = hex_to_uint(request["receiver_action_param"].asString());
    auto target_para = std::string((char *)target_param_vec.data(), target_param_vec.size());
    set_target(request["receiver_account"].asString(), request["receiver_action_name"].asString(), target_para);

    set_digest();
    auto signature = hex_to_uint(request["authorization"].asString());
    std::string signature_str((char *)signature.data(), signature.size());  // hex_to_uint client send xstream_t data 0xaaaa => string
    set_authorization(std::move(signature_str));

    set_len();
}

int32_t xtransaction_v2_t::parse(enum_xaction_type source_type, enum_xaction_type target_type, xtx_parse_data_t & tx_parse_data) {
#ifdef ENABLE_CREATE_USER  // debug use
    if (target_type == xaction_type_create_user_account) {
        tx_parse_data.m_new_account = m_source_addr.to_string();
    }
#endif

    if (source_type == xaction_type_asset_out) {
        std::string token_name = m_token_name;
        uint64_t amount = m_amount;
        if (get_tx_type() != xtransaction_type_transfer) {
            try {
                if (m_source_action_para.empty()) {
                    return xchain_error_action_param_empty;
                }
                xstream_t stream(xcontext_t::instance(), (uint8_t *)m_source_action_para.data(), m_source_action_para.size());
                stream >> token_name;
                stream >> amount;
            } catch (...) {
                return xchain_error_action_param_empty;
            }
        }
        tx_parse_data.m_asset = xproperty_asset(token_name, amount);
        if (!tx_parse_data.m_asset.is_tcash_token()) {
            tx_parse_data.m_amount_256 = amount;
        }
    }

    if (target_type == xaction_type_run_contract) {
        tx_parse_data.m_function_name = m_target_action_name;
        tx_parse_data.m_function_para = m_target_action_para;
    }

    try {
        if (target_type == xaction_type_pledge_token) {
            if (m_target_action_para.empty()) {
                return xchain_error_action_param_empty;
            }
            base::xstream_t stream(base::xcontext_t::instance(), (uint8_t *)m_target_action_para.data(), m_target_action_para.size());
            stream >> tx_parse_data.m_asset.m_token_name;
            stream >> tx_parse_data.m_asset.m_amount;
        }

        if (target_type == xaction_type_redeem_token) {
            if (m_target_action_para.empty()) {
                return xchain_error_action_param_empty;
            }
            base::xstream_t stream(base::xcontext_t::instance(), (uint8_t *)m_target_action_para.data(), m_target_action_para.size());
            stream >> tx_parse_data.m_asset.m_token_name;
            stream >> tx_parse_data.m_asset.m_amount;
        }

        if (target_type == xaction_type_pledge_token_vote) {
            if (m_target_action_para.empty()) {
                return xchain_error_action_param_empty;
            }
            base::xstream_t stream(base::xcontext_t::instance(), (uint8_t *)m_target_action_para.data(), m_target_action_para.size());
            stream >> tx_parse_data.m_vote_num;
            stream >> tx_parse_data.m_lock_duration;
        }

        if (target_type == xaction_type_redeem_token_vote) {
            if (m_target_action_para.empty()) {
                return xchain_error_action_param_empty;
            }
            base::xstream_t stream(base::xcontext_t::instance(), (uint8_t *)m_target_action_para.data(), m_target_action_para.size());
            stream >> tx_parse_data.m_vote_num;
        }
    } catch (...) {
        return xchain_error_action_param_empty;
    }

    return 0;
}

size_t xtransaction_v2_t::get_object_size_real() const {
    size_t total_size = sizeof(*this);
    // add string member variable alloc size.
    xdbg("------cache size------xtransaction_v2_t this:%d,m_source_addr:%d,m_unadjusted_target_addr:%d,m_token_name:%d,m_memo:%d,m_authorization:%d,m_edge_nodeid:%d",
         sizeof(*this),
         get_size(m_source_addr.to_string()),
         get_size(target_address_.original().to_string()),
         get_size(m_token_name),
         get_size(m_memo),
         get_size(m_authorization),
         get_size(m_edge_nodeid));
    xdbg(
        "------cache size------ xtransaction_v2_t "
        "m_source_action_name:%d,m_source_action_para:%d,m_target_action_name:%d,m_target_action_para:%d,m_transaction_hash_str:%d,m_adjust_target_addr:%d",
        get_size(m_source_action_name),
        get_size(m_source_action_para),
        get_size(m_target_action_name),
        get_size(m_target_action_para),
        get_size(m_transaction_hash_str),
        get_size(target_address_.adjusted().to_string()));
    // m_source_action_para and m_target_action_para might use same memory, here might be calculate twice!!!!

    total_size += get_size(m_source_addr.to_string()) + get_size(target_address_.original().to_string()) + get_size(m_token_name) + get_size(m_memo) + get_size(m_ext) + get_size(m_authorization) +
                  get_size(m_edge_nodeid) + get_size(m_source_action_name) + get_size(m_source_action_para) + get_size(m_target_action_name) + get_size(m_target_action_para) +
                  get_size(m_transaction_hash_str) + get_size(target_address_.adjusted().to_string());
    xdbg("------cache size------ xtransaction_v2_t total_size:%zu", total_size);
    return total_size;
}

// new APIs

void xtransaction_v2_t::source_address(common::xaccount_address_t src_addr) {
    m_source_addr = std::move(src_addr);
}

common::xaccount_address_t const & xtransaction_v2_t::source_address() const noexcept {
    return m_source_addr;
}

void xtransaction_v2_t::target_address(common::xaccount_address_t dst_addr) {
    target_address_ = xtarget_address_t::build_from(m_source_addr, std::move(dst_addr));
}

common::xaccount_address_t const & xtransaction_v2_t::target_address() const noexcept {
    return target_address_.address();
}

common::xaccount_address_t const & xtransaction_v2_t::target_address_unadjusted() const noexcept {
    return target_address_.original();
}

}  // namespace data
}  // namespace tcash
