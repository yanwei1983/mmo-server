// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: config/Cfg_Npc.proto

#include "config/Cfg_Npc.pb.h"

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
extern PROTOBUF_INTERNAL_EXPORT_config_2fCfg_5fNpc_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_Cfg_Npc_Row_config_2fCfg_5fNpc_2eproto;
class Cfg_Npc_RowDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Cfg_Npc_Row> _instance;
} _Cfg_Npc_Row_default_instance_;
class Cfg_NpcDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Cfg_Npc> _instance;
} _Cfg_Npc_default_instance_;
static void InitDefaultsscc_info_Cfg_Npc_config_2fCfg_5fNpc_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_Cfg_Npc_default_instance_;
    new (ptr) ::Cfg_Npc();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Cfg_Npc::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_Cfg_Npc_config_2fCfg_5fNpc_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_Cfg_Npc_config_2fCfg_5fNpc_2eproto}, {
      &scc_info_Cfg_Npc_Row_config_2fCfg_5fNpc_2eproto.base,}};

static void InitDefaultsscc_info_Cfg_Npc_Row_config_2fCfg_5fNpc_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_Cfg_Npc_Row_default_instance_;
    new (ptr) ::Cfg_Npc_Row();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Cfg_Npc_Row::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_Cfg_Npc_Row_config_2fCfg_5fNpc_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_Cfg_Npc_Row_config_2fCfg_5fNpc_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_config_2fCfg_5fNpc_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_config_2fCfg_5fNpc_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_config_2fCfg_5fNpc_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_config_2fCfg_5fNpc_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc_Row, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc_Row, id_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc_Row, level_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc_Row, name_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc_Row, dialog_txt_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc_Row, type_flag_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc_Row, shop_id_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc_Row, shop_linkname_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc_Row, idcamp_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc_Row, scriptid_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc_Row, map_id_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc_Row, posx_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc_Row, posy_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc_Row, face_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc_Row, high_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc_Row, volume_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Cfg_Npc, rows_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::Cfg_Npc_Row)},
  { 20, -1, sizeof(::Cfg_Npc)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_Cfg_Npc_Row_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_Cfg_Npc_default_instance_),
};

const char descriptor_table_protodef_config_2fCfg_5fNpc_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\024config/Cfg_Npc.proto\"\237\002\n\007Cfg_Npc\022\032\n\004ro"
  "ws\030\001 \003(\0132\014.Cfg_Npc.Row\032\367\001\n\003Row\022\n\n\002id\030\001 \001"
  "(\r\022\r\n\005level\030\002 \001(\r\022\014\n\004name\030\003 \001(\t\022\022\n\ndialo"
  "g_txt\030\004 \001(\t\022\021\n\ttype_flag\030\005 \001(\r\022\017\n\007shop_i"
  "d\030\006 \001(\r\022\025\n\rshop_linkname\030\007 \001(\t\022\016\n\006idcamp"
  "\030\010 \001(\r\022\020\n\010scriptid\030\t \001(\004\022\016\n\006map_id\030\n \001(\r"
  "\022\014\n\004posx\030\013 \001(\002\022\014\n\004posy\030\014 \001(\002\022\014\n\004face\030\r \001"
  "(\002\022\014\n\004high\030\016 \001(\002\022\016\n\006volume\030\017 \001(\002B\002H\002b\006pr"
  "oto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_config_2fCfg_5fNpc_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_config_2fCfg_5fNpc_2eproto_sccs[2] = {
  &scc_info_Cfg_Npc_config_2fCfg_5fNpc_2eproto.base,
  &scc_info_Cfg_Npc_Row_config_2fCfg_5fNpc_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_config_2fCfg_5fNpc_2eproto_once;
