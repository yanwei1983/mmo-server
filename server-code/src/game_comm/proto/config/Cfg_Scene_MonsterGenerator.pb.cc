// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: config/Cfg_Scene_MonsterGenerator.proto

#include "config/Cfg_Scene_MonsterGenerator.pb.h"

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
extern PROTOBUF_INTERNAL_EXPORT_config_2fCfg_5fScene_5fMonsterGenerator_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_Cfg_Scene_MonsterGenerator_Row_config_2fCfg_5fScene_5fMonsterGenerator_2eproto;
class Cfg_Scene_MonsterGenerator_RowDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Cfg_Scene_MonsterGenerator_Row> _instance;
} _Cfg_Scene_MonsterGenerator_Row_default_instance_;
class Cfg_Scene_MonsterGeneratorDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Cfg_Scene_MonsterGenerator> _instance;
} _Cfg_Scene_MonsterGenerator_default_instance_;
static void InitDefaultsscc_info_Cfg_Scene_MonsterGenerator_config_2fCfg_5fScene_5fMonsterGenerator_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_Cfg_Scene_MonsterGenerator_default_instance_;
    new (ptr) ::Cfg_Scene_MonsterGenerator();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Cfg_Scene_MonsterGenerator::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_Cfg_Scene_MonsterGenerator_config_2fCfg_5fScene_5fMonsterGenerator_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_Cfg_Scene_MonsterGenerator_config_2fCfg_5fScene_5fMonsterGenerator_2eproto}, {
      &scc_info_Cfg_Scene_MonsterGenerator_Row_config_2fCfg_5fScene_5fMonsterGenerator_2eproto.base,}};

static void InitDefaultsscc_info_Cfg_Scene_MonsterGenerator_Row_config_2fCfg_5fScene_5fMonsterGenerator_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_Cfg_Scene_MonsterGenerator_Row_default_instance_;
    new (ptr) ::Cfg_Scene_MonsterGenerator_Row();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Cfg_Scene_MonsterGenerator_Row::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_Cfg_Scene_MonsterGenerator_Row_config_2fCfg_5fScene_5fMonsterGenerator_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_Cfg_Scene_MonsterGenerator_Row_config_2fCfg_5fScene_5fMonsterGenerator_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_config_2fCfg_5fScene_5fMonsterGenerator_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_config_2fCfg_5fScene_5fMonsterGenerator_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_config_2fCfg_5fScene_5fMonsterGenerator_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_config_2fCfg_5fScene_5fMonsterGenerator_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_MonsterGenerator_Row, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_MonsterGenerator_Row, idmap_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_MonsterGenerator_Row, idx_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_MonsterGenerator_Row, active_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_MonsterGenerator_Row, shape_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_MonsterGenerator_Row, x_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_MonsterGenerator_Row, y_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_MonsterGenerator_Row, range_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_MonsterGenerator_Row, width_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_MonsterGenerator_Row, monsterid_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_MonsterGenerator_Row, per_gen_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_MonsterGenerator_Row, wait_time_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_MonsterGenerator_Row, gen_max_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_MonsterGenerator_Row, camp_id_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_MonsterGenerator, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_MonsterGenerator, rows_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::Cfg_Scene_MonsterGenerator_Row)},
  { 18, -1, sizeof(::Cfg_Scene_MonsterGenerator)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_Cfg_Scene_MonsterGenerator_Row_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_Cfg_Scene_MonsterGenerator_default_instance_),
};

const char descriptor_table_protodef_config_2fCfg_5fScene_5fMonsterGenerator_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\'config/Cfg_Scene_MonsterGenerator.prot"
  "o\"\233\002\n\032Cfg_Scene_MonsterGenerator\022-\n\004rows"
  "\030\001 \003(\0132\037.Cfg_Scene_MonsterGenerator.Row\032"
  "\315\001\n\003Row\022\r\n\005idmap\030\001 \001(\r\022\013\n\003idx\030\002 \001(\r\022\016\n\006a"
  "ctive\030\003 \001(\r\022\r\n\005shape\030\004 \001(\r\022\t\n\001x\030\005 \001(\002\022\t\n"
  "\001y\030\006 \001(\002\022\r\n\005range\030\007 \001(\002\022\r\n\005width\030\010 \001(\002\022\021"
  "\n\tmonsterid\030\t \001(\r\022\017\n\007per_gen\030\n \001(\r\022\021\n\twa"
  "it_time\030\013 \001(\r\022\017\n\007gen_max\030\014 \001(\r\022\017\n\007camp_i"
  "d\030\r \001(\rB\002H\002b\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_config_2fCfg_5fScene_5fMonsterGenerator_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_config_2fCfg_5fScene_5fMonsterGenerator_2eproto_sccs[2] = {
  &scc_info_Cfg_Scene_MonsterGenerator_config_2fCfg_5fScene_5fMonsterGenerator_2eproto.base,
  &scc_info_Cfg_Scene_MonsterGenerator_Row_config_2fCfg_5fScene_5fMonsterGenerator_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_config_2fCfg_5fScene_5fMonsterGenerator_2eproto_once;
