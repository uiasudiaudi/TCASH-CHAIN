#pragma once

#include "xbase/xcxx_config.h"

#include <json/json.h>
#if defined(XCXX20)
#include "src/ubuntu/xrpc.grpc.pb.h"
#else
#include "src/centos/xrpc.grpc.pb.h"
#endif

#include <atomic>
#include <string>
#include <thread>
#include <deque>
#include <condition_variable>

using grpc::Status;
using grpc::ServerContext;
using grpc::ServerWriter;

namespace tcash { namespace rpc {

extern std::atomic_int rpc_client_num;
extern std::atomic_int rpc_version;
extern std::deque<Json::Value> tableblock_data;
extern std::mutex tableblock_mtx;
extern std::condition_variable tableblock_cv;

enum enum_xrpc_version {
    xrpc_version_1 = 0,
    xrpc_version_2 = 2
};

class xrpc_handle_face_t
{
public:
    xrpc_handle_face_t() = default;
    xrpc_handle_face_t(xrpc_handle_face_t const &) = delete;
    xrpc_handle_face_t & operator=(xrpc_handle_face_t const &) = delete;
    xrpc_handle_face_t(xrpc_handle_face_t &&) = default;
    xrpc_handle_face_t & operator=(xrpc_handle_face_t &&) = delete;
    virtual ~xrpc_handle_face_t() = default;

    virtual bool handle(std::string & request, Json::Value& js_req, Json::Value& js_rsp, std::string & str_result, uint32_t & error_code) = 0;
};

class xgrpc_service
{
public:
    xgrpc_service(const std::string & host, const uint16_t port):m_address(host+":"+std::to_string(port))
    {
    }
    virtual ~xgrpc_service()
    {
    }
    void register_handle(const std::shared_ptr<xrpc_handle_face_t>& handle){
        m_handle = handle;
    }
    int32_t start();
private:
    int32_t run();
    uint32_t sync_run(const std::shared_ptr<xrpc_handle_face_t>& handle);
    uint32_t async_run();

    std::thread m_sync_thread;
    std::string m_address;
    std::shared_ptr<xrpc_handle_face_t> m_handle;
};

class xrpc_serviceimpl final : public tcash::xrpc_service::Service {

public:
    void register_handle(const std::shared_ptr<xrpc_handle_face_t>& handle);

private:
    Status call(ServerContext *context, const xrpc_request *request,
                xrpc_reply *reply) override;

    Status table_stream(ServerContext *context, const xrpc_request *request,
                ServerWriter<xrpc_reply> *replys) override;

    std::shared_ptr<xrpc_handle_face_t> m_handle;
    mutable std::mutex m_call_mtx;
};

}}


