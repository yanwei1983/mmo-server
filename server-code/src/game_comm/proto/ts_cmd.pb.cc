// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ts_cmd.proto

#include "ts_cmd.pb.h"

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
static constexpr ::PROTOBUF_NAMESPACE_ID::Metadata* file_level_metadata_ts_5fcmd_2eproto = nullptr;
static const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* file_level_enum_descriptors_ts_5fcmd_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_ts_5fcmd_2eproto = nullptr;
const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_ts_5fcmd_2eproto::offsets[1] = {};
static constexpr ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema* schemas = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::Message* const* file_default_instances = nullptr;

const char descriptor_table_protodef_ts_5fcmd_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\014ts_cmd.proto*\325\016\n\006TS_CMD\022\r\n\tCMD_BEGIN\020\000"
  "\022\016\n\nCMD_SC_KEY\020\001\022\014\n\010CMD_PING\020\002\022\014\n\010CMD_PO"
  "NG\020\003\022\020\n\014CMD_CS_LOGIN\020\n\022\020\n\014CMD_SC_LOGIN\020\013"
  "\022\030\n\024CMD_SC_ACTORINFOLIST\020\014\022\023\n\017CMD_SC_WAI"
  "TINFO\020\r\022\026\n\022CMD_CS_CREATEACTOR\020\016\022\026\n\022CMD_S"
  "C_CREATEACTOR\020\017\022\026\n\022CMD_CS_SELECTACTOR\020\020\022"
  "\026\n\022CMD_SC_SELECTACTOR\020\021\022\022\n\016CMD_SC_LOADMA"
  "P\020d\022\027\n\023CMD_CS_LOADMAP_SUCC\020e\022\023\n\017CMD_SC_E"
  "NTERMAP\020f\022\025\n\021CMD_SC_PLAYERINFO\020g\022\026\n\022CMD_"
  "SC_ACTORATTRIB\020h\022\025\n\021CMD_SC_COMMONDATA\020i\022"
  "\024\n\020CMD_SC_DATACOUNT\020j\022\021\n\rCMD_SC_SYSVAR\020k"
  "\022\023\n\017CMD_SC_COOLDOWN\020l\022\022\n\016CMD_SC_AOI_NEW\020"
  "x\022\025\n\021CMD_SC_AOI_REMOVE\020y\022\025\n\021CMD_SC_AOI_U"
  "PDATE\020z\022\017\n\013CMD_CS_MOVE\020{\022\024\n\020CMD_CS_CHANG"
  "EMAP\020|\022\025\n\020CMD_CS_CASTSKILL\020\202\001\022\025\n\020CMD_SC_"
  "CASTSKILL\020\203\001\022\027\n\022CMD_CS_SKILL_BREAK\020\204\001\022\027\n"
  "\022CMD_SC_SKILL_BREAK\020\205\001\022\036\n\031CMD_CS_SKILL_C"
  "HGTARGETPOS\020\206\001\022\030\n\023CMD_SC_SKILL_DAMAGE\020\207\001"
  "\022\026\n\021CMD_SC_SKILL_STUN\020\210\001\022\030\n\023CMD_SC_SKILL"
  "_EFFACT\020\211\001\022\022\n\rCMD_SC_DAMAGE\020\212\001\022\020\n\013CMD_SC"
  "_DEAD\020\213\001\022\031\n\024CMD_SC_ATTRIB_CHANGE\020\214\001\022\031\n\024C"
  "MD_CS_CHANGE_PKMODE\020\215\001\022\031\n\024CMD_SC_CHANGE_"
  "PKMODE\020\216\001\022\022\n\rCMD_CS_REBORN\020\217\001\022\027\n\022CMD_SC_"
  "STATUS_LIST\020\226\001\022\027\n\022CMD_SC_STATUS_INFO\020\227\001\022"
  "\031\n\024CMD_SC_STATUS_ACTION\020\230\001\022\027\n\022CMD_SC_ITE"
  "M_DELETE\020\240\001\022\025\n\020CMD_SC_ITEM_INFO\020\241\001\022\027\n\022CM"
  "D_SC_ITEM_CHANGE\020\242\001\022\025\n\020CMD_CS_ITEM_SWAP\020"
  "\243\001\022\026\n\021CMD_CS_ITEM_SPLIT\020\244\001\022\030\n\023CMD_CS_ITE"
  "M_COMBINE\020\245\001\022\025\n\020CMD_CS_ITEM_TIDY\020\246\001\022\024\n\017C"
  "MD_CS_ITEM_USE\020\247\001\022\035\n\030CMD_CS_ITEM_STORAGE"
  "_OPEN\020\252\001\022 \n\033CMD_CS_ITEM_STORAGE_CHECKIN\020"
  "\253\001\022!\n\034CMD_CS_ITEM_STORAGE_CHECKOUT\020\254\001\022\021\n"
  "\014CMD_CS_EQUIP\020\264\001\022\023\n\016CMD_CS_UNEQUIP\020\265\001\022\020\n"
  "\013CMD_CS_TALK\020\276\001\022\020\n\013CMD_SC_TALK\020\277\001\022\025\n\020CMD"
  "_SC_TASK_INFO\020\310\001\022\025\n\020CMD_SC_TASK_DATA\020\311\001\022"
  "\027\n\022CMD_CS_TASK_SUBMIT\020\312\001\022\026\n\021CMD_CS_ACTIV"
  "E_NPC\020\315\001\022\022\n\rCMD_SC_DIALOG\020\316\001\022\030\n\023CMD_CS_D"
  "IALOG_CLICK\020\317\001\022\025\n\020CMD_SC_ACHI_INFO\020\322\001\022\025\n"
  "\020CMD_CS_ACHI_TAKE\020\323\001\022\033\n\026CMD_SC_TEAMMEMBE"
  "R_INFO\020\325\001\022\035\n\030CMD_SC_TEAMMEMBER_ACTION\020\326\001"
  "\022\026\n\021CMD_CS_TEAMCREATE\020\327\001\022\024\n\017CMD_CS_TEAMQ"
  "UIT\020\330\001\022\032\n\025CMD_CS_TEAMKICKMEMBER\020\331\001\022\034\n\027CM"
  "D_CS_TEAMINVITEMEMBER\020\332\001\022\034\n\027CMD_SC_TEAMI"
  "NVITEMEMBER\020\333\001\022\034\n\027CMD_CS_TEAMACCEPTINVIT"
  "E\020\334\001\022\031\n\024CMD_CS_TEAMNEWLEADER\020\335\001\022\033\n\026CMD_C"
  "S_TEAMAPPLYMEMBER\020\336\001\022\033\n\026CMD_SC_TEAMAPPLY"
  "MEMBER\020\337\001\022\033\n\026CMD_CS_TEAMACCEPTAPPLY\020\340\001\022\016"
  "\n\tCMD_CLOSE\020\220Nb\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_ts_5fcmd_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_ts_5fcmd_2eproto_sccs[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_ts_5fcmd_2eproto_once;
