// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: config/Cfg_Monster.proto

#include "config/Cfg_Monster.pb.h"

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
extern PROTOBUF_INTERNAL_EXPORT_common_2fAttribChangeDataProto_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_AttribDataProto_common_2fAttribChangeDataProto_2eproto;
extern PROTOBUF_INTERNAL_EXPORT_config_2fCfg_5fMonster_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_Cfg_Monster_Row_config_2fCfg_5fMonster_2eproto;
class Cfg_Monster_RowDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Cfg_Monster_Row> _instance;
} _Cfg_Monster_Row_default_instance_;
class Cfg_MonsterDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Cfg_Monster> _instance;
} _Cfg_Monster_default_instance_;
static void InitDefaultsscc_info_Cfg_Monster_config_2fCfg_5fMonster_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_Cfg_Monster_default_instance_;
    new (ptr) ::Cfg_Monster();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Cfg_Monster::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_Cfg_Monster_config_2fCfg_5fMonster_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_Cfg_Monster_config_2fCfg_5fMonster_2eproto}, {
      &scc_info_Cfg_Monster_Row_config_2fCfg_5fMonster_2eproto.base,}};

static void InitDefaultsscc_info_Cfg_Monster_Row_config_2fCfg_5fMonster_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_Cfg_Monster_Row_default_instance_;
    new (ptr) ::Cfg_Monster_Row();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Cfg_Monster_Row::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_Cfg_Monster_Row_config_2fCfg_5fMonster_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_Cfg_Monster_Row_config_2fCfg_5fMonster_2eproto}, {
      &scc_info_AttribDataProto_common_2fAttribChangeDataProto_2eproto.base,}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_config_2fCfg_5fMonster_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_config_2fCfg_5fMonster_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_config_2fCfg_5fMonster_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_config_2fCfg_5fMonster_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Cfg_Monster_Row, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Cfg_Monster_Row, id_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Monster_Row, level_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Monster_Row, name_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Monster_Row, monster_type_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Monster_Row, monster_flag_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Monster_Row, kill_exp_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Monster_Row, drop_id_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Monster_Row, idcamp_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Monster_Row, scriptid_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Monster_Row, aitype_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Monster_Row, high_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Monster_Row, volume_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Monster_Row, attr_data_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Cfg_Monster, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Cfg_Monster, rows_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::Cfg_Monster_Row)},
  { 18, -1, sizeof(::Cfg_Monster)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_Cfg_Monster_Row_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_Cfg_Monster_default_instance_),
};

const char descriptor_table_protodef_config_2fCfg_5fMonster_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\030config/Cfg_Monster.proto\032\"common/Attri"
  "bChangeDataProto.proto\"\242\002\n\013Cfg_Monster\022\036"
  "\n\004rows\030\001 \003(\0132\020.Cfg_Monster.Row\032\362\001\n\003Row\022\n"
  "\n\002id\030\001 \001(\r\022\r\n\005level\030\002 \001(\r\022\014\n\004name\030\003 \001(\t\022"
  "\024\n\014monster_type\030\004 \001(\r\022\024\n\014monster_flag\030\005 "
  "\001(\r\022\020\n\010kill_exp\030\006 \001(\r\022\017\n\007drop_id\030\007 \001(\r\022\016"
  "\n\006idcamp\030\010 \001(\r\022\020\n\010scriptid\030\t \001(\004\022\016\n\006aity"
  "pe\030\n \001(\r\022\014\n\004high\030\013 \001(\002\022\016\n\006volume\030\014 \001(\002\022#"
  "\n\tattr_data\030\024 \001(\0132\020.AttribDataProtoB\002H\002b"
  "\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_config_2fCfg_5fMonster_2eproto_deps[1] = {
  &::descriptor_table_common_2fAttribChangeDataProto_2eproto,
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_config_2fCfg_5fMonster_2eproto_sccs[2] = {
  &scc_info_Cfg_Monster_config_2fCfg_5fMonster_2eproto.base,
  &scc_info_Cfg_Monster_Row_config_2fCfg_5fMonster_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_config_2fCfg_5fMonster_2eproto_once;
static bool descriptor_table_config_2fCfg_5fMonster_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_config_2fCfg_5fMonster_2eproto = {
  &descriptor_table_config_2fCfg_5fMonster_2eproto_initialized, descriptor_table_protodef_config_2fCfg_5fMonster_2eproto, "config/Cfg_Monster.proto", 367,
  &descriptor_table_config_2fCfg_5fMonster_2eproto_once, descriptor_table_config_2fCfg_5fMonster_2eproto_sccs, descriptor_table_config_2fCfg_5fMonster_2eproto_deps, 2, 1,
  schemas, file_default_instances, TableStruct_config_2fCfg_5fMonster_2eproto::offsets,
  file_level_metadata_config_2fCfg_5fMonster_2eproto, 2, file_level_enum_descriptors_config_2fCfg_5fMonster_2eproto, file_level_service_descriptors_config_2fCfg_5fMonster_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_config_2fCfg_5fMonster_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_config_2fCfg_5fMonster_2eproto), true);

