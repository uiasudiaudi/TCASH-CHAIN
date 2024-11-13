//
//  xbase_thread_message_handler.h
//
//  Created by Charlie Xie 01/23/2019.
//  Copyright (c) 2017-2019 Telos Foundation & contributors
//
#pragma once

#include "xbase/xcxx_config.h"
#include "xbase/xpacket.h"
#include "xbase/xthread.h"
#if defined(XCXX20)
#include "xtransport/proto/ubuntu/transport.pb.h"
#else
#include "xtransport/proto/centos/transport.pb.h"
#endif
#include "xtransport/transport.h"


#include <memory>
#include <mutex>
#include <vector>

namespace tcash {

namespace transport {

class MessageHandler;
class MultiThreadHandler;

struct QueueObject {
    QueueObject(std::shared_ptr<transport::protobuf::RoutingMessage> msg_ptr, std::shared_ptr<base::xpacket_t> pkt_ptr) : message_ptr(msg_ptr), packet_ptr(pkt_ptr) {
    }
    std::shared_ptr<transport::protobuf::RoutingMessage> message_ptr;
    std::shared_ptr<base::xpacket_t> packet_ptr;
};

class ThreadHandler : public base::xiobject_t {
public:
    explicit ThreadHandler(base::xiothread_t * raw_thread_ptr, const uint32_t raw_thread_index);
    virtual ~ThreadHandler();

private:
    ThreadHandler();
    ThreadHandler(const ThreadHandler &);
    ThreadHandler & operator=(const ThreadHandler &);

public:
    static bool fired_packet(base::xpacket_t & packet, int32_t cur_thread_id, uint64_t time_now_ms, on_receive_callback_t & callback_ptr);

public:
    base::xiothread_t * get_raw_thread() {
        return m_raw_thread;
    }

    // packet is from   send(xpacket_t & packet) or dispatch(xpacket_t & packet) of xdatabox_t
    // subclass need overwrite this virtual function if they need support signal(xpacket_t) or send(xpacket_t),only allow called internally
    virtual bool on_databox_open(base::xpacket_t & packet, int32_t cur_thread_id, uint64_t time_now_ms) override;

    void register_on_dispatch_callback(on_receive_callback_t callback);
    void unregister_on_dispatch_callback();

private:
    base::xiothread_t * m_raw_thread;
    uint32_t raw_thread_index_;
    std::mutex callback_mutex_;
    on_receive_callback_t callback_;
};

class MultiThreadHandler : public std::enable_shared_from_this<MultiThreadHandler> {
public:
    MultiThreadHandler();
    ~MultiThreadHandler();

    void Init();
    void Stcash();
    void HandleMessage(base::xpacket_t & packet);

    void register_on_dispatch_callback(on_receive_callback_t callback);
    void unregister_on_dispatch_callback();

private:
#ifdef __DIRECT_PASS_PACKET_WITHOUT_DATABOX__
    std::mutex m_mutex;
    on_receive_callback_t m_callback;
    size_t m_woker_threads_count{0};  // if need asynch message-handle,may create 1 threads
#else
    size_t m_woker_threads_count{2};  // 2 threads are enough to handle messages
#endif
    std::vector<ThreadHandler *> m_worker_threads;
};

}  // namespace transport

}  // namespace tcash
