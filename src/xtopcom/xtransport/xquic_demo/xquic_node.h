#pragma once
#include "cs_mini.h"
#include "xbasic/xbyte_buffer.h"
#include "xbasic/xrunnable.h"

#include <functional>
#include <memory>
#include <thread>
#include <unordered_map>

class xquic_node_t
  : public tcash::xbasic_runnable_t<xquic_node_t>
  , public std::enable_shared_from_this<xquic_node_t> {
public:
    xquic_node_t(std::size_t _server_port);

public:
    void start() override;

    void stcash() override;

public:
    bool send(std::string addr, uint32_t port, tcash::xbytes_t data);  // add ec;

    void on_quic_message_ready(tcash::xbytes_t const & bytes);

    void do_handle_quic_node_message();

private:
    /// configs:
    std::size_t m_server_port;

private:
    xquic_server_t m_server;
    xquic_client_t m_client;

    std::unordered_map<std::string, cli_user_conn_t *> m_conn_map;

#if defined DEBUG
    std::thread::id m_quic_node_thread_id{};

    std::thread::id m_quic_server_thread_id{};
#endif
};