static bool descriptor_table_config_2fCfg_5fNpc_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_config_2fCfg_5fNpc_2eproto = {
  &descriptor_table_config_2fCfg_5fNpc_2eproto_initialized, descriptor_table_protodef_config_2fCfg_5fNpc_2eproto, "config/Cfg_Npc.proto", 324,
  &descriptor_table_config_2fCfg_5fNpc_2eproto_once, descriptor_table_config_2fCfg_5fNpc_2eproto_sccs, descriptor_table_config_2fCfg_5fNpc_2eproto_deps, 2, 0,
  schemas, file_default_instances, TableStruct_config_2fCfg_5fNpc_2eproto::offsets,
  file_level_metadata_config_2fCfg_5fNpc_2eproto, 2, file_level_enum_descriptors_config_2fCfg_5fNpc_2eproto, file_level_service_descriptors_config_2fCfg_5fNpc_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_config_2fCfg_5fNpc_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_config_2fCfg_5fNpc_2eproto), true);

// ===================================================================

void Cfg_Npc_Row::InitAsDefaultInstance() {
}
class Cfg_Npc_Row::_Internal {
 public:
};

Cfg_Npc_Row::Cfg_Npc_Row()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Cfg_Npc.Row)
}
Cfg_Npc_Row::Cfg_Npc_Row(const Cfg_Npc_Row& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_name().empty()) {
    name_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.name_);
  }
  dialog_txt_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_dialog_txt().empty()) {
    dialog_txt_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.dialog_txt_);
  }
  shop_linkname_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_shop_linkname().empty()) {
    shop_linkname_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.shop_linkname_);
  }
  ::memcpy(&id_, &from.id_,
    static_cast<size_t>(reinterpret_cast<char*>(&volume_) -
    reinterpret_cast<char*>(&id_)) + sizeof(volume_));
  // @@protoc_insertion_point(copy_constructor:Cfg_Npc.Row)
}

void Cfg_Npc_Row::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_Cfg_Npc_Row_config_2fCfg_5fNpc_2eproto.base);
  name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  dialog_txt_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  shop_linkname_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ::memset(&id_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&volume_) -
      reinterpret_cast<char*>(&id_)) + sizeof(volume_));
}

Cfg_Npc_Row::~Cfg_Npc_Row() {
  // @@protoc_insertion_point(destructor:Cfg_Npc.Row)
  SharedDtor();
}

void Cfg_Npc_Row::SharedDtor() {
  name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  dialog_txt_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  shop_linkname_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void Cfg_Npc_Row::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Cfg_Npc_Row& Cfg_Npc_Row::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Cfg_Npc_Row_config_2fCfg_5fNpc_2eproto.base);
  return *internal_default_instance();
}


void Cfg_Npc_Row::InternalSwap(Cfg_Npc_Row* other) {
  using std::swap;
  GetReflection()->Swap(this, other);}

::PROTOBUF_NAMESPACE_ID::Metadata Cfg_Npc_Row::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void Cfg_Npc::InitAsDefaultInstance() {
}
class Cfg_Npc::_Internal {
 public:
};

Cfg_Npc::Cfg_Npc()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Cfg_Npc)
}
Cfg_Npc::Cfg_Npc(const Cfg_Npc& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      rows_(from.rows_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:Cfg_Npc)
}

void Cfg_Npc::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_Cfg_Npc_config_2fCfg_5fNpc_2eproto.base);
}

Cfg_Npc::~Cfg_Npc() {
  // @@protoc_insertion_point(destructor:Cfg_Npc)
  SharedDtor();
}

void Cfg_Npc::SharedDtor() {
}

void Cfg_Npc::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Cfg_Npc& Cfg_Npc::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Cfg_Npc_config_2fCfg_5fNpc_2eproto.base);
  return *internal_default_instance();
}


void Cfg_Npc::InternalSwap(Cfg_Npc* other) {
  using std::swap;
  GetReflection()->Swap(this, other);}

::PROTOBUF_NAMESPACE_ID::Metadata Cfg_Npc::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Cfg_Npc_Row* Arena::CreateMaybeMessage< ::Cfg_Npc_Row >(Arena* arena) {
  return Arena::CreateInternal< ::Cfg_Npc_Row >(arena);
}
template<> PROTOBUF_NOINLINE ::Cfg_Npc* Arena::CreateMaybeMessage< ::Cfg_Npc >(Arena* arena) {
  return Arena::CreateInternal< ::Cfg_Npc >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
