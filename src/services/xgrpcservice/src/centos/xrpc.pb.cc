// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: xrpc.proto

#include "xrpc.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// This is a temporary google only hack
#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
#include "third_party/protobuf/version.h"
#endif
// @@protoc_insertion_point(includes)

namespace tcash {
class xrpc_requestDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<xrpc_request>
      _instance;
} _xrpc_request_default_instance_;
class xrpc_replyDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<xrpc_reply>
      _instance;
} _xrpc_reply_default_instance_;
}  // namespace tcash
namespace protobuf_xrpc_2eproto {
static void InitDefaultsxrpc_request() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::tcash::_xrpc_request_default_instance_;
    new (ptr) ::tcash::xrpc_request();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::tcash::xrpc_request::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<0> scc_info_xrpc_request =
    {{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsxrpc_request}, {}};

static void InitDefaultsxrpc_reply() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::tcash::_xrpc_reply_default_instance_;
    new (ptr) ::tcash::xrpc_reply();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::tcash::xrpc_reply::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<0> scc_info_xrpc_reply =
    {{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsxrpc_reply}, {}};

void InitDefaults() {
  ::google::protobuf::internal::InitSCC(&scc_info_xrpc_request.base);
  ::google::protobuf::internal::InitSCC(&scc_info_xrpc_reply.base);
}

::google::protobuf::Metadata file_level_metadata[2];

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tcash::xrpc_request, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tcash::xrpc_request, action_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tcash::xrpc_request, body_),
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tcash::xrpc_reply, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tcash::xrpc_reply, result_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tcash::xrpc_reply, body_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::tcash::xrpc_request)},
  { 7, -1, sizeof(::tcash::xrpc_reply)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::tcash::_xrpc_request_default_instance_),
  reinterpret_cast<const ::google::protobuf::Message*>(&::tcash::_xrpc_reply_default_instance_),
};

void protobuf_AssignDescriptors() {
  AddDescriptors();
  AssignDescriptors(
      "xrpc.proto", schemas, file_default_instances, TableStruct::offsets,
      file_level_metadata, NULL, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static ::google::protobuf::internal::once_flag once;
  ::google::protobuf::internal::call_once(once, protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_PROTOBUF_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 2);
}

void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
      "\n\nxrpc.proto\022\003tcash\",\n\014xrpc_request\022\016\n\006act"
      "ion\030\001 \001(\t\022\014\n\004body\030\002 \001(\t\"*\n\nxrpc_reply\022\016\n"
      "\006result\030\001 \001(\t\022\014\n\004body\030\002 \001(\t2t\n\014xrpc_serv"
      "ice\022,\n\004call\022\021.tcash.xrpc_request\032\017.tcash.xrp"
      "c_reply\"\000\0226\n\014table_stream\022\021.tcash.xrpc_req"
      "uest\032\017.tcash.xrpc_reply\"\0000\001B6\n\033io.grpc.exa"
      "mples.helloworldB\017HelloWorldProtcash\001\242\002\003HL"
      "Wb\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 289);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "xrpc.proto", &protobuf_RegisterTypes);
}

void AddDescriptors() {
  static ::google::protobuf::internal::once_flag once;
  ::google::protobuf::internal::call_once(once, AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at dynamic initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;
}  // namespace protobuf_xrpc_2eproto
namespace tcash {

// ===================================================================

void xrpc_request::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int xrpc_request::kActionFieldNumber;
const int xrpc_request::kBodyFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

xrpc_request::xrpc_request()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  ::google::protobuf::internal::InitSCC(
      &protobuf_xrpc_2eproto::scc_info_xrpc_request.base);
  SharedCtor();
  // @@protoc_insertion_point(constructor:tcash.xrpc_request)
}
xrpc_request::xrpc_request(const xrpc_request& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  action_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.action().size() > 0) {
    action_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.action_);
  }
  body_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.body().size() > 0) {
    body_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.body_);
  }
  // @@protoc_insertion_point(copy_constructor:tcash.xrpc_request)
}

