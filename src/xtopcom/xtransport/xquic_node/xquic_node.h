#pragma once
#include "xbase/xpacket.h"
#include "xbasic/xbyte_buffer.h"
#include "xbasic/xrunnable.h"
#if defined(XCXX20)
#include "xtransport/proto/ubuntu/transport.pb.h"
#else
#include "xtransport/proto/centos/transport.pb.h"
#endif
#include "xtransport/transport_fwd.h"

#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>

// fwd
class xquic_server_t;
class xquic_client_t;
class cli_user_conn_t;

NS_BEG3(tcash, transport, quic)

class xquic_node_t
  : public tcash::xbasic_runnable_t<xquic_node_t>
  , public std::enable_shared_from_this<xquic_node_t> {
public:
    xquic_node_t(std::size_t _p2p_inbound_port);

private:
    void check_cert_file();

public:
    void start() override;

    void stcash() override;

    void register_on_receive_callback(on_receive_callback_t cb);

    void unregister_on_receive_callback();

public:
    int send_data(std::string const & data, std::string const & addr, uint16_t port);

    void on_quic_message_ready(tcash::xbytes_t const & bytes, std::string const & peer_ip, std::size_t peer_inbound_port);

private:
    std::size_t m_p2p_inbound_port;
    std::size_t m_server_port;

    on_receive_callback_t m_cb;

    std::shared_ptr<xquic_server_t> m_server_ptr;
    std::shared_ptr<xquic_client_t> m_client_ptr;

    std::mutex m_conn_map_mutex;
    std::unordered_map<std::string, cli_user_conn_t *> m_conn_map;
};

NS_END3