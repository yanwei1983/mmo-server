// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: config/Cfg_SkillFAM.proto

#include "config/Cfg_SkillFAM.pb.h"

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
extern PROTOBUF_INTERNAL_EXPORT_config_2fCfg_5fSkillFAM_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_Cfg_SkillFAM_Row_config_2fCfg_5fSkillFAM_2eproto;
class Cfg_SkillFAM_RowDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Cfg_SkillFAM_Row> _instance;
} _Cfg_SkillFAM_Row_default_instance_;
class Cfg_SkillFAMDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Cfg_SkillFAM> _instance;
} _Cfg_SkillFAM_default_instance_;
static void InitDefaultsscc_info_Cfg_SkillFAM_config_2fCfg_5fSkillFAM_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_Cfg_SkillFAM_default_instance_;
    new (ptr) ::Cfg_SkillFAM();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Cfg_SkillFAM::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_Cfg_SkillFAM_config_2fCfg_5fSkillFAM_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_Cfg_SkillFAM_config_2fCfg_5fSkillFAM_2eproto}, {
      &scc_info_Cfg_SkillFAM_Row_config_2fCfg_5fSkillFAM_2eproto.base,}};

static void InitDefaultsscc_info_Cfg_SkillFAM_Row_config_2fCfg_5fSkillFAM_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_Cfg_SkillFAM_Row_default_instance_;
    new (ptr) ::Cfg_SkillFAM_Row();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Cfg_SkillFAM_Row::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_Cfg_SkillFAM_Row_config_2fCfg_5fSkillFAM_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_Cfg_SkillFAM_Row_config_2fCfg_5fSkillFAM_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_config_2fCfg_5fSkillFAM_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_config_2fCfg_5fSkillFAM_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_config_2fCfg_5fSkillFAM_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_config_2fCfg_5fSkillFAM_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Cfg_SkillFAM_Row, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Cfg_SkillFAM_Row, idskill_),
  PROTOBUF_FIELD_OFFSET(::Cfg_SkillFAM_Row, dis_),
  PROTOBUF_FIELD_OFFSET(::Cfg_SkillFAM_Row, self_hp_),
  PROTOBUF_FIELD_OFFSET(::Cfg_SkillFAM_Row, self_mp_),
  PROTOBUF_FIELD_OFFSET(::Cfg_SkillFAM_Row, target_hp_),
  PROTOBUF_FIELD_OFFSET(::Cfg_SkillFAM_Row, oper_type_),
  PROTOBUF_FIELD_OFFSET(::Cfg_SkillFAM_Row, like_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Cfg_SkillFAM, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Cfg_SkillFAM, rows_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::Cfg_SkillFAM_Row)},
  { 12, -1, sizeof(::Cfg_SkillFAM)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_Cfg_SkillFAM_Row_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_Cfg_SkillFAM_default_instance_),
};

const char descriptor_table_protodef_config_2fCfg_5fSkillFAM_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\031config/Cfg_SkillFAM.proto\"\252\001\n\014Cfg_Skil"
  "lFAM\022\037\n\004rows\030\001 \003(\0132\021.Cfg_SkillFAM.Row\032y\n"
  "\003Row\022\017\n\007idskill\030\001 \001(\r\022\013\n\003dis\030\002 \001(\r\022\017\n\007se"
  "lf_hp\030\003 \001(\r\022\017\n\007self_mp\030\004 \001(\r\022\021\n\ttarget_h"
  "p\030\005 \001(\r\022\021\n\toper_type\030\006 \001(\r\022\014\n\004like\030\007 \001(\r"
  "B\002H\002b\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_config_2fCfg_5fSkillFAM_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_config_2fCfg_5fSkillFAM_2eproto_sccs[2] = {
  &scc_info_Cfg_SkillFAM_config_2fCfg_5fSkillFAM_2eproto.base,
  &scc_info_Cfg_SkillFAM_Row_config_2fCfg_5fSkillFAM_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_config_2fCfg_5fSkillFAM_2eproto_once;