void xrpc_request::SharedCtor() {
  action_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  body_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

xrpc_request::~xrpc_request() {
  // @@protoc_insertion_point(destructor:tcash.xrpc_request)
  SharedDtor();
}

void xrpc_request::SharedDtor() {
  action_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  body_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

void xrpc_request::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const ::google::protobuf::Descriptor* xrpc_request::descriptor() {
  ::protobuf_xrpc_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_xrpc_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const xrpc_request& xrpc_request::default_instance() {
  ::google::protobuf::internal::InitSCC(&protobuf_xrpc_2eproto::scc_info_xrpc_request.base);
  return *internal_default_instance();
}


void xrpc_request::Clear() {
// @@protoc_insertion_point(message_clear_start:tcash.xrpc_request)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  action_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  body_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  _internal_metadata_.Clear();
}

bool xrpc_request::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:tcash.xrpc_request)
  for (;;) {
    ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // string action = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(10u /* 10 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_action()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->action().data(), static_cast<int>(this->action().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "tcash.xrpc_request.action"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string body = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(18u /* 18 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_body()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->body().data(), static_cast<int>(this->body().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "tcash.xrpc_request.body"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:tcash.xrpc_request)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:tcash.xrpc_request)
  return false;
#undef DO_
}

void xrpc_request::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:tcash.xrpc_request)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string action = 1;
  if (this->action().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->action().data(), static_cast<int>(this->action().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tcash.xrpc_request.action");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->action(), output);
  }

  // string body = 2;
  if (this->body().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->body().data(), static_cast<int>(this->body().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tcash.xrpc_request.body");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->body(), output);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:tcash.xrpc_request)
}

::google::protobuf::uint8* xrpc_request::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:tcash.xrpc_request)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string action = 1;
  if (this->action().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->action().data(), static_cast<int>(this->action().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tcash.xrpc_request.action");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->action(), target);
  }

  // string body = 2;
  if (this->body().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->body().data(), static_cast<int>(this->body().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tcash.xrpc_request.body");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->body(), target);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:tcash.xrpc_request)
  return target;
}

size_t xrpc_request::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:tcash.xrpc_request)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // string action = 1;
  if (this->action().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->action());
  }

  // string body = 2;
  if (this->body().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->body());
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void xrpc_request::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:tcash.xrpc_request)
  GOOGLE_DCHECK_NE(&from, this);
  const xrpc_request* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const xrpc_request>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:tcash.xrpc_request)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:tcash.xrpc_request)
    MergeFrom(*source);
  }
}

void xrpc_request::MergeFrom(const xrpc_request& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:tcash.xrpc_request)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.action().size() > 0) {

    action_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.action_);
  }
  if (from.body().size() > 0) {

    body_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.body_);
  }
}

void xrpc_request::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:tcash.xrpc_request)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void xrpc_request::CopyFrom(const xrpc_request& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:tcash.xrpc_request)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool xrpc_request::IsInitialized() const {
  return true;
}

void xrpc_request::Swap(xrpc_request* other) {
  if (other == this) return;
  InternalSwap(other);
}
void xrpc_request::InternalSwap(xrpc_request* other) {
  using std::swap;
  action_.Swap(&other->action_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  body_.Swap(&other->body_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  _internal_metadata_.Swap(&other->_internal_metadata_);
}

::google::protobuf::Metadata xrpc_request::GetMetadata() const {
  protobuf_xrpc_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_xrpc_2eproto::file_level_metadata[kIndexInFileMessages];
}


// ===================================================================

void xrpc_reply::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int xrpc_reply::kResultFieldNumber;
const int xrpc_reply::kBodyFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

xrpc_reply::xrpc_reply()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  ::google::protobuf::internal::InitSCC(
      &protobuf_xrpc_2eproto::scc_info_xrpc_reply.base);
  SharedCtor();
  // @@protoc_insertion_point(constructor:tcash.xrpc_reply)
}
xrpc_reply::xrpc_reply(const xrpc_reply& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  result_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.result().size() > 0) {
    result_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.result_);
  }
  body_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.body().size() > 0) {
    body_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.body_);
  }
  // @@protoc_insertion_point(copy_constructor:tcash.xrpc_reply)
}