static bool descriptor_table_config_2fCfg_5fScene_5fMonsterGenerator_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_config_2fCfg_5fScene_5fMonsterGenerator_2eproto = {
  &descriptor_table_config_2fCfg_5fScene_5fMonsterGenerator_2eproto_initialized, descriptor_table_protodef_config_2fCfg_5fScene_5fMonsterGenerator_2eproto, "config/Cfg_Scene_MonsterGenerator.proto", 339,
  &descriptor_table_config_2fCfg_5fScene_5fMonsterGenerator_2eproto_once, descriptor_table_config_2fCfg_5fScene_5fMonsterGenerator_2eproto_sccs, descriptor_table_config_2fCfg_5fScene_5fMonsterGenerator_2eproto_deps, 2, 0,
  schemas, file_default_instances, TableStruct_config_2fCfg_5fScene_5fMonsterGenerator_2eproto::offsets,
  file_level_metadata_config_2fCfg_5fScene_5fMonsterGenerator_2eproto, 2, file_level_enum_descriptors_config_2fCfg_5fScene_5fMonsterGenerator_2eproto, file_level_service_descriptors_config_2fCfg_5fScene_5fMonsterGenerator_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_config_2fCfg_5fScene_5fMonsterGenerator_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_config_2fCfg_5fScene_5fMonsterGenerator_2eproto), true);

// ===================================================================

void Cfg_Scene_MonsterGenerator_Row::InitAsDefaultInstance() {
}
class Cfg_Scene_MonsterGenerator_Row::_Internal {
 public:
};

Cfg_Scene_MonsterGenerator_Row::Cfg_Scene_MonsterGenerator_Row()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Cfg_Scene_MonsterGenerator.Row)
}
Cfg_Scene_MonsterGenerator_Row::Cfg_Scene_MonsterGenerator_Row(const Cfg_Scene_MonsterGenerator_Row& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::memcpy(&idmap_, &from.idmap_,
    static_cast<size_t>(reinterpret_cast<char*>(&camp_id_) -
    reinterpret_cast<char*>(&idmap_)) + sizeof(camp_id_));
  // @@protoc_insertion_point(copy_constructor:Cfg_Scene_MonsterGenerator.Row)
}

void Cfg_Scene_MonsterGenerator_Row::SharedCtor() {
  ::memset(&idmap_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&camp_id_) -
      reinterpret_cast<char*>(&idmap_)) + sizeof(camp_id_));
}

Cfg_Scene_MonsterGenerator_Row::~Cfg_Scene_MonsterGenerator_Row() {
  // @@protoc_insertion_point(destructor:Cfg_Scene_MonsterGenerator.Row)
  SharedDtor();
}

void Cfg_Scene_MonsterGenerator_Row::SharedDtor() {
}

void Cfg_Scene_MonsterGenerator_Row::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Cfg_Scene_MonsterGenerator_Row& Cfg_Scene_MonsterGenerator_Row::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Cfg_Scene_MonsterGenerator_Row_config_2fCfg_5fScene_5fMonsterGenerator_2eproto.base);
  return *internal_default_instance();
}


void Cfg_Scene_MonsterGenerator_Row::InternalSwap(Cfg_Scene_MonsterGenerator_Row* other) {
  using std::swap;
  GetReflection()->Swap(this, other);}

::PROTOBUF_NAMESPACE_ID::Metadata Cfg_Scene_MonsterGenerator_Row::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void Cfg_Scene_MonsterGenerator::InitAsDefaultInstance() {
}
class Cfg_Scene_MonsterGenerator::_Internal {
 public:
};

Cfg_Scene_MonsterGenerator::Cfg_Scene_MonsterGenerator()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Cfg_Scene_MonsterGenerator)
}
Cfg_Scene_MonsterGenerator::Cfg_Scene_MonsterGenerator(const Cfg_Scene_MonsterGenerator& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      rows_(from.rows_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:Cfg_Scene_MonsterGenerator)
}

void Cfg_Scene_MonsterGenerator::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_Cfg_Scene_MonsterGenerator_config_2fCfg_5fScene_5fMonsterGenerator_2eproto.base);
}

Cfg_Scene_MonsterGenerator::~Cfg_Scene_MonsterGenerator() {
  // @@protoc_insertion_point(destructor:Cfg_Scene_MonsterGenerator)
  SharedDtor();
}

void Cfg_Scene_MonsterGenerator::SharedDtor() {
}

void Cfg_Scene_MonsterGenerator::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Cfg_Scene_MonsterGenerator& Cfg_Scene_MonsterGenerator::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Cfg_Scene_MonsterGenerator_config_2fCfg_5fScene_5fMonsterGenerator_2eproto.base);
  return *internal_default_instance();
}


void Cfg_Scene_MonsterGenerator::InternalSwap(Cfg_Scene_MonsterGenerator* other) {
  using std::swap;
  GetReflection()->Swap(this, other);}

::PROTOBUF_NAMESPACE_ID::Metadata Cfg_Scene_MonsterGenerator::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Cfg_Scene_MonsterGenerator_Row* Arena::CreateMaybeMessage< ::Cfg_Scene_MonsterGenerator_Row >(Arena* arena) {
  return Arena::CreateInternal< ::Cfg_Scene_MonsterGenerator_Row >(arena);
}
template<> PROTOBUF_NOINLINE ::Cfg_Scene_MonsterGenerator* Arena::CreateMaybeMessage< ::Cfg_Scene_MonsterGenerator >(Arena* arena) {
  return Arena::CreateInternal< ::Cfg_Scene_MonsterGenerator >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
