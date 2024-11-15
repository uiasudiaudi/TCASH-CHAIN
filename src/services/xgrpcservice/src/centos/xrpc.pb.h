// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: xrpc.proto

#ifndef PROTOBUF_INCLUDED_xrpc_2eproto
#define PROTOBUF_INCLUDED_xrpc_2eproto

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3006001
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#define PROTOBUF_INTERNAL_EXPORT_protobuf_xrpc_2eproto 

namespace protobuf_xrpc_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[2];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
}  // namespace protobuf_xrpc_2eproto
namespace tcash {
class xrpc_reply;
class xrpc_replyDefaultTypeInternal;
extern xrpc_replyDefaultTypeInternal _xrpc_reply_default_instance_;
class xrpc_request;
class xrpc_requestDefaultTypeInternal;
extern xrpc_requestDefaultTypeInternal _xrpc_request_default_instance_;
}  // namespace tcash
namespace google {
namespace protobuf {
template<> ::tcash::xrpc_reply* Arena::CreateMaybeMessage<::tcash::xrpc_reply>(Arena*);
template<> ::tcash::xrpc_request* Arena::CreateMaybeMessage<::tcash::xrpc_request>(Arena*);
}  // namespace protobuf
}  // namespace google
namespace tcash {

// ===================================================================

class xrpc_request : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:tcash.xrpc_request) */ {
 public:
  xrpc_request();
  virtual ~xrpc_request();

  xrpc_request(const xrpc_request& from);