// ===================================================================

void Cfg_Monster_Row::InitAsDefaultInstance() {
  ::_Cfg_Monster_Row_default_instance_._instance.get_mutable()->attr_data_ = const_cast< ::AttribDataProto*>(
      ::AttribDataProto::internal_default_instance());
}
class Cfg_Monster_Row::_Internal {
 public:
  static const ::AttribDataProto& attr_data(const Cfg_Monster_Row* msg);
};

const ::AttribDataProto&
Cfg_Monster_Row::_Internal::attr_data(const Cfg_Monster_Row* msg) {
  return *msg->attr_data_;
}
void Cfg_Monster_Row::clear_attr_data() {
  if (GetArenaNoVirtual() == nullptr && attr_data_ != nullptr) {
    delete attr_data_;
  }
  attr_data_ = nullptr;
}
Cfg_Monster_Row::Cfg_Monster_Row()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Cfg_Monster.Row)
}
Cfg_Monster_Row::Cfg_Monster_Row(const Cfg_Monster_Row& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_name().empty()) {
    name_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.name_);
  }
  if (from._internal_has_attr_data()) {
    attr_data_ = new ::AttribDataProto(*from.attr_data_);
  } else {
    attr_data_ = nullptr;
  }
  ::memcpy(&id_, &from.id_,
    static_cast<size_t>(reinterpret_cast<char*>(&volume_) -
    reinterpret_cast<char*>(&id_)) + sizeof(volume_));
  // @@protoc_insertion_point(copy_constructor:Cfg_Monster.Row)
}

void Cfg_Monster_Row::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_Cfg_Monster_Row_config_2fCfg_5fMonster_2eproto.base);
  name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ::memset(&attr_data_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&volume_) -
      reinterpret_cast<char*>(&attr_data_)) + sizeof(volume_));
}

Cfg_Monster_Row::~Cfg_Monster_Row() {
  // @@protoc_insertion_point(destructor:Cfg_Monster.Row)
  SharedDtor();
}

void Cfg_Monster_Row::SharedDtor() {
  name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (this != internal_default_instance()) delete attr_data_;
}

void Cfg_Monster_Row::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Cfg_Monster_Row& Cfg_Monster_Row::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Cfg_Monster_Row_config_2fCfg_5fMonster_2eproto.base);
  return *internal_default_instance();
}


void Cfg_Monster_Row::InternalSwap(Cfg_Monster_Row* other) {
  using std::swap;
  GetReflection()->Swap(this, other);}

::PROTOBUF_NAMESPACE_ID::Metadata Cfg_Monster_Row::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void Cfg_Monster::InitAsDefaultInstance() {
}
class Cfg_Monster::_Internal {
 public:
};

Cfg_Monster::Cfg_Monster()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Cfg_Monster)
}
Cfg_Monster::Cfg_Monster(const Cfg_Monster& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      rows_(from.rows_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:Cfg_Monster)
}

void Cfg_Monster::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_Cfg_Monster_config_2fCfg_5fMonster_2eproto.base);
}

Cfg_Monster::~Cfg_Monster() {
  // @@protoc_insertion_point(destructor:Cfg_Monster)
  SharedDtor();
}

void Cfg_Monster::SharedDtor() {
}

void Cfg_Monster::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Cfg_Monster& Cfg_Monster::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Cfg_Monster_config_2fCfg_5fMonster_2eproto.base);
  return *internal_default_instance();
}


void Cfg_Monster::InternalSwap(Cfg_Monster* other) {
  using std::swap;
  GetReflection()->Swap(this, other);}

::PROTOBUF_NAMESPACE_ID::Metadata Cfg_Monster::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Cfg_Monster_Row* Arena::CreateMaybeMessage< ::Cfg_Monster_Row >(Arena* arena) {
  return Arena::CreateInternal< ::Cfg_Monster_Row >(arena);
}
template<> PROTOBUF_NOINLINE ::Cfg_Monster* Arena::CreateMaybeMessage< ::Cfg_Monster >(Arena* arena) {
  return Arena::CreateInternal< ::Cfg_Monster >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
