// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: config/Cfg_Status.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_config_2fCfg_5fStatus_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_config_2fCfg_5fStatus_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3011000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3011002 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include "common/AttribChangeDataProto.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_config_2fCfg_5fStatus_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_config_2fCfg_5fStatus_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[2]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_config_2fCfg_5fStatus_2eproto;
class Cfg_Status;
class Cfg_StatusDefaultTypeInternal;
extern Cfg_StatusDefaultTypeInternal _Cfg_Status_default_instance_;
class Cfg_Status_Row;
class Cfg_Status_RowDefaultTypeInternal;
extern Cfg_Status_RowDefaultTypeInternal _Cfg_Status_Row_default_instance_;
PROTOBUF_NAMESPACE_OPEN
template<> ::Cfg_Status* Arena::CreateMaybeMessage<::Cfg_Status>(Arena*);
template<> ::Cfg_Status_Row* Arena::CreateMaybeMessage<::Cfg_Status_Row>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

// ===================================================================

class Cfg_Status_Row :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Cfg_Status.Row) */ {
 public:
  Cfg_Status_Row();
  virtual ~Cfg_Status_Row();

  Cfg_Status_Row(const Cfg_Status_Row& from);
  Cfg_Status_Row(Cfg_Status_Row&& from) noexcept
    : Cfg_Status_Row() {
    *this = ::std::move(from);
  }

