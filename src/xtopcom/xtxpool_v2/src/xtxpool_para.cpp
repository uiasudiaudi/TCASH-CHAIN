// Copyright (c) 2017-2020 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xtxpool_v2/xtxpool_para.h"

#include "xconfig/xconfig_register.h"

NS_BEG2(tcash, xtxpool_v2)

#define TIMER_HEIGHT_PER_DAY (24 * 360)

xtxpool_resources::xtxpool_resources(const observer_ptr<base::xvblockstore_t> & blockstore,
                                     const observer_ptr<base::xvcertauth_t> & certauth,
                                     const observer_ptr<mbus::xmessage_bus_face_t> & bus)
  : m_blockstore(blockstore), m_certauth(certauth), m_bus(bus) {
}

xtxpool_resources::~xtxpool_resources() {
}

base::xvblockstore_t * xtxpool_resources::get_vblockstore() const {
    return m_blockstore.get();
}
base::xvcertauth_t * xtxpool_resources::get_certauth() const {
    return m_certauth.get();
}

mbus::xmessage_bus_face_t * xtxpool_resources::get_bus() const {
    return m_bus.get();
}

xreceiptid_state_cache_t & xtxpool_resources::get_receiptid_state_cache() {
    return m_receiptid_state_cache;
}

NS_END2