static bool descriptor_table_ts_5fcmd_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_ts_5fcmd_2eproto = {
  &descriptor_table_ts_5fcmd_2eproto_initialized, descriptor_table_protodef_ts_5fcmd_2eproto, "ts_cmd.proto", 1902,
  &descriptor_table_ts_5fcmd_2eproto_once, descriptor_table_ts_5fcmd_2eproto_sccs, descriptor_table_ts_5fcmd_2eproto_deps, 0, 0,
  schemas, file_default_instances, TableStruct_ts_5fcmd_2eproto::offsets,
  file_level_metadata_ts_5fcmd_2eproto, 0, file_level_enum_descriptors_ts_5fcmd_2eproto, file_level_service_descriptors_ts_5fcmd_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_ts_5fcmd_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_ts_5fcmd_2eproto), true);
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* TS_CMD_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_ts_5fcmd_2eproto);
  return file_level_enum_descriptors_ts_5fcmd_2eproto[0];
}
bool TS_CMD_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 100:
    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
    case 106:
    case 107:
    case 108:
    case 120:
    case 121:
    case 122:
    case 123:
    case 124:
    case 130:
    case 131:
    case 132:
    case 133:
    case 134:
    case 135:
    case 136:
    case 137:
    case 138:
    case 139:
    case 140:
    case 141:
    case 142:
    case 143:
    case 150:
    case 151:
    case 152:
    case 160:
    case 161:
    case 162:
    case 163:
    case 164:
    case 165:
    case 166:
    case 167:
    case 170:
    case 171:
    case 172:
    case 180:
    case 181:
    case 190:
    case 191:
    case 200:
    case 201:
    case 202:
    case 205:
    case 206:
    case 207:
    case 210:
    case 211:
    case 213:
    case 214:
    case 215:
    case 216:
    case 217:
    case 218:
    case 219:
    case 220:
    case 221:
    case 222:
    case 223:
    case 224:
    case 10000:
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