  inline Cfg_Status_Row& operator=(const Cfg_Status_Row& from) {
    CopyFrom(from);
    return *this;
  }
  inline Cfg_Status_Row& operator=(Cfg_Status_Row&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const Cfg_Status_Row& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Cfg_Status_Row* internal_default_instance() {
    return reinterpret_cast<const Cfg_Status_Row*>(
               &_Cfg_Status_Row_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Cfg_Status_Row& a, Cfg_Status_Row& b) {
    a.Swap(&b);
  }
  inline void Swap(Cfg_Status_Row* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Cfg_Status_Row* New() const final {
    return CreateMaybeMessage<Cfg_Status_Row>(nullptr);
  }

  Cfg_Status_Row* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Cfg_Status_Row>(arena);
  }
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Cfg_Status_Row* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Cfg_Status.Row";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_config_2fCfg_5fStatus_2eproto);
    return ::descriptor_table_config_2fCfg_5fStatus_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kAttribChangeListFieldNumber = 12,
    kIdFieldNumber = 1,
    kLevelFieldNumber = 2,
    kStatusTypeFieldNumber = 3,
    kStatusExpireTypeFieldNumber = 4,
    kStatusFlagFieldNumber = 5,
    kPowerFieldNumber = 6,
    kSecsFieldNumber = 7,
    kTimesFieldNumber = 8,
    kMaxTimesFieldNumber = 9,
    kMaxSecsFieldNumber = 10,
    kScriptidFieldNumber = 11,
  };
  // repeated .AttribChangeDataProto attrib_change_list = 12;
  int attrib_change_list_size() const;
  private:
  int _internal_attrib_change_list_size() const;
  public:
  void clear_attrib_change_list();
  ::AttribChangeDataProto* mutable_attrib_change_list(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::AttribChangeDataProto >*
      mutable_attrib_change_list();
  private:
  const ::AttribChangeDataProto& _internal_attrib_change_list(int index) const;
  ::AttribChangeDataProto* _internal_add_attrib_change_list();
  public:
  const ::AttribChangeDataProto& attrib_change_list(int index) const;
  ::AttribChangeDataProto* add_attrib_change_list();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::AttribChangeDataProto >&
      attrib_change_list() const;

  // uint32 id = 1;
  void clear_id();
  ::PROTOBUF_NAMESPACE_ID::uint32 id() const;
  void set_id(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_id() const;
  void _internal_set_id(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 level = 2;
  void clear_level();
  ::PROTOBUF_NAMESPACE_ID::uint32 level() const;
  void set_level(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_level() const;
  void _internal_set_level(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 status_type = 3;
  void clear_status_type();
  ::PROTOBUF_NAMESPACE_ID::uint32 status_type() const;
  void set_status_type(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_status_type() const;
  void _internal_set_status_type(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 status_expire_type = 4;
  void clear_status_expire_type();
  ::PROTOBUF_NAMESPACE_ID::uint32 status_expire_type() const;
  void set_status_expire_type(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_status_expire_type() const;
  void _internal_set_status_expire_type(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 status_flag = 5;
  void clear_status_flag();
  ::PROTOBUF_NAMESPACE_ID::uint32 status_flag() const;
  void set_status_flag(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_status_flag() const;
  void _internal_set_status_flag(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 power = 6;
  void clear_power();
  ::PROTOBUF_NAMESPACE_ID::uint32 power() const;
  void set_power(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_power() const;
  void _internal_set_power(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // int32 secs = 7;
  void clear_secs();
  ::PROTOBUF_NAMESPACE_ID::int32 secs() const;
  void set_secs(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_secs() const;
  void _internal_set_secs(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // int32 times = 8;
  void clear_times();
  ::PROTOBUF_NAMESPACE_ID::int32 times() const;
  void set_times(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_times() const;
  void _internal_set_times(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // uint32 max_times = 9;
  void clear_max_times();
  ::PROTOBUF_NAMESPACE_ID::uint32 max_times() const;
  void set_max_times(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_max_times() const;
  void _internal_set_max_times(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 max_secs = 10;
  void clear_max_secs();
  ::PROTOBUF_NAMESPACE_ID::uint32 max_secs() const;
  void set_max_secs(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_max_secs() const;
  void _internal_set_max_secs(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // int64 scriptid = 11;
  void clear_scriptid();
  ::PROTOBUF_NAMESPACE_ID::int64 scriptid() const;
  void set_scriptid(::PROTOBUF_NAMESPACE_ID::int64 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int64 _internal_scriptid() const;
  void _internal_set_scriptid(::PROTOBUF_NAMESPACE_ID::int64 value);
  public:

  // @@protoc_insertion_point(class_scope:Cfg_Status.Row)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::AttribChangeDataProto > attrib_change_list_;
  ::PROTOBUF_NAMESPACE_ID::uint32 id_;
  ::PROTOBUF_NAMESPACE_ID::uint32 level_;
  ::PROTOBUF_NAMESPACE_ID::uint32 status_type_;
  ::PROTOBUF_NAMESPACE_ID::uint32 status_expire_type_;
  ::PROTOBUF_NAMESPACE_ID::uint32 status_flag_;
  ::PROTOBUF_NAMESPACE_ID::uint32 power_;
  ::PROTOBUF_NAMESPACE_ID::int32 secs_;
  ::PROTOBUF_NAMESPACE_ID::int32 times_;
  ::PROTOBUF_NAMESPACE_ID::uint32 max_times_;
  ::PROTOBUF_NAMESPACE_ID::uint32 max_secs_;
  ::PROTOBUF_NAMESPACE_ID::int64 scriptid_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_config_2fCfg_5fStatus_2eproto;
};
// -------------------------------------------------------------------

class Cfg_Status :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Cfg_Status) */ {
 public:
  Cfg_Status();
  virtual ~Cfg_Status();

  Cfg_Status(const Cfg_Status& from);
  Cfg_Status(Cfg_Status&& from) noexcept
    : Cfg_Status() {
    *this = ::std::move(from);
  }

  inline Cfg_Status& operator=(const Cfg_Status& from) {
    CopyFrom(from);
    return *this;
  }
  inline Cfg_Status& operator=(Cfg_Status&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const Cfg_Status& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Cfg_Status* internal_default_instance() {
    return reinterpret_cast<const Cfg_Status*>(
               &_Cfg_Status_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(Cfg_Status& a, Cfg_Status& b) {
    a.Swap(&b);
  }
  inline void Swap(Cfg_Status* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Cfg_Status* New() const final {
    return CreateMaybeMessage<Cfg_Status>(nullptr);
  }

  Cfg_Status* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Cfg_Status>(arena);
  }
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Cfg_Status* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Cfg_Status";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_config_2fCfg_5fStatus_2eproto);
    return ::descriptor_table_config_2fCfg_5fStatus_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  typedef Cfg_Status_Row Row;

  // accessors -------------------------------------------------------

  enum : int {
    kRowsFieldNumber = 1,
  };
  // repeated .Cfg_Status.Row rows = 1;
  int rows_size() const;
  private:
  int _internal_rows_size() const;
  public:
  void clear_rows();
  ::Cfg_Status_Row* mutable_rows(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Status_Row >*
      mutable_rows();
  private:
  const ::Cfg_Status_Row& _internal_rows(int index) const;
  ::Cfg_Status_Row* _internal_add_rows();
  public:
  const ::Cfg_Status_Row& rows(int index) const;
  ::Cfg_Status_Row* add_rows();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Status_Row >&
      rows() const;

  // @@protoc_insertion_point(class_scope:Cfg_Status)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Status_Row > rows_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_config_2fCfg_5fStatus_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Cfg_Status_Row

// uint32 id = 1;
inline void Cfg_Status_Row::clear_id() {
  id_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Status_Row::_internal_id() const {
  return id_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Status_Row::id() const {
  // @@protoc_insertion_point(field_get:Cfg_Status.Row.id)
  return _internal_id();
}
inline void Cfg_Status_Row::_internal_set_id(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  id_ = value;
}
inline void Cfg_Status_Row::set_id(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_id(value);
  // @@protoc_insertion_point(field_set:Cfg_Status.Row.id)
}

// uint32 level = 2;
inline void Cfg_Status_Row::clear_level() {
  level_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Status_Row::_internal_level() const {
  return level_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Status_Row::level() const {
  // @@protoc_insertion_point(field_get:Cfg_Status.Row.level)
  return _internal_level();
}
inline void Cfg_Status_Row::_internal_set_level(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  level_ = value;
}
inline void Cfg_Status_Row::set_level(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_level(value);
  // @@protoc_insertion_point(field_set:Cfg_Status.Row.level)
}

// uint32 status_type = 3;
inline void Cfg_Status_Row::clear_status_type() {
  status_type_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Status_Row::_internal_status_type() const {
  return status_type_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Status_Row::status_type() const {
  // @@protoc_insertion_point(field_get:Cfg_Status.Row.status_type)
  return _internal_status_type();
}
inline void Cfg_Status_Row::_internal_set_status_type(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  status_type_ = value;
}
inline void Cfg_Status_Row::set_status_type(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_status_type(value);
  // @@protoc_insertion_point(field_set:Cfg_Status.Row.status_type)
}

// uint32 status_expire_type = 4;
inline void Cfg_Status_Row::clear_status_expire_type() {
  status_expire_type_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Status_Row::_internal_status_expire_type() const {
  return status_expire_type_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Status_Row::status_expire_type() const {
  // @@protoc_insertion_point(field_get:Cfg_Status.Row.status_expire_type)
  return _internal_status_expire_type();
}
inline void Cfg_Status_Row::_internal_set_status_expire_type(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  status_expire_type_ = value;
}
inline void Cfg_Status_Row::set_status_expire_type(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_status_expire_type(value);
  // @@protoc_insertion_point(field_set:Cfg_Status.Row.status_expire_type)
}

// uint32 status_flag = 5;
inline void Cfg_Status_Row::clear_status_flag() {
  status_flag_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Status_Row::_internal_status_flag() const {
  return status_flag_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Status_Row::status_flag() const {
  // @@protoc_insertion_point(field_get:Cfg_Status.Row.status_flag)
  return _internal_status_flag();
}
inline void Cfg_Status_Row::_internal_set_status_flag(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  status_flag_ = value;
}
inline void Cfg_Status_Row::set_status_flag(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_status_flag(value);
  // @@protoc_insertion_point(field_set:Cfg_Status.Row.status_flag)
}

// uint32 power = 6;
inline void Cfg_Status_Row::clear_power() {
  power_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Status_Row::_internal_power() const {
  return power_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Status_Row::power() const {
  // @@protoc_insertion_point(field_get:Cfg_Status.Row.power)
  return _internal_power();
}
inline void Cfg_Status_Row::_internal_set_power(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  power_ = value;
}
inline void Cfg_Status_Row::set_power(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_power(value);
  // @@protoc_insertion_point(field_set:Cfg_Status.Row.power)
}

// int32 secs = 7;
inline void Cfg_Status_Row::clear_secs() {
  secs_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 Cfg_Status_Row::_internal_secs() const {
  return secs_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 Cfg_Status_Row::secs() const {
  // @@protoc_insertion_point(field_get:Cfg_Status.Row.secs)
  return _internal_secs();
}
inline void Cfg_Status_Row::_internal_set_secs(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  secs_ = value;
}
inline void Cfg_Status_Row::set_secs(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_secs(value);
  // @@protoc_insertion_point(field_set:Cfg_Status.Row.secs)
}

// int32 times = 8;
inline void Cfg_Status_Row::clear_times() {
  times_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 Cfg_Status_Row::_internal_times() const {
  return times_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 Cfg_Status_Row::times() const {
  // @@protoc_insertion_point(field_get:Cfg_Status.Row.times)
  return _internal_times();
}
inline void Cfg_Status_Row::_internal_set_times(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  times_ = value;
}
inline void Cfg_Status_Row::set_times(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_times(value);
  // @@protoc_insertion_point(field_set:Cfg_Status.Row.times)
}

// uint32 max_times = 9;
inline void Cfg_Status_Row::clear_max_times() {
  max_times_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Status_Row::_internal_max_times() const {
  return max_times_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Status_Row::max_times() const {
  // @@protoc_insertion_point(field_get:Cfg_Status.Row.max_times)
  return _internal_max_times();
}
inline void Cfg_Status_Row::_internal_set_max_times(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  max_times_ = value;
}
inline void Cfg_Status_Row::set_max_times(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_max_times(value);
  // @@protoc_insertion_point(field_set:Cfg_Status.Row.max_times)
}

// uint32 max_secs = 10;
inline void Cfg_Status_Row::clear_max_secs() {
  max_secs_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Status_Row::_internal_max_secs() const {
  return max_secs_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Status_Row::max_secs() const {
  // @@protoc_insertion_point(field_get:Cfg_Status.Row.max_secs)
  return _internal_max_secs();
}
inline void Cfg_Status_Row::_internal_set_max_secs(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  max_secs_ = value;
}
inline void Cfg_Status_Row::set_max_secs(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_max_secs(value);
  // @@protoc_insertion_point(field_set:Cfg_Status.Row.max_secs)
}

// int64 scriptid = 11;
inline void Cfg_Status_Row::clear_scriptid() {
  scriptid_ = PROTOBUF_LONGLONG(0);
}
inline ::PROTOBUF_NAMESPACE_ID::int64 Cfg_Status_Row::_internal_scriptid() const {
  return scriptid_;
}
inline ::PROTOBUF_NAMESPACE_ID::int64 Cfg_Status_Row::scriptid() const {
  // @@protoc_insertion_point(field_get:Cfg_Status.Row.scriptid)
  return _internal_scriptid();
}
inline void Cfg_Status_Row::_internal_set_scriptid(::PROTOBUF_NAMESPACE_ID::int64 value) {
  
  scriptid_ = value;
}
inline void Cfg_Status_Row::set_scriptid(::PROTOBUF_NAMESPACE_ID::int64 value) {
  _internal_set_scriptid(value);
  // @@protoc_insertion_point(field_set:Cfg_Status.Row.scriptid)
}

// repeated .AttribChangeDataProto attrib_change_list = 12;
inline int Cfg_Status_Row::_internal_attrib_change_list_size() const {
  return attrib_change_list_.size();
}
inline int Cfg_Status_Row::attrib_change_list_size() const {
  return _internal_attrib_change_list_size();
}
inline ::AttribChangeDataProto* Cfg_Status_Row::mutable_attrib_change_list(int index) {
  // @@protoc_insertion_point(field_mutable:Cfg_Status.Row.attrib_change_list)
  return attrib_change_list_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::AttribChangeDataProto >*
Cfg_Status_Row::mutable_attrib_change_list() {
  // @@protoc_insertion_point(field_mutable_list:Cfg_Status.Row.attrib_change_list)
  return &attrib_change_list_;
}
inline const ::AttribChangeDataProto& Cfg_Status_Row::_internal_attrib_change_list(int index) const {
  return attrib_change_list_.Get(index);
}
inline const ::AttribChangeDataProto& Cfg_Status_Row::attrib_change_list(int index) const {
  // @@protoc_insertion_point(field_get:Cfg_Status.Row.attrib_change_list)
  return _internal_attrib_change_list(index);
}
inline ::AttribChangeDataProto* Cfg_Status_Row::_internal_add_attrib_change_list() {
  return attrib_change_list_.Add();
}
inline ::AttribChangeDataProto* Cfg_Status_Row::add_attrib_change_list() {
  // @@protoc_insertion_point(field_add:Cfg_Status.Row.attrib_change_list)
  return _internal_add_attrib_change_list();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::AttribChangeDataProto >&
Cfg_Status_Row::attrib_change_list() const {
  // @@protoc_insertion_point(field_list:Cfg_Status.Row.attrib_change_list)
  return attrib_change_list_;
}

// -------------------------------------------------------------------

// Cfg_Status

// repeated .Cfg_Status.Row rows = 1;
inline int Cfg_Status::_internal_rows_size() const {
  return rows_.size();
}
inline int Cfg_Status::rows_size() const {
  return _internal_rows_size();
}
inline void Cfg_Status::clear_rows() {
  rows_.Clear();
}
inline ::Cfg_Status_Row* Cfg_Status::mutable_rows(int index) {
  // @@protoc_insertion_point(field_mutable:Cfg_Status.rows)
  return rows_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Status_Row >*
Cfg_Status::mutable_rows() {
  // @@protoc_insertion_point(field_mutable_list:Cfg_Status.rows)
  return &rows_;
}
inline const ::Cfg_Status_Row& Cfg_Status::_internal_rows(int index) const {
  return rows_.Get(index);
}
inline const ::Cfg_Status_Row& Cfg_Status::rows(int index) const {
  // @@protoc_insertion_point(field_get:Cfg_Status.rows)
  return _internal_rows(index);
}
inline ::Cfg_Status_Row* Cfg_Status::_internal_add_rows() {
  return rows_.Add();
}
inline ::Cfg_Status_Row* Cfg_Status::add_rows() {
  // @@protoc_insertion_point(field_add:Cfg_Status.rows)
  return _internal_add_rows();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Status_Row >&
Cfg_Status::rows() const {
  // @@protoc_insertion_point(field_list:Cfg_Status.rows)
  return rows_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_config_2fCfg_5fStatus_2eproto
