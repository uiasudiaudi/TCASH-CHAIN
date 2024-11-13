// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xpbase/base/kad_key/kadmlia_key.h"

#include "xbase/xutl.h"
#include "xutility/xhash.h"
#include "xmetrics/xmetrics.h"

#include <cassert>
#include <ostream>
#include <sstream>

namespace tcash {

namespace base {
/**
 * old: [network_id:21]-[zone_id:7]-[cluster_id:7]-[group_id:8]-[height:21]
 * new:
 * [ver:1]-[network_id:20]-[zone_id:7]-[cluster_id:7]-[group_id:8]-[height:21]
 */
#define kRootNetworkId 0xFFFFFULL
#define set_kad_service_ver(service_id)                                   \
    ((service_id |= (0x1ULL) << 63))
#define set_kad_network_id(service_id, network_id)                             \
    ((service_id |= ((uint64_t)(network_id)&0x0FFFFFULL) << 43))
#define set_kad_zone_id(service_id, zone_id)                                   \
    ((service_id |= ((uint64_t)(zone_id)&0x7F) << 36))
#define set_kad_cluster_id(service_id, cluster_id)                             \
    ((service_id |= ((uint64_t)(cluster_id)&0x7F) << 29))
#define set_kad_group_id(service_id, group_id)                                 \
    ((service_id |= ((uint64_t)(group_id)&0xFF) << 21))
#define set_kad_height(service_id, height)                                     \
    ((service_id |= ((uint64_t)(height)&0x1FFFFFULL)))
// #define set_kad_root(service_id) ((service_id |= (0xFFFFFFULL)))

ServiceType::ServiceType(common::xip2_t const xip2) {
    set_kad_service_ver(m_type);
    set_kad_network_id(m_type, xip2.network_id().value());
    set_kad_zone_id(m_type, xip2.zone_id().value());
    set_kad_cluster_id(m_type, xip2.cluster_id().value());
    set_kad_group_id(m_type, xip2.group_id().value());
    set_kad_height(m_type, xip2.height());

    update_info();
}

ServiceType::ServiceType(common::xip2_t const xip2, uint64_t const height) {
    set_kad_service_ver(m_type);
    set_kad_network_id(m_type, xip2.network_id().value());
    set_kad_zone_id(m_type, xip2.zone_id().value());
    set_kad_cluster_id(m_type, xip2.cluster_id().value());
    set_kad_group_id(m_type, xip2.group_id().value());
    set_kad_height(m_type, height);
    update_info();
}

ServiceType ServiceType::build_from(uint64_t const type) {
    ServiceType service_type;
    service_type.m_type = type;
    service_type.update_info();
    return service_type;
}

ServiceType ServiceType::build_from(common::xip2_t const xip2) {
    return ServiceType(xip2);
}

ServiceType ServiceType::build_from(common::xip2_t const xip2, uint64_t const height) {
       return ServiceType(xip2, height);
}

void ServiceType::update_info() {
    // std::string info{""};
    // info += "[ver " + std::to_string((m_type >> 63)) + "]-";
    // info += "[network " + std::to_string((m_type << 1) >> (1 + 43)) + "]-";
    // info += "[zone " + std::to_string((m_type << 21) >> (21 + 36)) + "]-";
    // info += "[cluster " + std::to_string((m_type << 28) >> (28 + 29)) + "]-";
    // info += "[group " + std::to_string((m_type << 35) >> (35 + 21)) + "]-";
    // info += "[height " + std::to_string((m_type << 43) >> 43) + "]";
    // m_info = info;
    std::string buffer;
    buffer.reserve(100);
    std::ostringstream oss(buffer);

    oss << "[ver " << ((m_type >> 63)) << "]-"
        << "[network " << ((m_type << 1) >> (1 + 43)) << "]-"
        << "[zone " << ((m_type << 21) >> (21 + 36)) << "]-"
        << "[cluster " << ((m_type << 28) >> (28 + 29)) << "]-"
        << "[group " << ((m_type << 35) >> (35 + 21)) << "]-"
        << "[height " << ((m_type << 43) >> 43) << "]";

    m_info = oss.str();
}

#define IS_BROADCAST_HEIGHT(service_type_value)                                \
    ((service_type_value & 0x1FFFFFULL) == 0x1FFFFFULL)
#define BROADCAST_HEIGHT(service_type_value)                                   \
    ((service_type_value | 0x1FFFFFULL))

bool ServiceType::operator==(ServiceType const &other) const {
    if (IS_BROADCAST_HEIGHT(other.value()) || IS_BROADCAST_HEIGHT(m_type)) {
        return BROADCAST_HEIGHT(other.value()) == BROADCAST_HEIGHT(m_type);
    } else {
        return other.value() == m_type;
    }
}
bool ServiceType::operator!=(ServiceType const &other) const {
    return !(*this == other);
}
bool ServiceType::operator<(ServiceType const &other) const {
    return m_type < other.value();
}

bool ServiceType::IsNewer(ServiceType const &other) const {
    if (IS_BROADCAST_HEIGHT(other.value()) || IS_BROADCAST_HEIGHT(m_type))
        return false;
    if (BROADCAST_HEIGHT(other.value()) == BROADCAST_HEIGHT(m_type)) {
        if ((m_type & 0x1FFFFFULL) > ((other.value() & 0x1FFFFFULL)))
            return true;
    }
    return false;
}

bool ServiceType::IsBroadcastService() const {
    return IS_BROADCAST_HEIGHT(m_type);
}

uint64_t ServiceType::value() const { return m_type; }

service_type_ver ServiceType::ver() const {
    return static_cast<base::service_type_ver>(m_type >> 63);
}

common::xnetwork_id_t ServiceType::network_id() const {
    return common::xnetwork_id_t{
            static_cast<uint32_t>((m_type << 1) >> (1 + 43))};
}

common::xzone_id_t ServiceType::zone_id() const {
    return common::xzone_id_t{
            static_cast<uint8_t>((m_type << 21) >> (21 + 36))};
}
common::xcluster_id_t ServiceType::cluster_id() const {
    return common::xcluster_id_t{
            static_cast<uint8_t>((m_type << 28) >> (28 + 29))};
}
common::xgroup_id_t ServiceType::group_id() const {
    return common::xgroup_id_t{
            static_cast<uint8_t>((m_type << 35) >> (35 + 21))};
}
uint64_t ServiceType::height() const { return (m_type << 43) >> 43; }

//void ServiceType::set_ver(uint64_t new_ver) {
//    m_type &= ((m_type << 1) >> 1);
//    m_type |= ((uint64_t)(new_ver) << 63);
//    update_info();
//}

void ServiceType::set_height(uint64_t new_height) {
    m_type &= ((uint64_t)0xFFFFFFFFFFE00000ULL);
    m_type |= ((uint64_t)(new_height));
    update_info();
}

bool ServiceType::is_root_service() const {
    return (((m_type << 1) >> (43 + 1)) == kRootNetworkId);
}

common::xip2_t ServiceType::group_xip2() const {
    common::xip2_t res{network_id(), zone_id(), cluster_id(), group_id()};
    return res;
}

std::string const &  ServiceType::info() const noexcept{ return m_info; }

ServiceType CreateServiceType(common::xip2_t const &xip) {
    //uint64_t res{0};
    //set_kad_service_ver(res, static_cast<size_t>(now_service_type_ver));
    //set_kad_network_id(res, xip.network_id().value());
    //set_kad_zone_id(res, xip.zone_id().value());
    //set_kad_cluster_id(res, xip.cluster_id().value());
    //set_kad_group_id(res, xip.group_id().value());
    //set_kad_height(res, xip.height());
    return ServiceType::build_from(xip);
}

base::KadmliaKeyPtr GetKadmliaKey(common::xip2_t const &xip) {
    auto kad_key_ptr = std::make_shared<base::KadmliaKey>(xip);
    return kad_key_ptr;
}
base::KadmliaKeyPtr GetKadmliaKey(std::string const &node_id) {
    auto kad_key_ptr = std::make_shared<base::KadmliaKey>(node_id);
    return kad_key_ptr;
}

base::KadmliaKeyPtr GetRootKadmliaKey(std::string const &node_id) {
    tcash::utl::xsha2_256_t h;
    tcash::uint256_t v;
    h.reset();
    h.update(node_id);
    h.get_hash(v);
    XMETRICS_GAUGE(metrics::cpu_hash_256_GetRootKadmliaKey_calc, 1);
    std::string node_id_hash_32((char *)v.data(), v.size());
    uint64_t high, low;
    std::string _h = node_id_hash_32.substr(0, 8);
    std::string _l = node_id_hash_32.substr(8, 8);
    memcpy(&high, _h.c_str(), _h.size());
    memcpy(&low, _l.c_str(), _l.size());

    auto _xvip = xvip2_t();
    _xvip.low_addr = low;
    _xvip.high_addr = high;
    reset_network_id_to_xip2(_xvip);
    set_network_id_to_xip2(_xvip, kRootNetworkId);
    common::xip2_t xip(_xvip);
    assert(xip.network_id().value() == kRootNetworkId);
    // xdbg("[GetRootKadmliaKey] get root kad key: xip:%s node_id: %s",
    //      xip.to_string().c_str(), node_id.c_str());
    return GetKadmliaKey(xip);
}

KadmliaKey::KadmliaKey(common::xip2_t const &xip) : xip_(xip) {}

KadmliaKey::KadmliaKey(std::string const &from_str) {
    // xdbg("KadmliaKey from_str %s", from_str.c_str());
    assert(from_str.size() == 33 && from_str[16] == '.');
    auto low_str = from_str.substr(0, 16);
    auto high_str = from_str.substr(17, 32);
    std::istringstream low_sstr(low_str);
    std::istringstream high_sstr(high_str);
    uint64_t low_part, high_part;
    low_sstr >> std::hex >> low_part;
    high_sstr >> std::hex >> high_part;
    // xdbg("KadmliaKey from_str %s,%s", low_str.c_str(), high_str.c_str());
    xip_ = common::xip2_t{low_part, high_part};
}

std::string KadmliaKey::Get() { return xip_.to_string(); }

ServiceType KadmliaKey::GetServiceType() { return CreateServiceType(xip_); }

} // namespace base

} // namespace tcash