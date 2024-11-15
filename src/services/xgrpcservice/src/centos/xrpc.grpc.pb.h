// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: xrpc.proto
#ifndef GRPC_xrpc_2eproto__INCLUDED
#define GRPC_xrpc_2eproto__INCLUDED

#include "xrpc.pb.h"

#include <grpcpp/impl/codegen/async_generic_service.h>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/impl/codegen/rpc_method.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/impl/codegen/stub_options.h>
#include <grpcpp/impl/codegen/sync_stream.h>

namespace grpc {
class CompletionQueue;
class Channel;
class ServerCompletionQueue;
class ServerContext;
}  // namespace grpc

namespace tcash {

class xrpc_service final {
 public:
  static constexpr char const* service_full_name() {
    return "tcash.xrpc_service";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status call(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::tcash::xrpc_reply* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::tcash::xrpc_reply>> Asynccall(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::tcash::xrpc_reply>>(AsynccallRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::tcash::xrpc_reply>> PrepareAsynccall(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::tcash::xrpc_reply>>(PrepareAsynccallRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientReaderInterface< ::tcash::xrpc_reply>> table_stream(::grpc::ClientContext* context, const ::tcash::xrpc_request& request) {
      return std::unique_ptr< ::grpc::ClientReaderInterface< ::tcash::xrpc_reply>>(table_streamRaw(context, request));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::tcash::xrpc_reply>> Asynctable_stream(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::tcash::xrpc_reply>>(Asynctable_streamRaw(context, request, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::tcash::xrpc_reply>> PrepareAsynctable_stream(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::tcash::xrpc_reply>>(PrepareAsynctable_streamRaw(context, request, cq));
    }
  private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::tcash::xrpc_reply>* AsynccallRaw(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::tcash::xrpc_reply>* PrepareAsynccallRaw(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientReaderInterface< ::tcash::xrpc_reply>* table_streamRaw(::grpc::ClientContext* context, const ::tcash::xrpc_request& request) = 0;
    virtual ::grpc::ClientAsyncReaderInterface< ::tcash::xrpc_reply>* Asynctable_streamRaw(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::grpc::CompletionQueue* cq, void* tag) = 0;
    virtual ::grpc::ClientAsyncReaderInterface< ::tcash::xrpc_reply>* PrepareAsynctable_streamRaw(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel);
    ::grpc::Status call(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::tcash::xrpc_reply* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::tcash::xrpc_reply>> Asynccall(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::tcash::xrpc_reply>>(AsynccallRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::tcash::xrpc_reply>> PrepareAsynccall(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::tcash::xrpc_reply>>(PrepareAsynccallRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientReader< ::tcash::xrpc_reply>> table_stream(::grpc::ClientContext* context, const ::tcash::xrpc_request& request) {
      return std::unique_ptr< ::grpc::ClientReader< ::tcash::xrpc_reply>>(table_streamRaw(context, request));
    }
    std::unique_ptr< ::grpc::ClientAsyncReader< ::tcash::xrpc_reply>> Asynctable_stream(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReader< ::tcash::xrpc_reply>>(Asynctable_streamRaw(context, request, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncReader< ::tcash::xrpc_reply>> PrepareAsynctable_stream(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReader< ::tcash::xrpc_reply>>(PrepareAsynctable_streamRaw(context, request, cq));
    }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    ::grpc::ClientAsyncResponseReader< ::tcash::xrpc_reply>* AsynccallRaw(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::tcash::xrpc_reply>* PrepareAsynccallRaw(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientReader< ::tcash::xrpc_reply>* table_streamRaw(::grpc::ClientContext* context, const ::tcash::xrpc_request& request) override;
    ::grpc::ClientAsyncReader< ::tcash::xrpc_reply>* Asynctable_streamRaw(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::grpc::CompletionQueue* cq, void* tag) override;
    ::grpc::ClientAsyncReader< ::tcash::xrpc_reply>* PrepareAsynctable_streamRaw(::grpc::ClientContext* context, const ::tcash::xrpc_request& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_call_;
    const ::grpc::internal::RpcMethod rpcmethod_table_stream_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status call(::grpc::ServerContext* context, const ::tcash::xrpc_request* request, ::tcash::xrpc_reply* response);
    virtual ::grpc::Status table_stream(::grpc::ServerContext* context, const ::tcash::xrpc_request* request, ::grpc::ServerWriter< ::tcash::xrpc_reply>* writer);
  };
  template <class BaseClass>
  class WithAsyncMethod_call : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_call() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_call() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status call(::grpc::ServerContext* context, const ::tcash::xrpc_request* request, ::tcash::xrpc_reply* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void Requestcall(::grpc::ServerContext* context, ::tcash::xrpc_request* request, ::grpc::ServerAsyncResponseWriter< ::tcash::xrpc_reply>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_table_stream : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_table_stream() {
      ::grpc::Service::MarkMethodAsync(1);
    }
    ~WithAsyncMethod_table_stream() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status table_stream(::grpc::ServerContext* context, const ::tcash::xrpc_request* request, ::grpc::ServerWriter< ::tcash::xrpc_reply>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void Requesttable_stream(::grpc::ServerContext* context, ::tcash::xrpc_request* request, ::grpc::ServerAsyncWriter< ::tcash::xrpc_reply>* writer, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncServerStreaming(1, context, request, writer, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_call<WithAsyncMethod_table_stream<Service > > AsyncService;
  template <class BaseClass>
  class WithGenericMethod_call : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_call() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_call() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status call(::grpc::ServerContext* context, const ::tcash::xrpc_request* request, ::tcash::xrpc_reply* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_table_stream : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_table_stream() {
      ::grpc::Service::MarkMethodGeneric(1);
    }
    ~WithGenericMethod_table_stream() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status table_stream(::grpc::ServerContext* context, const ::tcash::xrpc_request* request, ::grpc::ServerWriter< ::tcash::xrpc_reply>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_call : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithRawMethod_call() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_call() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status call(::grpc::ServerContext* context, const ::tcash::xrpc_request* request, ::tcash::xrpc_reply* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void Requestcall(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawMethod_table_stream : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithRawMethod_table_stream() {
      ::grpc::Service::MarkMethodRaw(1);
    }
    ~WithRawMethod_table_stream() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status table_stream(::grpc::ServerContext* context, const ::tcash::xrpc_request* request, ::grpc::ServerWriter< ::tcash::xrpc_reply>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void Requesttable_stream(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncWriter< ::grpc::ByteBuffer>* writer, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncServerStreaming(1, context, request, writer, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_call : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithStreamedUnaryMethod_call() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler< ::tcash::xrpc_request, ::tcash::xrpc_reply>(std::bind(&WithStreamedUnaryMethod_call<BaseClass>::Streamedcall, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithStreamedUnaryMethod_call() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status call(::grpc::ServerContext* context, const ::tcash::xrpc_request* request, ::tcash::xrpc_reply* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status Streamedcall(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::tcash::xrpc_request,::tcash::xrpc_reply>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_call<Service > StreamedUnaryService;
  template <class BaseClass>
  class WithSplitStreamingMethod_table_stream : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithSplitStreamingMethod_table_stream() {
      ::grpc::Service::MarkMethodStreamed(1,
        new ::grpc::internal::SplitServerStreamingHandler< ::tcash::xrpc_request, ::tcash::xrpc_reply>(std::bind(&WithSplitStreamingMethod_table_stream<BaseClass>::Streamedtable_stream, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithSplitStreamingMethod_table_stream() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status table_stream(::grpc::ServerContext* context, const ::tcash::xrpc_request* request, ::grpc::ServerWriter< ::tcash::xrpc_reply>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with split streamed
    virtual ::grpc::Status Streamedtable_stream(::grpc::ServerContext* context, ::grpc::ServerSplitStreamer< ::tcash::xrpc_request,::tcash::xrpc_reply>* server_split_streamer) = 0;
  };
  typedef WithSplitStreamingMethod_table_stream<Service > SplitStreamedService;
  typedef WithStreamedUnaryMethod_call<WithSplitStreamingMethod_table_stream<Service > > StreamedService;
};

}  // namespace tcash


#endif  // GRPC_xrpc_2eproto__INCLUDED
