// Copyright (c) 2017-2019 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "socket_intf.h"
#include "xbase/xdata.h"
#include "xbase/xlog.h"
#include "xbase/xobject.h"
#include "xbase/xpacket.h"
#include "xbase/xsocket.h"
#include "xbase/xthread.h"
#include "xbase/xtimer.h"
#include "xbase/xutl.h"
#include "xbasic/xbyte_buffer.h"
#include "xpbase/base/tcash_utils.h"
#include "xtransport/transport.h"
#include "xtransport/xquic_node/xquic_node.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>

// using namespace tcash;
// using namespace base;

namespace tcash {
namespace transport {

class MultiThreadHandler;

class UdpTransport
  : public Transport
  , public std::enable_shared_from_this<UdpTransport> {
public:
    UdpTransport();
    virtual ~UdpTransport() override;
    virtual bool Init(std::string const & local_ip, uint16_t local_port, MultiThreadHandler * message_handler) override;
    virtual int Start() override;
    virtual void Stcash() override;
    virtual int SendDataWithProp(std::string const & data, const std::string & peer_ip, uint16_t peer_port, UdpPropertyPtr & udp_property, uint16_t priority_flag = 0) override;

    virtual int32_t get_handle() override {
        return static_cast<int32_t>(udp_handle_);
    }
    virtual int get_socket_status() override;
    virtual std::string local_ip() override {
        return local_ip_;
    }
    virtual uint16_t local_port() override {
        return local_port_;
    }

    virtual void register_on_receive_callback(on_receive_callback_t callback) override;
    virtual void unregister_on_receive_callback() override;

    virtual int SendPing(const xbyte_buffer_t & data, const std::string & peer_ip, uint16_t peer_port) override;
    virtual int SendPing(base::xpacket_t & packet) override;
    virtual int RegisterOfflineCallback(std::function<void(const std::string & ip, const uint16_t port)> cb) override;
    virtual int RegisterNodeCallback(std::function<int32_t(std::string const & node_addr, std::string const & node_sign)> cb) override;
    virtual int CheckRatelimitMap(const std::string & to_addr) override;

private:
    void SetcashtBuffer();

private:
    base::xiothread_t * io_thread_;
    SocketIntf * udp_socket_;
    std::shared_ptr<quic::xquic_node_t> quic_node_{nullptr};
    std::string local_ip_;
    uint16_t local_port_;
    bool socket_connected_;
    xfd_handle_t udp_handle_;
    MultiThreadHandler * message_handler_;
    std::mutex restart_xbase_udp_server_mutex_;

    DISALLOW_COPY_AND_ASSIGN(UdpTransport);
};

typedef std::shared_ptr<UdpTransport> UdpTransportPtr;

}  // namespace transport
}  // namespace tcash
