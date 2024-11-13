// Copyright (c) 2017-2020 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xdata.h"
#include "xbasic/xmemory.hpp"
#include "xchain_timer/xchain_timer_face.h"
#include "xdata/xcons_transaction.h"
#include "xdata/xtransaction.h"
#include "xmbus/xmessage_bus.h"
#include "xtxpool_v2/xtxpool_resources_face.h"

#include <atomic>
#include <string>

NS_BEG2(tcash, xtxpool_v2)

class xtxpool_resources : public xtxpool_resources_face {
public:
    xtxpool_resources(const observer_ptr<base::xvblockstore_t> & blockstore,
                      const observer_ptr<base::xvcertauth_t> & certauth,
                      const observer_ptr<mbus::xmessage_bus_face_t> & bus);

    virtual ~xtxpool_resources();

public:
    virtual base::xvblockstore_t * get_vblockstore() const override;
    virtual base::xvcertauth_t * get_certauth() const override;
    virtual mbus::xmessage_bus_face_t * get_bus() const override;
    virtual xreceiptid_state_cache_t & get_receiptid_state_cache() override;

private:
    observer_ptr<base::xvblockstore_t> m_blockstore;
    observer_ptr<base::xvcertauth_t> m_certauth;
    observer_ptr<mbus::xmessage_bus_face_t> m_bus;
    xreceiptid_state_cache_t m_receiptid_state_cache;
};

NS_END2
