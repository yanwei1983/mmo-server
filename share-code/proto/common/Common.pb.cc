// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: common/Common.proto

#include "common/Common.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
static constexpr ::PROTOBUF_NAMESPACE_ID::Metadata* file_level_metadata_common_2fCommon_2eproto = nullptr;
static const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* file_level_enum_descriptors_common_2fCommon_2eproto[5];
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_common_2fCommon_2eproto = nullptr;
const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_common_2fCommon_2eproto::offsets[1] = {};
static constexpr ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema* schemas = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::Message* const* file_default_instances = nullptr;

const char descriptor_table_protodef_common_2fCommon_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\023common/Common.proto*\277\003\n\014ACTOR_ATTRIB\022\017"
  "\n\013ATTRIB_NONE\020\000\022\021\n\rATTRIB_HP_MAX\020\001\022\021\n\rAT"
  "TRIB_MP_MAX\020\002\022\021\n\rATTRIB_FP_MAX\020\003\022\021\n\rATTR"
  "IB_NP_MAX\020\004\022\022\n\016ATTRIB_MOVESPD\020\005\022\022\n\016ATTRI"
  "B_MIN_ATK\020\t\022\022\n\016ATTRIB_MAX_ATK\020\n\022\022\n\016ATTRI"
  "B_MIN_DEF\020\013\022\022\n\016ATTRIB_MAX_DEF\020\014\022\023\n\017ATTRI"
  "B_MIN_MATK\020\r\022\023\n\017ATTRIB_MAX_MATK\020\016\022\023\n\017ATT"
  "RIB_MIN_MDEF\020\017\022\023\n\017ATTRIB_MAX_MDEF\020\020\022\016\n\nA"
  "TTRIB_HIT\020\021\022\020\n\014ATTRIB_DODGE\020\022\022\025\n\021ATTRIB_"
  "DAMAGE_ADJ\020\023\022\036\n\032ATTRIB_DAMAGE_REFLECT_RA"
  "TE\020\024\022\035\n\031ATTRIB_DAMAGE_REFLECT_ADJ\020\025\022\022\n\016A"
  "TTRIB_EXP_ADJ\020\026\022\016\n\nATTRIB_MAX\020\027*\342\002\n\016ACTO"
  "R_PROPERTY\022\r\n\tPROP_NONE\020\000\022\014\n\010PROP_EXP\020\001\022"
  "\016\n\nPROP_LEVEL\020\002\022\013\n\007PROP_HP\020\003\022\013\n\007PROP_MP\020"
  "\004\022\013\n\007PROP_FP\020\005\022\013\n\007PROP_NP\020\006\022\016\n\nPROP_MONE"
  "Y\020\n\022\022\n\016PROP_MONEYBIND\020\013\022\r\n\tPROP_GOLD\020\014\022\021"
  "\n\rPROP_GOLDBIND\020\r\022\016\n\nPROP_PKVAL\020\016\022\016\n\nPRO"
  "P_HONOR\020\017\022\r\n\tPROP_CAMP\020\020\022\022\n\016PROP_ACHIPOI"
  "NT\020\021\022\017\n\013PROP_VIPLEV\020\022\022\017\n\013PROP_HP_MAX\0202\022\026"
  "\n\022PROP_WEAPON_CHANGE\0203\022\025\n\021PROP_ARMOR_CHA"
  "NGE\0204\022\025\n\021PROP_MOUNT_CHANGE\0205*W\n\nMONEY_TY"
  "PE\022\013\n\007MT_NONE\020\000\022\014\n\010MT_MONEY\020\001\022\020\n\014MT_MONE"
  "YBIND\020\002\022\013\n\007MT_GOLD\020\003\022\017\n\013MT_GOLDBIND\020\004*s\n"
  "\007PK_MODE\022\020\n\014PKMODE_PEACE\020\000\022\022\n\016PKMODE_RED"
  "NAME\020\001\022\017\n\013PKMODE_TEAM\020\002\022\020\n\014PKMODE_GUILD\020"
  "\003\022\017\n\013PKMODE_CAMP\020\004\022\016\n\nPKMODE_ALL\020\005*j\n\tTa"
  "skState\022\027\n\023TASKSTATE_NOTACCEPT\020\000\022\026\n\022TASK"
  "STATE_ACCEPTED\020\001\022\024\n\020TASKSTATE_GIVEUP\020\002\022\026"
  "\n\022TASKSTATE_FINISHED\020\003B\002H\002b\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_common_2fCommon_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_common_2fCommon_2eproto_sccs[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_common_2fCommon_2eproto_once;
static bool descriptor_table_common_2fCommon_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_common_2fCommon_2eproto = {
  &descriptor_table_common_2fCommon_2eproto_initialized, descriptor_table_protodef_common_2fCommon_2eproto, "common/Common.proto", 1154,
  &descriptor_table_common_2fCommon_2eproto_once, descriptor_table_common_2fCommon_2eproto_sccs, descriptor_table_common_2fCommon_2eproto_deps, 0, 0,
  schemas, file_default_instances, TableStruct_common_2fCommon_2eproto::offsets,
  file_level_metadata_common_2fCommon_2eproto, 0, file_level_enum_descriptors_common_2fCommon_2eproto, file_level_service_descriptors_common_2fCommon_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_common_2fCommon_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_common_2fCommon_2eproto), true);
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ACTOR_ATTRIB_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_common_2fCommon_2eproto);
  return file_level_enum_descriptors_common_2fCommon_2eproto[0];
}
bool ACTOR_ATTRIB_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
      return true;
    default:
      return false;
  }
}

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ACTOR_PROPERTY_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_common_2fCommon_2eproto);
  return file_level_enum_descriptors_common_2fCommon_2eproto[1];
}
bool ACTOR_PROPERTY_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 50:
    case 51:
    case 52:
    case 53:
      return true;
    default:
      return false;
  }
}

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* MONEY_TYPE_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_common_2fCommon_2eproto);
  return file_level_enum_descriptors_common_2fCommon_2eproto[2];
}
bool MONEY_TYPE_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
      return true;
    default:
      return false;
  }
}

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* PK_MODE_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_common_2fCommon_2eproto);
  return file_level_enum_descriptors_common_2fCommon_2eproto[3];
}
bool PK_MODE_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      return true;
    default:
      return false;
  }
}

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* TaskState_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_common_2fCommon_2eproto);
  return file_level_enum_descriptors_common_2fCommon_2eproto[4];
}
bool TaskState_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
