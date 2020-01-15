// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: common/Common.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_common_2fCommon_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_common_2fCommon_2eproto

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
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_common_2fCommon_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_common_2fCommon_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_common_2fCommon_2eproto;
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE

enum ACTOR_ATTRIB : int {
  ATTRIB_NONE = 0,
  ATTRIB_HP_MAX = 1,
  ATTRIB_MP_MAX = 2,
  ATTRIB_FP_MAX = 3,
  ATTRIB_NP_MAX = 4,
  ATTRIB_MOVESPD = 5,
  ATTRIB_MIN_ATK = 6,
  ATTRIB_MAX_ATK = 7,
  ATTRIB_MIN_DEF = 8,
  ATTRIB_MAX_DEF = 9,
  ATTRIB_MIN_MATK = 10,
  ATTRIB_MAX_MATK = 11,
  ATTRIB_MIN_MDEF = 12,
  ATTRIB_MAX_MDEF = 13,
  ATTRIB_HIT = 14,
  ATTRIB_DODGE = 15,
  ATTRIB_DAMAGE_ADJ = 16,
  ATTRIB_DAMAGE_REFLECT_RATE = 17,
  ATTRIB_DAMAGE_REFLECT_ADJ = 18,
  ATTRIB_EXP_ADJ = 19,
  ATTRIB_MAX = 20,
  ACTOR_ATTRIB_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  ACTOR_ATTRIB_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool ACTOR_ATTRIB_IsValid(int value);
constexpr ACTOR_ATTRIB ACTOR_ATTRIB_MIN = ATTRIB_NONE;
constexpr ACTOR_ATTRIB ACTOR_ATTRIB_MAX = ATTRIB_MAX;
constexpr int ACTOR_ATTRIB_ARRAYSIZE = ACTOR_ATTRIB_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ACTOR_ATTRIB_descriptor();
template<typename T>
inline const std::string& ACTOR_ATTRIB_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ACTOR_ATTRIB>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ACTOR_ATTRIB_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    ACTOR_ATTRIB_descriptor(), enum_t_value);
}
inline bool ACTOR_ATTRIB_Parse(
    const std::string& name, ACTOR_ATTRIB* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<ACTOR_ATTRIB>(
    ACTOR_ATTRIB_descriptor(), name, value);
}
enum ACTOR_PROPERTY : int {
  PROP_NONE = 0,
  PROP_EXP = 1,
  PROP_LEVEL = 2,
  PROP_HP = 3,
  PROP_MP = 4,
  PROP_FP = 5,
  PROP_NP = 6,
  PROP_MONEY = 10,
  PROP_MONEYBIND = 11,
  PROP_GOLD = 12,
  PROP_GOLDBIND = 13,
  PROP_PKVAL = 14,
  PROP_HONOR = 15,
  PROP_CAMP = 16,
  PROP_ACHIPOINT = 17,
  PROP_VIPLEV = 18,
  PROP_HP_MAX = 50,
  PROP_WEAPON_CHANGE = 51,
  PROP_ARMOR_CHANGE = 52,
  PROP_MOUNT_CHANGE = 53,
  ACTOR_PROPERTY_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  ACTOR_PROPERTY_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool ACTOR_PROPERTY_IsValid(int value);
constexpr ACTOR_PROPERTY ACTOR_PROPERTY_MIN = PROP_NONE;
constexpr ACTOR_PROPERTY ACTOR_PROPERTY_MAX = PROP_MOUNT_CHANGE;
constexpr int ACTOR_PROPERTY_ARRAYSIZE = ACTOR_PROPERTY_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ACTOR_PROPERTY_descriptor();
template<typename T>
inline const std::string& ACTOR_PROPERTY_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ACTOR_PROPERTY>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ACTOR_PROPERTY_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    ACTOR_PROPERTY_descriptor(), enum_t_value);
}
inline bool ACTOR_PROPERTY_Parse(
    const std::string& name, ACTOR_PROPERTY* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<ACTOR_PROPERTY>(
    ACTOR_PROPERTY_descriptor(), name, value);
}
enum MONEY_TYPE : int {
  MT_NONE = 0,
  MT_MONEY = 1,
  MT_MONEYBIND = 2,
  MT_GOLD = 3,
  MT_GOLDBIND = 4,
  MONEY_TYPE_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  MONEY_TYPE_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool MONEY_TYPE_IsValid(int value);
constexpr MONEY_TYPE MONEY_TYPE_MIN = MT_NONE;
constexpr MONEY_TYPE MONEY_TYPE_MAX = MT_GOLDBIND;
constexpr int MONEY_TYPE_ARRAYSIZE = MONEY_TYPE_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* MONEY_TYPE_descriptor();
template<typename T>
inline const std::string& MONEY_TYPE_Name(T enum_t_value) {
  static_assert(::std::is_same<T, MONEY_TYPE>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function MONEY_TYPE_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    MONEY_TYPE_descriptor(), enum_t_value);
}
inline bool MONEY_TYPE_Parse(
    const std::string& name, MONEY_TYPE* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<MONEY_TYPE>(
    MONEY_TYPE_descriptor(), name, value);
}
enum PK_MODE : int {
  PKMODE_PEACE = 0,
  PKMODE_REDNAME = 1,
  PKMODE_TEAM = 2,
  PKMODE_GUILD = 3,
  PKMODE_CAMP = 4,
  PKMODE_ALL = 5,
  PK_MODE_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  PK_MODE_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool PK_MODE_IsValid(int value);
constexpr PK_MODE PK_MODE_MIN = PKMODE_PEACE;
constexpr PK_MODE PK_MODE_MAX = PKMODE_ALL;
constexpr int PK_MODE_ARRAYSIZE = PK_MODE_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* PK_MODE_descriptor();
template<typename T>
inline const std::string& PK_MODE_Name(T enum_t_value) {
  static_assert(::std::is_same<T, PK_MODE>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function PK_MODE_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    PK_MODE_descriptor(), enum_t_value);
}
inline bool PK_MODE_Parse(
    const std::string& name, PK_MODE* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<PK_MODE>(
    PK_MODE_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)


PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::ACTOR_ATTRIB> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::ACTOR_ATTRIB>() {
  return ::ACTOR_ATTRIB_descriptor();
}
template <> struct is_proto_enum< ::ACTOR_PROPERTY> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::ACTOR_PROPERTY>() {
  return ::ACTOR_PROPERTY_descriptor();
}
template <> struct is_proto_enum< ::MONEY_TYPE> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::MONEY_TYPE>() {
  return ::MONEY_TYPE_descriptor();
}
template <> struct is_proto_enum< ::PK_MODE> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::PK_MODE>() {
  return ::PK_MODE_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_common_2fCommon_2eproto
