// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: config/Cfg_SkillFAM.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_config_2fCfg_5fSkillFAM_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_config_2fCfg_5fSkillFAM_2eproto

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
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_config_2fCfg_5fSkillFAM_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_config_2fCfg_5fSkillFAM_2eproto {
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
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_config_2fCfg_5fSkillFAM_2eproto;
class Cfg_SkillFAM;
class Cfg_SkillFAMDefaultTypeInternal;
extern Cfg_SkillFAMDefaultTypeInternal _Cfg_SkillFAM_default_instance_;
class Cfg_SkillFAM_Row;
class Cfg_SkillFAM_RowDefaultTypeInternal;
extern Cfg_SkillFAM_RowDefaultTypeInternal _Cfg_SkillFAM_Row_default_instance_;
PROTOBUF_NAMESPACE_OPEN
template<> ::Cfg_SkillFAM* Arena::CreateMaybeMessage<::Cfg_SkillFAM>(Arena*);
template<> ::Cfg_SkillFAM_Row* Arena::CreateMaybeMessage<::Cfg_SkillFAM_Row>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

// ===================================================================

class Cfg_SkillFAM_Row :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Cfg_SkillFAM.Row) */ {
 public:
  Cfg_SkillFAM_Row();
  virtual ~Cfg_SkillFAM_Row();

  Cfg_SkillFAM_Row(const Cfg_SkillFAM_Row& from);
  Cfg_SkillFAM_Row(Cfg_SkillFAM_Row&& from) noexcept
    : Cfg_SkillFAM_Row() {
    *this = ::std::move(from);
  }