static bool descriptor_table_config_2fCfg_5fSkillFAM_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_config_2fCfg_5fSkillFAM_2eproto = {
  &descriptor_table_config_2fCfg_5fSkillFAM_2eproto_initialized, descriptor_table_protodef_config_2fCfg_5fSkillFAM_2eproto, "config/Cfg_SkillFAM.proto", 212,
  &descriptor_table_config_2fCfg_5fSkillFAM_2eproto_once, descriptor_table_config_2fCfg_5fSkillFAM_2eproto_sccs, descriptor_table_config_2fCfg_5fSkillFAM_2eproto_deps, 2, 0,
  schemas, file_default_instances, TableStruct_config_2fCfg_5fSkillFAM_2eproto::offsets,
  file_level_metadata_config_2fCfg_5fSkillFAM_2eproto, 2, file_level_enum_descriptors_config_2fCfg_5fSkillFAM_2eproto, file_level_service_descriptors_config_2fCfg_5fSkillFAM_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_config_2fCfg_5fSkillFAM_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_config_2fCfg_5fSkillFAM_2eproto), true);

// ===================================================================

void Cfg_SkillFAM_Row::InitAsDefaultInstance() {
}
class Cfg_SkillFAM_Row::_Internal {
 public:
};

Cfg_SkillFAM_Row::Cfg_SkillFAM_Row()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Cfg_SkillFAM.Row)
}
Cfg_SkillFAM_Row::Cfg_SkillFAM_Row(const Cfg_SkillFAM_Row& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::memcpy(&idskill_, &from.idskill_,
    static_cast<size_t>(reinterpret_cast<char*>(&like_) -
    reinterpret_cast<char*>(&idskill_)) + sizeof(like_));
  // @@protoc_insertion_point(copy_constructor:Cfg_SkillFAM.Row)
}

void Cfg_SkillFAM_Row::SharedCtor() {
  ::memset(&idskill_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&like_) -
      reinterpret_cast<char*>(&idskill_)) + sizeof(like_));
}

Cfg_SkillFAM_Row::~Cfg_SkillFAM_Row() {
  // @@protoc_insertion_point(destructor:Cfg_SkillFAM.Row)
  SharedDtor();
}

void Cfg_SkillFAM_Row::SharedDtor() {
}

void Cfg_SkillFAM_Row::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Cfg_SkillFAM_Row& Cfg_SkillFAM_Row::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Cfg_SkillFAM_Row_config_2fCfg_5fSkillFAM_2eproto.base);
  return *internal_default_instance();
}


void Cfg_SkillFAM_Row::InternalSwap(Cfg_SkillFAM_Row* other) {
  using std::swap;
  GetReflection()->Swap(this, other);}

::PROTOBUF_NAMESPACE_ID::Metadata Cfg_SkillFAM_Row::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void Cfg_SkillFAM::InitAsDefaultInstance() {
}
class Cfg_SkillFAM::_Internal {
 public:
};

Cfg_SkillFAM::Cfg_SkillFAM()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Cfg_SkillFAM)
}
Cfg_SkillFAM::Cfg_SkillFAM(const Cfg_SkillFAM& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      rows_(from.rows_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:Cfg_SkillFAM)
}

void Cfg_SkillFAM::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_Cfg_SkillFAM_config_2fCfg_5fSkillFAM_2eproto.base);
}

Cfg_SkillFAM::~Cfg_SkillFAM() {
  // @@protoc_insertion_point(destructor:Cfg_SkillFAM)
  SharedDtor();
}

void Cfg_SkillFAM::SharedDtor() {
}

void Cfg_SkillFAM::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Cfg_SkillFAM& Cfg_SkillFAM::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Cfg_SkillFAM_config_2fCfg_5fSkillFAM_2eproto.base);
  return *internal_default_instance();
}


void Cfg_SkillFAM::InternalSwap(Cfg_SkillFAM* other) {
  using std::swap;
  GetReflection()->Swap(this, other);}

::PROTOBUF_NAMESPACE_ID::Metadata Cfg_SkillFAM::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Cfg_SkillFAM_Row* Arena::CreateMaybeMessage< ::Cfg_SkillFAM_Row >(Arena* arena) {
  return Arena::CreateInternal< ::Cfg_SkillFAM_Row >(arena);
}
template<> PROTOBUF_NOINLINE ::Cfg_SkillFAM* Arena::CreateMaybeMessage< ::Cfg_SkillFAM >(Arena* arena) {
  return Arena::CreateInternal< ::Cfg_SkillFAM >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