  inline xrpc_request& operator=(const xrpc_request& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  xrpc_request(xrpc_request&& from) noexcept
    : xrpc_request() {
    *this = ::std::move(from);
  }

  inline xrpc_request& operator=(xrpc_request&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const xrpc_request& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const xrpc_request* internal_default_instance() {
    return reinterpret_cast<const xrpc_request*>(
               &_xrpc_request_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(xrpc_request* other);
  friend void swap(xrpc_request& a, xrpc_request& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline xrpc_request* New() const final {
    return CreateMaybeMessage<xrpc_request>(NULL);
  }

  xrpc_request* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<xrpc_request>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const xrpc_request& from);
  void MergeFrom(const xrpc_request& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(xrpc_request* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string action = 1;
  void clear_action();
  static const int kActionFieldNumber = 1;
  const ::std::string& action() const;
  void set_action(const ::std::string& value);
  #if LANG_CXX11
  void set_action(::std::string&& value);
  #endif
  void set_action(const char* value);
  void set_action(const char* value, size_t size);
  ::std::string* mutable_action();
  ::std::string* release_action();
  void set_allocated_action(::std::string* action);

  // string body = 2;
  void clear_body();
  static const int kBodyFieldNumber = 2;
  const ::std::string& body() const;
  void set_body(const ::std::string& value);
  #if LANG_CXX11
  void set_body(::std::string&& value);
  #endif
  void set_body(const char* value);
  void set_body(const char* value, size_t size);
  ::std::string* mutable_body();
  ::std::string* release_body();
  void set_allocated_body(::std::string* body);

  // @@protoc_insertion_point(class_scope:tcash.xrpc_request)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr action_;
  ::google::protobuf::internal::ArenaStringPtr body_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_xrpc_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class xrpc_reply : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:tcash.xrpc_reply) */ {
 public:
  xrpc_reply();
  virtual ~xrpc_reply();

  xrpc_reply(const xrpc_reply& from);

  inline xrpc_reply& operator=(const xrpc_reply& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  xrpc_reply(xrpc_reply&& from) noexcept
    : xrpc_reply() {
    *this = ::std::move(from);
  }

  inline xrpc_reply& operator=(xrpc_reply&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const xrpc_reply& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const xrpc_reply* internal_default_instance() {
    return reinterpret_cast<const xrpc_reply*>(
               &_xrpc_reply_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  void Swap(xrpc_reply* other);
  friend void swap(xrpc_reply& a, xrpc_reply& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline xrpc_reply* New() const final {
    return CreateMaybeMessage<xrpc_reply>(NULL);
  }

  xrpc_reply* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<xrpc_reply>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const xrpc_reply& from);
  void MergeFrom(const xrpc_reply& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(xrpc_reply* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string result = 1;
  void clear_result();
  static const int kResultFieldNumber = 1;
  const ::std::string& result() const;
  void set_result(const ::std::string& value);
  #if LANG_CXX11
  void set_result(::std::string&& value);
  #endif
  void set_result(const char* value);
  void set_result(const char* value, size_t size);
  ::std::string* mutable_result();
  ::std::string* release_result();
  void set_allocated_result(::std::string* result);

  // string body = 2;
  void clear_body();
  static const int kBodyFieldNumber = 2;
  const ::std::string& body() const;
  void set_body(const ::std::string& value);
  #if LANG_CXX11
  void set_body(::std::string&& value);
  #endif
  void set_body(const char* value);
  void set_body(const char* value, size_t size);
  ::std::string* mutable_body();
  ::std::string* release_body();
  void set_allocated_body(::std::string* body);

  // @@protoc_insertion_point(class_scope:tcash.xrpc_reply)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr result_;
  ::google::protobuf::internal::ArenaStringPtr body_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_xrpc_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// xrpc_request

// string action = 1;
inline void xrpc_request::clear_action() {
  action_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& xrpc_request::action() const {
  // @@protoc_insertion_point(field_get:tcash.xrpc_request.action)
  return action_.GetNoArena();
}
inline void xrpc_request::set_action(const ::std::string& value) {
  
  action_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tcash.xrpc_request.action)
}
#if LANG_CXX11
inline void xrpc_request::set_action(::std::string&& value) {
  
  action_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:tcash.xrpc_request.action)
}
#endif
inline void xrpc_request::set_action(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  action_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tcash.xrpc_request.action)
}
inline void xrpc_request::set_action(const char* value, size_t size) {
  
  action_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tcash.xrpc_request.action)
}
inline ::std::string* xrpc_request::mutable_action() {
  
  // @@protoc_insertion_point(field_mutable:tcash.xrpc_request.action)
  return action_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* xrpc_request::release_action() {
  // @@protoc_insertion_point(field_release:tcash.xrpc_request.action)
  
  return action_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void xrpc_request::set_allocated_action(::std::string* action) {
  if (action != NULL) {
    
  } else {
    
  }
  action_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), action);
  // @@protoc_insertion_point(field_set_allocated:tcash.xrpc_request.action)
}

// string body = 2;
inline void xrpc_request::clear_body() {
  body_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& xrpc_request::body() const {
  // @@protoc_insertion_point(field_get:tcash.xrpc_request.body)
  return body_.GetNoArena();
}
inline void xrpc_request::set_body(const ::std::string& value) {
  
  body_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tcash.xrpc_request.body)
}
#if LANG_CXX11
inline void xrpc_request::set_body(::std::string&& value) {
  
  body_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:tcash.xrpc_request.body)
}
#endif
inline void xrpc_request::set_body(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  body_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tcash.xrpc_request.body)
}
inline void xrpc_request::set_body(const char* value, size_t size) {
  
  body_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tcash.xrpc_request.body)
}
inline ::std::string* xrpc_request::mutable_body() {
  
  // @@protoc_insertion_point(field_mutable:tcash.xrpc_request.body)
  return body_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* xrpc_request::release_body() {
  // @@protoc_insertion_point(field_release:tcash.xrpc_request.body)
  
  return body_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void xrpc_request::set_allocated_body(::std::string* body) {
  if (body != NULL) {
    
  } else {
    
  }
  body_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), body);
  // @@protoc_insertion_point(field_set_allocated:tcash.xrpc_request.body)
}

// -------------------------------------------------------------------

// xrpc_reply

// string result = 1;
inline void xrpc_reply::clear_result() {
  result_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& xrpc_reply::result() const {
  // @@protoc_insertion_point(field_get:tcash.xrpc_reply.result)
  return result_.GetNoArena();
}
inline void xrpc_reply::set_result(const ::std::string& value) {
  
  result_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tcash.xrpc_reply.result)
}
#if LANG_CXX11
inline void xrpc_reply::set_result(::std::string&& value) {
  
  result_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:tcash.xrpc_reply.result)
}
#endif
inline void xrpc_reply::set_result(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  result_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tcash.xrpc_reply.result)
}
inline void xrpc_reply::set_result(const char* value, size_t size) {
  
  result_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tcash.xrpc_reply.result)
}
inline ::std::string* xrpc_reply::mutable_result() {
  
  // @@protoc_insertion_point(field_mutable:tcash.xrpc_reply.result)
  return result_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* xrpc_reply::release_result() {
  // @@protoc_insertion_point(field_release:tcash.xrpc_reply.result)
  
  return result_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void xrpc_reply::set_allocated_result(::std::string* result) {
  if (result != NULL) {
    
  } else {
    
  }
  result_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), result);
  // @@protoc_insertion_point(field_set_allocated:tcash.xrpc_reply.result)
}

// string body = 2;
inline void xrpc_reply::clear_body() {
  body_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& xrpc_reply::body() const {
  // @@protoc_insertion_point(field_get:tcash.xrpc_reply.body)
  return body_.GetNoArena();
}
inline void xrpc_reply::set_body(const ::std::string& value) {
  
  body_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tcash.xrpc_reply.body)
}
#if LANG_CXX11
inline void xrpc_reply::set_body(::std::string&& value) {
  
  body_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:tcash.xrpc_reply.body)
}
#endif
inline void xrpc_reply::set_body(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  body_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tcash.xrpc_reply.body)
}
inline void xrpc_reply::set_body(const char* value, size_t size) {
  
  body_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tcash.xrpc_reply.body)
}
inline ::std::string* xrpc_reply::mutable_body() {
  
  // @@protoc_insertion_point(field_mutable:tcash.xrpc_reply.body)
  return body_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* xrpc_reply::release_body() {
  // @@protoc_insertion_point(field_release:tcash.xrpc_reply.body)
  
  return body_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void xrpc_reply::set_allocated_body(::std::string* body) {
  if (body != NULL) {
    
  } else {
    
  }
  body_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), body);
  // @@protoc_insertion_point(field_set_allocated:tcash.xrpc_reply.body)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace tcash

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_INCLUDED_xrpc_2eproto