  inline Cfg_SkillFAM_Row& operator=(const Cfg_SkillFAM_Row& from) {
    CopyFrom(from);
    return *this;
  }
  inline Cfg_SkillFAM_Row& operator=(Cfg_SkillFAM_Row&& from) noexcept {
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
  static const Cfg_SkillFAM_Row& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Cfg_SkillFAM_Row* internal_default_instance() {
    return reinterpret_cast<const Cfg_SkillFAM_Row*>(
               &_Cfg_SkillFAM_Row_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Cfg_SkillFAM_Row& a, Cfg_SkillFAM_Row& b) {
    a.Swap(&b);
  }
  inline void Swap(Cfg_SkillFAM_Row* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Cfg_SkillFAM_Row* New() const final {
    return CreateMaybeMessage<Cfg_SkillFAM_Row>(nullptr);
  }

  Cfg_SkillFAM_Row* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Cfg_SkillFAM_Row>(arena);
  }
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Cfg_SkillFAM_Row* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Cfg_SkillFAM.Row";
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
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_config_2fCfg_5fSkillFAM_2eproto);
    return ::descriptor_table_config_2fCfg_5fSkillFAM_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kIdFieldNumber = 1,
    kDisFieldNumber = 2,
    kSelfHpFieldNumber = 3,
    kSelfMpFieldNumber = 4,
    kTargetHpFieldNumber = 5,
    kSkillDisFieldNumber = 6,
    kSkillPowFieldNumber = 7,
    kSkillUsetimeFieldNumber = 8,
    kSkillCdFieldNumber = 9,
    kSkillMpFieldNumber = 10,
    kOperTypeFieldNumber = 11,
    kLikeFieldNumber = 12,
  };
  // uint32 id = 1;
  void clear_id();
  ::PROTOBUF_NAMESPACE_ID::uint32 id() const;
  void set_id(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_id() const;
  void _internal_set_id(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 dis = 2;
  void clear_dis();
  ::PROTOBUF_NAMESPACE_ID::uint32 dis() const;
  void set_dis(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_dis() const;
  void _internal_set_dis(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 self_hp = 3;
  void clear_self_hp();
  ::PROTOBUF_NAMESPACE_ID::uint32 self_hp() const;
  void set_self_hp(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_self_hp() const;
  void _internal_set_self_hp(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 self_mp = 4;
  void clear_self_mp();
  ::PROTOBUF_NAMESPACE_ID::uint32 self_mp() const;
  void set_self_mp(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_self_mp() const;
  void _internal_set_self_mp(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 target_hp = 5;
  void clear_target_hp();
  ::PROTOBUF_NAMESPACE_ID::uint32 target_hp() const;
  void set_target_hp(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_target_hp() const;
  void _internal_set_target_hp(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 skill_dis = 6;
  void clear_skill_dis();
  ::PROTOBUF_NAMESPACE_ID::uint32 skill_dis() const;
  void set_skill_dis(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_skill_dis() const;
  void _internal_set_skill_dis(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 skill_pow = 7;
  void clear_skill_pow();
  ::PROTOBUF_NAMESPACE_ID::uint32 skill_pow() const;
  void set_skill_pow(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_skill_pow() const;
  void _internal_set_skill_pow(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 skill_usetime = 8;
  void clear_skill_usetime();
  ::PROTOBUF_NAMESPACE_ID::uint32 skill_usetime() const;
  void set_skill_usetime(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_skill_usetime() const;
  void _internal_set_skill_usetime(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 skill_cd = 9;
  void clear_skill_cd();
  ::PROTOBUF_NAMESPACE_ID::uint32 skill_cd() const;
  void set_skill_cd(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_skill_cd() const;
  void _internal_set_skill_cd(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 skill_mp = 10;
  void clear_skill_mp();
  ::PROTOBUF_NAMESPACE_ID::uint32 skill_mp() const;
  void set_skill_mp(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_skill_mp() const;
  void _internal_set_skill_mp(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 oper_type = 11;
  void clear_oper_type();
  ::PROTOBUF_NAMESPACE_ID::uint32 oper_type() const;
  void set_oper_type(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_oper_type() const;
  void _internal_set_oper_type(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 like = 12;
  void clear_like();
  ::PROTOBUF_NAMESPACE_ID::uint32 like() const;
  void set_like(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_like() const;
  void _internal_set_like(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // @@protoc_insertion_point(class_scope:Cfg_SkillFAM.Row)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::uint32 id_;
  ::PROTOBUF_NAMESPACE_ID::uint32 dis_;
  ::PROTOBUF_NAMESPACE_ID::uint32 self_hp_;
  ::PROTOBUF_NAMESPACE_ID::uint32 self_mp_;
  ::PROTOBUF_NAMESPACE_ID::uint32 target_hp_;
  ::PROTOBUF_NAMESPACE_ID::uint32 skill_dis_;
  ::PROTOBUF_NAMESPACE_ID::uint32 skill_pow_;
  ::PROTOBUF_NAMESPACE_ID::uint32 skill_usetime_;
  ::PROTOBUF_NAMESPACE_ID::uint32 skill_cd_;
  ::PROTOBUF_NAMESPACE_ID::uint32 skill_mp_;
  ::PROTOBUF_NAMESPACE_ID::uint32 oper_type_;
  ::PROTOBUF_NAMESPACE_ID::uint32 like_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_config_2fCfg_5fSkillFAM_2eproto;
};
// -------------------------------------------------------------------

class Cfg_SkillFAM :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Cfg_SkillFAM) */ {
 public:
  Cfg_SkillFAM();
  virtual ~Cfg_SkillFAM();

  Cfg_SkillFAM(const Cfg_SkillFAM& from);
  Cfg_SkillFAM(Cfg_SkillFAM&& from) noexcept
    : Cfg_SkillFAM() {
    *this = ::std::move(from);
  }

  inline Cfg_SkillFAM& operator=(const Cfg_SkillFAM& from) {
    CopyFrom(from);
    return *this;
  }
  inline Cfg_SkillFAM& operator=(Cfg_SkillFAM&& from) noexcept {
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
  static const Cfg_SkillFAM& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Cfg_SkillFAM* internal_default_instance() {
    return reinterpret_cast<const Cfg_SkillFAM*>(
               &_Cfg_SkillFAM_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(Cfg_SkillFAM& a, Cfg_SkillFAM& b) {
    a.Swap(&b);
  }
  inline void Swap(Cfg_SkillFAM* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Cfg_SkillFAM* New() const final {
    return CreateMaybeMessage<Cfg_SkillFAM>(nullptr);
  }

  Cfg_SkillFAM* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Cfg_SkillFAM>(arena);
  }
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Cfg_SkillFAM* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Cfg_SkillFAM";
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
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_config_2fCfg_5fSkillFAM_2eproto);
    return ::descriptor_table_config_2fCfg_5fSkillFAM_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  typedef Cfg_SkillFAM_Row Row;

  // accessors -------------------------------------------------------

  enum : int {
    kRowsFieldNumber = 1,
  };
  // repeated .Cfg_SkillFAM.Row rows = 1;
  int rows_size() const;
  private:
  int _internal_rows_size() const;
  public:
  void clear_rows();
  ::Cfg_SkillFAM_Row* mutable_rows(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_SkillFAM_Row >*
      mutable_rows();
  private:
  const ::Cfg_SkillFAM_Row& _internal_rows(int index) const;
  ::Cfg_SkillFAM_Row* _internal_add_rows();
  public:
  const ::Cfg_SkillFAM_Row& rows(int index) const;
  ::Cfg_SkillFAM_Row* add_rows();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_SkillFAM_Row >&
      rows() const;

  // @@protoc_insertion_point(class_scope:Cfg_SkillFAM)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_SkillFAM_Row > rows_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_config_2fCfg_5fSkillFAM_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Cfg_SkillFAM_Row

// uint32 id = 1;
inline void Cfg_SkillFAM_Row::clear_id() {
  id_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::_internal_id() const {
  return id_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::id() const {
  // @@protoc_insertion_point(field_get:Cfg_SkillFAM.Row.id)
  return _internal_id();
}
inline void Cfg_SkillFAM_Row::_internal_set_id(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  id_ = value;
}
inline void Cfg_SkillFAM_Row::set_id(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_id(value);
  // @@protoc_insertion_point(field_set:Cfg_SkillFAM.Row.id)
}

// uint32 dis = 2;
inline void Cfg_SkillFAM_Row::clear_dis() {
  dis_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::_internal_dis() const {
  return dis_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::dis() const {
  // @@protoc_insertion_point(field_get:Cfg_SkillFAM.Row.dis)
  return _internal_dis();
}
inline void Cfg_SkillFAM_Row::_internal_set_dis(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  dis_ = value;
}
inline void Cfg_SkillFAM_Row::set_dis(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_dis(value);
  // @@protoc_insertion_point(field_set:Cfg_SkillFAM.Row.dis)
}

// uint32 self_hp = 3;
inline void Cfg_SkillFAM_Row::clear_self_hp() {
  self_hp_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::_internal_self_hp() const {
  return self_hp_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::self_hp() const {
  // @@protoc_insertion_point(field_get:Cfg_SkillFAM.Row.self_hp)
  return _internal_self_hp();
}
inline void Cfg_SkillFAM_Row::_internal_set_self_hp(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  self_hp_ = value;
}
inline void Cfg_SkillFAM_Row::set_self_hp(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_self_hp(value);
  // @@protoc_insertion_point(field_set:Cfg_SkillFAM.Row.self_hp)
}

// uint32 self_mp = 4;
inline void Cfg_SkillFAM_Row::clear_self_mp() {
  self_mp_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::_internal_self_mp() const {
  return self_mp_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::self_mp() const {
  // @@protoc_insertion_point(field_get:Cfg_SkillFAM.Row.self_mp)
  return _internal_self_mp();
}
inline void Cfg_SkillFAM_Row::_internal_set_self_mp(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  self_mp_ = value;
}
inline void Cfg_SkillFAM_Row::set_self_mp(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_self_mp(value);
  // @@protoc_insertion_point(field_set:Cfg_SkillFAM.Row.self_mp)
}

// uint32 target_hp = 5;
inline void Cfg_SkillFAM_Row::clear_target_hp() {
  target_hp_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::_internal_target_hp() const {
  return target_hp_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::target_hp() const {
  // @@protoc_insertion_point(field_get:Cfg_SkillFAM.Row.target_hp)
  return _internal_target_hp();
}
inline void Cfg_SkillFAM_Row::_internal_set_target_hp(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  target_hp_ = value;
}
inline void Cfg_SkillFAM_Row::set_target_hp(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_target_hp(value);
  // @@protoc_insertion_point(field_set:Cfg_SkillFAM.Row.target_hp)
}

// uint32 skill_dis = 6;
inline void Cfg_SkillFAM_Row::clear_skill_dis() {
  skill_dis_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::_internal_skill_dis() const {
  return skill_dis_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::skill_dis() const {
  // @@protoc_insertion_point(field_get:Cfg_SkillFAM.Row.skill_dis)
  return _internal_skill_dis();
}
inline void Cfg_SkillFAM_Row::_internal_set_skill_dis(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  skill_dis_ = value;
}
inline void Cfg_SkillFAM_Row::set_skill_dis(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_skill_dis(value);
  // @@protoc_insertion_point(field_set:Cfg_SkillFAM.Row.skill_dis)
}

// uint32 skill_pow = 7;
inline void Cfg_SkillFAM_Row::clear_skill_pow() {
  skill_pow_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::_internal_skill_pow() const {
  return skill_pow_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::skill_pow() const {
  // @@protoc_insertion_point(field_get:Cfg_SkillFAM.Row.skill_pow)
  return _internal_skill_pow();
}
inline void Cfg_SkillFAM_Row::_internal_set_skill_pow(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  skill_pow_ = value;
}
inline void Cfg_SkillFAM_Row::set_skill_pow(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_skill_pow(value);
  // @@protoc_insertion_point(field_set:Cfg_SkillFAM.Row.skill_pow)
}

// uint32 skill_usetime = 8;
inline void Cfg_SkillFAM_Row::clear_skill_usetime() {
  skill_usetime_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::_internal_skill_usetime() const {
  return skill_usetime_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::skill_usetime() const {
  // @@protoc_insertion_point(field_get:Cfg_SkillFAM.Row.skill_usetime)
  return _internal_skill_usetime();
}
inline void Cfg_SkillFAM_Row::_internal_set_skill_usetime(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  skill_usetime_ = value;
}
inline void Cfg_SkillFAM_Row::set_skill_usetime(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_skill_usetime(value);
  // @@protoc_insertion_point(field_set:Cfg_SkillFAM.Row.skill_usetime)
}

// uint32 skill_cd = 9;
inline void Cfg_SkillFAM_Row::clear_skill_cd() {
  skill_cd_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::_internal_skill_cd() const {
  return skill_cd_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::skill_cd() const {
  // @@protoc_insertion_point(field_get:Cfg_SkillFAM.Row.skill_cd)
  return _internal_skill_cd();
}
inline void Cfg_SkillFAM_Row::_internal_set_skill_cd(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  skill_cd_ = value;
}
inline void Cfg_SkillFAM_Row::set_skill_cd(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_skill_cd(value);
  // @@protoc_insertion_point(field_set:Cfg_SkillFAM.Row.skill_cd)
}

// uint32 skill_mp = 10;
inline void Cfg_SkillFAM_Row::clear_skill_mp() {
  skill_mp_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::_internal_skill_mp() const {
  return skill_mp_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::skill_mp() const {
  // @@protoc_insertion_point(field_get:Cfg_SkillFAM.Row.skill_mp)
  return _internal_skill_mp();
}
inline void Cfg_SkillFAM_Row::_internal_set_skill_mp(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  skill_mp_ = value;
}
inline void Cfg_SkillFAM_Row::set_skill_mp(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_skill_mp(value);
  // @@protoc_insertion_point(field_set:Cfg_SkillFAM.Row.skill_mp)
}

// uint32 oper_type = 11;
inline void Cfg_SkillFAM_Row::clear_oper_type() {
  oper_type_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::_internal_oper_type() const {
  return oper_type_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::oper_type() const {
  // @@protoc_insertion_point(field_get:Cfg_SkillFAM.Row.oper_type)
  return _internal_oper_type();
}
inline void Cfg_SkillFAM_Row::_internal_set_oper_type(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  oper_type_ = value;
}
inline void Cfg_SkillFAM_Row::set_oper_type(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_oper_type(value);
  // @@protoc_insertion_point(field_set:Cfg_SkillFAM.Row.oper_type)
}

// uint32 like = 12;
inline void Cfg_SkillFAM_Row::clear_like() {
  like_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::_internal_like() const {
  return like_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_SkillFAM_Row::like() const {
  // @@protoc_insertion_point(field_get:Cfg_SkillFAM.Row.like)
  return _internal_like();
}
inline void Cfg_SkillFAM_Row::_internal_set_like(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  like_ = value;
}
inline void Cfg_SkillFAM_Row::set_like(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_like(value);
  // @@protoc_insertion_point(field_set:Cfg_SkillFAM.Row.like)
}

// -------------------------------------------------------------------

// Cfg_SkillFAM

// repeated .Cfg_SkillFAM.Row rows = 1;
inline int Cfg_SkillFAM::_internal_rows_size() const {
  return rows_.size();
}
inline int Cfg_SkillFAM::rows_size() const {
  return _internal_rows_size();
}
inline void Cfg_SkillFAM::clear_rows() {
  rows_.Clear();
}
inline ::Cfg_SkillFAM_Row* Cfg_SkillFAM::mutable_rows(int index) {
  // @@protoc_insertion_point(field_mutable:Cfg_SkillFAM.rows)
  return rows_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_SkillFAM_Row >*
Cfg_SkillFAM::mutable_rows() {
  // @@protoc_insertion_point(field_mutable_list:Cfg_SkillFAM.rows)
  return &rows_;
}
inline const ::Cfg_SkillFAM_Row& Cfg_SkillFAM::_internal_rows(int index) const {
  return rows_.Get(index);
}
inline const ::Cfg_SkillFAM_Row& Cfg_SkillFAM::rows(int index) const {
  // @@protoc_insertion_point(field_get:Cfg_SkillFAM.rows)
  return _internal_rows(index);
}
inline ::Cfg_SkillFAM_Row* Cfg_SkillFAM::_internal_add_rows() {
  return rows_.Add();
}
inline ::Cfg_SkillFAM_Row* Cfg_SkillFAM::add_rows() {
  // @@protoc_insertion_point(field_add:Cfg_SkillFAM.rows)
  return _internal_add_rows();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_SkillFAM_Row >&
Cfg_SkillFAM::rows() const {
  // @@protoc_insertion_point(field_list:Cfg_SkillFAM.rows)
  return rows_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_config_2fCfg_5fSkillFAM_2eproto