void xrpc_reply::SharedCtor() {
  result_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  body_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

xrpc_reply::~xrpc_reply() {
  // @@protoc_insertion_point(destructor:tcash.xrpc_reply)
  SharedDtor();
}

void xrpc_reply::SharedDtor() {
  result_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  body_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

void xrpc_reply::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const ::google::protobuf::Descriptor* xrpc_reply::descriptor() {
  ::protobuf_xrpc_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_xrpc_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const xrpc_reply& xrpc_reply::default_instance() {
  ::google::protobuf::internal::InitSCC(&protobuf_xrpc_2eproto::scc_info_xrpc_reply.base);
  return *internal_default_instance();
}


void xrpc_reply::Clear() {
// @@protoc_insertion_point(message_clear_start:tcash.xrpc_reply)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  result_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  body_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  _internal_metadata_.Clear();
}

bool xrpc_reply::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:tcash.xrpc_reply)
  for (;;) {
    ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // string result = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(10u /* 10 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_result()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->result().data(), static_cast<int>(this->result().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "tcash.xrpc_reply.result"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string body = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(18u /* 18 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_body()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->body().data(), static_cast<int>(this->body().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "tcash.xrpc_reply.body"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:tcash.xrpc_reply)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:tcash.xrpc_reply)
  return false;
#undef DO_
}

void xrpc_reply::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:tcash.xrpc_reply)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string result = 1;
  if (this->result().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->result().data(), static_cast<int>(this->result().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tcash.xrpc_reply.result");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->result(), output);
  }

  // string body = 2;
  if (this->body().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->body().data(), static_cast<int>(this->body().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tcash.xrpc_reply.body");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->body(), output);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:tcash.xrpc_reply)
}

::google::protobuf::uint8* xrpc_reply::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:tcash.xrpc_reply)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string result = 1;
  if (this->result().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->result().data(), static_cast<int>(this->result().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tcash.xrpc_reply.result");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->result(), target);
  }

  // string body = 2;
  if (this->body().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->body().data(), static_cast<int>(this->body().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tcash.xrpc_reply.body");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->body(), target);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:tcash.xrpc_reply)
  return target;
}

size_t xrpc_reply::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:tcash.xrpc_reply)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // string result = 1;
  if (this->result().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->result());
  }

  // string body = 2;
  if (this->body().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->body());
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void xrpc_reply::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:tcash.xrpc_reply)
  GOOGLE_DCHECK_NE(&from, this);
  const xrpc_reply* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const xrpc_reply>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:tcash.xrpc_reply)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:tcash.xrpc_reply)
    MergeFrom(*source);
  }
}

void xrpc_reply::MergeFrom(const xrpc_reply& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:tcash.xrpc_reply)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.result().size() > 0) {

    result_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.result_);
  }
  if (from.body().size() > 0) {

    body_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.body_);
  }
}

void xrpc_reply::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:tcash.xrpc_reply)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void xrpc_reply::CopyFrom(const xrpc_reply& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:tcash.xrpc_reply)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool xrpc_reply::IsInitialized() const {
  return true;
}

void xrpc_reply::Swap(xrpc_reply* other) {
  if (other == this) return;
  InternalSwap(other);
}
void xrpc_reply::InternalSwap(xrpc_reply* other) {
  using std::swap;
  result_.Swap(&other->result_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  body_.Swap(&other->body_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  _internal_metadata_.Swap(&other->_internal_metadata_);
}

::google::protobuf::Metadata xrpc_reply::GetMetadata() const {
  protobuf_xrpc_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_xrpc_2eproto::file_level_metadata[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace tcash
namespace google {
namespace protobuf {
template<> GOOGLE_PROTOBUF_ATTRIBUTE_NOINLINE ::tcash::xrpc_request* Arena::CreateMaybeMessage< ::tcash::xrpc_request >(Arena* arena) {
  return Arena::CreateInternal< ::tcash::xrpc_request >(arena);
}
template<> GOOGLE_PROTOBUF_ATTRIBUTE_NOINLINE ::tcash::xrpc_reply* Arena::CreateMaybeMessage< ::tcash::xrpc_reply >(Arena* arena) {
  return Arena::CreateInternal< ::tcash::xrpc_reply >(arena);
}
}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)
