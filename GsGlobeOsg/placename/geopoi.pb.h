// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: geopoi.proto

#ifndef PROTOBUF_geopoi_2eproto__INCLUDED
#define PROTOBUF_geopoi_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3004000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_util.h>
// @@protoc_insertion_point(includes)
namespace GEOPOI {
class PBPOI;
class PBPOIDefaultTypeInternal;
extern PBPOIDefaultTypeInternal _PBPOI_default_instance_;
class PBPOITile;
class PBPOITileDefaultTypeInternal;
extern PBPOITileDefaultTypeInternal _PBPOITile_default_instance_;
class StringTable;
class StringTableDefaultTypeInternal;
extern StringTableDefaultTypeInternal _StringTable_default_instance_;
}  // namespace GEOPOI

namespace GEOPOI {

namespace protobuf_geopoi_2eproto {
// Internal implementation detail -- do not call these.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[];
  static const ::google::protobuf::uint32 offsets[];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static void InitDefaultsImpl();
};
void AddDescriptors();
void InitDefaults();
}  // namespace protobuf_geopoi_2eproto

enum enumGeometryType {
  ePoint = 0,
  eMultiLineString = 1,
  ePolygon = 2
};
bool enumGeometryType_IsValid(int value);
const enumGeometryType enumGeometryType_MIN = ePoint;
const enumGeometryType enumGeometryType_MAX = ePolygon;
const int enumGeometryType_ARRAYSIZE = enumGeometryType_MAX + 1;

// ===================================================================

class PBPOI : public ::google::protobuf::MessageLite /* @@protoc_insertion_point(class_definition:GEOPOI.PBPOI) */ {
 public:
  PBPOI();
  virtual ~PBPOI();

  PBPOI(const PBPOI& from);

  inline PBPOI& operator=(const PBPOI& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  PBPOI(PBPOI&& from) noexcept
    : PBPOI() {
    *this = ::std::move(from);
  }

  inline PBPOI& operator=(PBPOI&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  inline const ::std::string& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }
  inline ::std::string* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const PBPOI& default_instance();

  static inline const PBPOI* internal_default_instance() {
    return reinterpret_cast<const PBPOI*>(
               &_PBPOI_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(PBPOI* other);
  friend void swap(PBPOI& a, PBPOI& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline PBPOI* New() const PROTOBUF_FINAL { return New(NULL); }

  PBPOI* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CheckTypeAndMergeFrom(const ::google::protobuf::MessageLite& from)
    PROTOBUF_FINAL;
  void CopyFrom(const PBPOI& from);
  void MergeFrom(const PBPOI& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  void DiscardUnknownFields();
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(PBPOI* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::std::string GetTypeName() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated double Coordinates = 3 [packed = true];
  int coordinates_size() const;
  void clear_coordinates();
  static const int kCoordinatesFieldNumber = 3;
  double coordinates(int index) const;
  void set_coordinates(int index, double value);
  void add_coordinates(double value);
  const ::google::protobuf::RepeatedField< double >&
      coordinates() const;
  ::google::protobuf::RepeatedField< double >*
      mutable_coordinates();

  // required string Name = 2;
  bool has_name() const;
  void clear_name();
  static const int kNameFieldNumber = 2;
  const ::std::string& name() const;
  void set_name(const ::std::string& value);
  #if LANG_CXX11
  void set_name(::std::string&& value);
  #endif
  void set_name(const char* value);
  void set_name(const char* value, size_t size);
  ::std::string* mutable_name();
  ::std::string* release_name();
  void set_allocated_name(::std::string* name);

  // required uint64 OID = 1;
  bool has_oid() const;
  void clear_oid();
  static const int kOIDFieldNumber = 1;
  ::google::protobuf::uint64 oid() const;
  void set_oid(::google::protobuf::uint64 value);

  // required .GEOPOI.enumGeometryType GeometryType = 4;
  bool has_geometrytype() const;
  void clear_geometrytype();
  static const int kGeometryTypeFieldNumber = 4;
  ::GEOPOI::enumGeometryType geometrytype() const;
  void set_geometrytype(::GEOPOI::enumGeometryType value);

  // optional int32 Interate = 5;
  bool has_interate() const;
  void clear_interate();
  static const int kInterateFieldNumber = 5;
  ::google::protobuf::int32 interate() const;
  void set_interate(::google::protobuf::int32 value);

  // optional int32 SymbolID = 10 [default = 0];
  bool has_symbolid() const;
  void clear_symbolid();
  static const int kSymbolIDFieldNumber = 10;
  ::google::protobuf::int32 symbolid() const;
  void set_symbolid(::google::protobuf::int32 value);

  // optional uint32 ShiningColor = 12 [default = 0];
  bool has_shiningcolor() const;
  void clear_shiningcolor();
  static const int kShiningColorFieldNumber = 12;
  ::google::protobuf::uint32 shiningcolor() const;
  void set_shiningcolor(::google::protobuf::uint32 value);

  // optional uint32 FontNameIndex = 13 [default = 0];
  bool has_fontnameindex() const;
  void clear_fontnameindex();
  static const int kFontNameIndexFieldNumber = 13;
  ::google::protobuf::uint32 fontnameindex() const;
  void set_fontnameindex(::google::protobuf::uint32 value);

  // optional uint32 FontColor = 15 [default = 0];
  bool has_fontcolor() const;
  void clear_fontcolor();
  static const int kFontColorFieldNumber = 15;
  ::google::protobuf::uint32 fontcolor() const;
  void set_fontcolor(::google::protobuf::uint32 value);

  // optional int32 FontSize = 14 [default = 18];
  bool has_fontsize() const;
  void clear_fontsize();
  static const int kFontSizeFieldNumber = 14;
  ::google::protobuf::int32 fontsize() const;
  void set_fontsize(::google::protobuf::int32 value);

  // optional double DisplayHeight = 11 [default = 32];
  bool has_displayheight() const;
  void clear_displayheight();
  static const int kDisplayHeightFieldNumber = 11;
  double displayheight() const;
  void set_displayheight(double value);

  // @@protoc_insertion_point(class_scope:GEOPOI.PBPOI)
 private:
  void set_has_oid();
  void clear_has_oid();
  void set_has_name();
  void clear_has_name();
  void set_has_geometrytype();
  void clear_has_geometrytype();
  void set_has_interate();
  void clear_has_interate();
  void set_has_symbolid();
  void clear_has_symbolid();
  void set_has_displayheight();
  void clear_has_displayheight();
  void set_has_shiningcolor();
  void clear_has_shiningcolor();
  void set_has_fontnameindex();
  void clear_has_fontnameindex();
  void set_has_fontsize();
  void clear_has_fontsize();
  void set_has_fontcolor();
  void clear_has_fontcolor();

  // helper for ByteSizeLong()
  size_t RequiredFieldsByteSizeFallback() const;

  ::google::protobuf::internal::InternalMetadataWithArenaLite _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::RepeatedField< double > coordinates_;
  mutable int _coordinates_cached_byte_size_;
  ::google::protobuf::internal::ArenaStringPtr name_;
  ::google::protobuf::uint64 oid_;
  int geometrytype_;
  ::google::protobuf::int32 interate_;
  ::google::protobuf::int32 symbolid_;
  ::google::protobuf::uint32 shiningcolor_;
  ::google::protobuf::uint32 fontnameindex_;
  ::google::protobuf::uint32 fontcolor_;
  ::google::protobuf::int32 fontsize_;
  double displayheight_;
  friend struct protobuf_geopoi_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class StringTable : public ::google::protobuf::MessageLite /* @@protoc_insertion_point(class_definition:GEOPOI.StringTable) */ {
 public:
  StringTable();
  virtual ~StringTable();

  StringTable(const StringTable& from);

  inline StringTable& operator=(const StringTable& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  StringTable(StringTable&& from) noexcept
    : StringTable() {
    *this = ::std::move(from);
  }

  inline StringTable& operator=(StringTable&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  inline const ::std::string& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }
  inline ::std::string* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const StringTable& default_instance();

  static inline const StringTable* internal_default_instance() {
    return reinterpret_cast<const StringTable*>(
               &_StringTable_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    1;

  void Swap(StringTable* other);
  friend void swap(StringTable& a, StringTable& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline StringTable* New() const PROTOBUF_FINAL { return New(NULL); }

  StringTable* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CheckTypeAndMergeFrom(const ::google::protobuf::MessageLite& from)
    PROTOBUF_FINAL;
  void CopyFrom(const StringTable& from);
  void MergeFrom(const StringTable& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  void DiscardUnknownFields();
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(StringTable* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::std::string GetTypeName() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated string s = 1;
  int s_size() const;
  void clear_s();
  static const int kSFieldNumber = 1;
  const ::std::string& s(int index) const;
  ::std::string* mutable_s(int index);
  void set_s(int index, const ::std::string& value);
  #if LANG_CXX11
  void set_s(int index, ::std::string&& value);
  #endif
  void set_s(int index, const char* value);
  void set_s(int index, const char* value, size_t size);
  ::std::string* add_s();
  void add_s(const ::std::string& value);
  #if LANG_CXX11
  void add_s(::std::string&& value);
  #endif
  void add_s(const char* value);
  void add_s(const char* value, size_t size);
  const ::google::protobuf::RepeatedPtrField< ::std::string>& s() const;
  ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_s();

  // @@protoc_insertion_point(class_scope:GEOPOI.StringTable)
 private:

  ::google::protobuf::internal::InternalMetadataWithArenaLite _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::RepeatedPtrField< ::std::string> s_;
  friend struct protobuf_geopoi_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class PBPOITile : public ::google::protobuf::MessageLite /* @@protoc_insertion_point(class_definition:GEOPOI.PBPOITile) */ {
 public:
  PBPOITile();
  virtual ~PBPOITile();

  PBPOITile(const PBPOITile& from);

  inline PBPOITile& operator=(const PBPOITile& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  PBPOITile(PBPOITile&& from) noexcept
    : PBPOITile() {
    *this = ::std::move(from);
  }

  inline PBPOITile& operator=(PBPOITile&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  inline const ::std::string& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }
  inline ::std::string* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const PBPOITile& default_instance();

  static inline const PBPOITile* internal_default_instance() {
    return reinterpret_cast<const PBPOITile*>(
               &_PBPOITile_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    2;

  void Swap(PBPOITile* other);
  friend void swap(PBPOITile& a, PBPOITile& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline PBPOITile* New() const PROTOBUF_FINAL { return New(NULL); }

  PBPOITile* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CheckTypeAndMergeFrom(const ::google::protobuf::MessageLite& from)
    PROTOBUF_FINAL;
  void CopyFrom(const PBPOITile& from);
  void MergeFrom(const PBPOITile& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  void DiscardUnknownFields();
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(PBPOITile* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::std::string GetTypeName() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .GEOPOI.PBPOI POIS = 4;
  int pois_size() const;
  void clear_pois();
  static const int kPOISFieldNumber = 4;
  const ::GEOPOI::PBPOI& pois(int index) const;
  ::GEOPOI::PBPOI* mutable_pois(int index);
  ::GEOPOI::PBPOI* add_pois();
  ::google::protobuf::RepeatedPtrField< ::GEOPOI::PBPOI >*
      mutable_pois();
  const ::google::protobuf::RepeatedPtrField< ::GEOPOI::PBPOI >&
      pois() const;

  // required .GEOPOI.StringTable StringTable = 3;
  bool has_stringtable() const;
  void clear_stringtable();
  static const int kStringTableFieldNumber = 3;
  const ::GEOPOI::StringTable& stringtable() const;
  ::GEOPOI::StringTable* mutable_stringtable();
  ::GEOPOI::StringTable* release_stringtable();
  void set_allocated_stringtable(::GEOPOI::StringTable* stringtable);

  // required int64 Version = 1;
  bool has_version() const;
  void clear_version();
  static const int kVersionFieldNumber = 1;
  ::google::protobuf::int64 version() const;
  void set_version(::google::protobuf::int64 value);

  // required int64 TileKey = 2;
  bool has_tilekey() const;
  void clear_tilekey();
  static const int kTileKeyFieldNumber = 2;
  ::google::protobuf::int64 tilekey() const;
  void set_tilekey(::google::protobuf::int64 value);

  // @@protoc_insertion_point(class_scope:GEOPOI.PBPOITile)
 private:
  void set_has_version();
  void clear_has_version();
  void set_has_tilekey();
  void clear_has_tilekey();
  void set_has_stringtable();
  void clear_has_stringtable();

  // helper for ByteSizeLong()
  size_t RequiredFieldsByteSizeFallback() const;

  ::google::protobuf::internal::InternalMetadataWithArenaLite _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::RepeatedPtrField< ::GEOPOI::PBPOI > pois_;
  ::GEOPOI::StringTable* stringtable_;
  ::google::protobuf::int64 version_;
  ::google::protobuf::int64 tilekey_;
  friend struct protobuf_geopoi_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// PBPOI

// required uint64 OID = 1;
inline bool PBPOI::has_oid() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void PBPOI::set_has_oid() {
  _has_bits_[0] |= 0x00000002u;
}
inline void PBPOI::clear_has_oid() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void PBPOI::clear_oid() {
  oid_ = GOOGLE_ULONGLONG(0);
  clear_has_oid();
}
inline ::google::protobuf::uint64 PBPOI::oid() const {
  // @@protoc_insertion_point(field_get:GEOPOI.PBPOI.OID)
  return oid_;
}
inline void PBPOI::set_oid(::google::protobuf::uint64 value) {
  set_has_oid();
  oid_ = value;
  // @@protoc_insertion_point(field_set:GEOPOI.PBPOI.OID)
}

// required string Name = 2;
inline bool PBPOI::has_name() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void PBPOI::set_has_name() {
  _has_bits_[0] |= 0x00000001u;
}
inline void PBPOI::clear_has_name() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void PBPOI::clear_name() {
  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_name();
}
inline const ::std::string& PBPOI::name() const {
  // @@protoc_insertion_point(field_get:GEOPOI.PBPOI.Name)
  return name_.GetNoArena();
}
inline void PBPOI::set_name(const ::std::string& value) {
  set_has_name();
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:GEOPOI.PBPOI.Name)
}
#if LANG_CXX11
inline void PBPOI::set_name(::std::string&& value) {
  set_has_name();
  name_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:GEOPOI.PBPOI.Name)
}
#endif
inline void PBPOI::set_name(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  set_has_name();
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:GEOPOI.PBPOI.Name)
}
inline void PBPOI::set_name(const char* value, size_t size) {
  set_has_name();
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:GEOPOI.PBPOI.Name)
}
inline ::std::string* PBPOI::mutable_name() {
  set_has_name();
  // @@protoc_insertion_point(field_mutable:GEOPOI.PBPOI.Name)
  return name_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* PBPOI::release_name() {
  // @@protoc_insertion_point(field_release:GEOPOI.PBPOI.Name)
  clear_has_name();
  return name_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void PBPOI::set_allocated_name(::std::string* name) {
  if (name != NULL) {
    set_has_name();
  } else {
    clear_has_name();
  }
  name_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), name);
  // @@protoc_insertion_point(field_set_allocated:GEOPOI.PBPOI.Name)
}

// repeated double Coordinates = 3 [packed = true];
inline int PBPOI::coordinates_size() const {
  return coordinates_.size();
}
inline void PBPOI::clear_coordinates() {
  coordinates_.Clear();
}
inline double PBPOI::coordinates(int index) const {
  // @@protoc_insertion_point(field_get:GEOPOI.PBPOI.Coordinates)
  return coordinates_.Get(index);
}
inline void PBPOI::set_coordinates(int index, double value) {
  coordinates_.Set(index, value);
  // @@protoc_insertion_point(field_set:GEOPOI.PBPOI.Coordinates)
}
inline void PBPOI::add_coordinates(double value) {
  coordinates_.Add(value);
  // @@protoc_insertion_point(field_add:GEOPOI.PBPOI.Coordinates)
}
inline const ::google::protobuf::RepeatedField< double >&
PBPOI::coordinates() const {
  // @@protoc_insertion_point(field_list:GEOPOI.PBPOI.Coordinates)
  return coordinates_;
}
inline ::google::protobuf::RepeatedField< double >*
PBPOI::mutable_coordinates() {
  // @@protoc_insertion_point(field_mutable_list:GEOPOI.PBPOI.Coordinates)
  return &coordinates_;
}

// required .GEOPOI.enumGeometryType GeometryType = 4;
inline bool PBPOI::has_geometrytype() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void PBPOI::set_has_geometrytype() {
  _has_bits_[0] |= 0x00000004u;
}
inline void PBPOI::clear_has_geometrytype() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void PBPOI::clear_geometrytype() {
  geometrytype_ = 0;
  clear_has_geometrytype();
}
inline ::GEOPOI::enumGeometryType PBPOI::geometrytype() const {
  // @@protoc_insertion_point(field_get:GEOPOI.PBPOI.GeometryType)
  return static_cast< ::GEOPOI::enumGeometryType >(geometrytype_);
}
inline void PBPOI::set_geometrytype(::GEOPOI::enumGeometryType value) {
  assert(::GEOPOI::enumGeometryType_IsValid(value));
  set_has_geometrytype();
  geometrytype_ = value;
  // @@protoc_insertion_point(field_set:GEOPOI.PBPOI.GeometryType)
}

// optional int32 Interate = 5;
inline bool PBPOI::has_interate() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void PBPOI::set_has_interate() {
  _has_bits_[0] |= 0x00000008u;
}
inline void PBPOI::clear_has_interate() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void PBPOI::clear_interate() {
  interate_ = 0;
  clear_has_interate();
}
inline ::google::protobuf::int32 PBPOI::interate() const {
  // @@protoc_insertion_point(field_get:GEOPOI.PBPOI.Interate)
  return interate_;
}
inline void PBPOI::set_interate(::google::protobuf::int32 value) {
  set_has_interate();
  interate_ = value;
  // @@protoc_insertion_point(field_set:GEOPOI.PBPOI.Interate)
}

// optional int32 SymbolID = 10 [default = 0];
inline bool PBPOI::has_symbolid() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void PBPOI::set_has_symbolid() {
  _has_bits_[0] |= 0x00000010u;
}
inline void PBPOI::clear_has_symbolid() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void PBPOI::clear_symbolid() {
  symbolid_ = 0;
  clear_has_symbolid();
}
inline ::google::protobuf::int32 PBPOI::symbolid() const {
  // @@protoc_insertion_point(field_get:GEOPOI.PBPOI.SymbolID)
  return symbolid_;
}
inline void PBPOI::set_symbolid(::google::protobuf::int32 value) {
  set_has_symbolid();
  symbolid_ = value;
  // @@protoc_insertion_point(field_set:GEOPOI.PBPOI.SymbolID)
}

// optional double DisplayHeight = 11 [default = 32];
inline bool PBPOI::has_displayheight() const {
  return (_has_bits_[0] & 0x00000200u) != 0;
}
inline void PBPOI::set_has_displayheight() {
  _has_bits_[0] |= 0x00000200u;
}
inline void PBPOI::clear_has_displayheight() {
  _has_bits_[0] &= ~0x00000200u;
}
inline void PBPOI::clear_displayheight() {
  displayheight_ = 32;
  clear_has_displayheight();
}
inline double PBPOI::displayheight() const {
  // @@protoc_insertion_point(field_get:GEOPOI.PBPOI.DisplayHeight)
  return displayheight_;
}
inline void PBPOI::set_displayheight(double value) {
  set_has_displayheight();
  displayheight_ = value;
  // @@protoc_insertion_point(field_set:GEOPOI.PBPOI.DisplayHeight)
}

// optional uint32 ShiningColor = 12 [default = 0];
inline bool PBPOI::has_shiningcolor() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void PBPOI::set_has_shiningcolor() {
  _has_bits_[0] |= 0x00000020u;
}
inline void PBPOI::clear_has_shiningcolor() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void PBPOI::clear_shiningcolor() {
  shiningcolor_ = 0u;
  clear_has_shiningcolor();
}
inline ::google::protobuf::uint32 PBPOI::shiningcolor() const {
  // @@protoc_insertion_point(field_get:GEOPOI.PBPOI.ShiningColor)
  return shiningcolor_;
}
inline void PBPOI::set_shiningcolor(::google::protobuf::uint32 value) {
  set_has_shiningcolor();
  shiningcolor_ = value;
  // @@protoc_insertion_point(field_set:GEOPOI.PBPOI.ShiningColor)
}

// optional uint32 FontNameIndex = 13 [default = 0];
inline bool PBPOI::has_fontnameindex() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void PBPOI::set_has_fontnameindex() {
  _has_bits_[0] |= 0x00000040u;
}
inline void PBPOI::clear_has_fontnameindex() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void PBPOI::clear_fontnameindex() {
  fontnameindex_ = 0u;
  clear_has_fontnameindex();
}
inline ::google::protobuf::uint32 PBPOI::fontnameindex() const {
  // @@protoc_insertion_point(field_get:GEOPOI.PBPOI.FontNameIndex)
  return fontnameindex_;
}
inline void PBPOI::set_fontnameindex(::google::protobuf::uint32 value) {
  set_has_fontnameindex();
  fontnameindex_ = value;
  // @@protoc_insertion_point(field_set:GEOPOI.PBPOI.FontNameIndex)
}

// optional int32 FontSize = 14 [default = 18];
inline bool PBPOI::has_fontsize() const {
  return (_has_bits_[0] & 0x00000100u) != 0;
}
inline void PBPOI::set_has_fontsize() {
  _has_bits_[0] |= 0x00000100u;
}
inline void PBPOI::clear_has_fontsize() {
  _has_bits_[0] &= ~0x00000100u;
}
inline void PBPOI::clear_fontsize() {
  fontsize_ = 18;
  clear_has_fontsize();
}
inline ::google::protobuf::int32 PBPOI::fontsize() const {
  // @@protoc_insertion_point(field_get:GEOPOI.PBPOI.FontSize)
  return fontsize_;
}
inline void PBPOI::set_fontsize(::google::protobuf::int32 value) {
  set_has_fontsize();
  fontsize_ = value;
  // @@protoc_insertion_point(field_set:GEOPOI.PBPOI.FontSize)
}

// optional uint32 FontColor = 15 [default = 0];
inline bool PBPOI::has_fontcolor() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void PBPOI::set_has_fontcolor() {
  _has_bits_[0] |= 0x00000080u;
}
inline void PBPOI::clear_has_fontcolor() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void PBPOI::clear_fontcolor() {
  fontcolor_ = 0u;
  clear_has_fontcolor();
}
inline ::google::protobuf::uint32 PBPOI::fontcolor() const {
  // @@protoc_insertion_point(field_get:GEOPOI.PBPOI.FontColor)
  return fontcolor_;
}
inline void PBPOI::set_fontcolor(::google::protobuf::uint32 value) {
  set_has_fontcolor();
  fontcolor_ = value;
  // @@protoc_insertion_point(field_set:GEOPOI.PBPOI.FontColor)
}

// -------------------------------------------------------------------

// StringTable

// repeated string s = 1;
inline int StringTable::s_size() const {
  return s_.size();
}
inline void StringTable::clear_s() {
  s_.Clear();
}
inline const ::std::string& StringTable::s(int index) const {
  // @@protoc_insertion_point(field_get:GEOPOI.StringTable.s)
  return s_.Get(index);
}
inline ::std::string* StringTable::mutable_s(int index) {
  // @@protoc_insertion_point(field_mutable:GEOPOI.StringTable.s)
  return s_.Mutable(index);
}
inline void StringTable::set_s(int index, const ::std::string& value) {
  // @@protoc_insertion_point(field_set:GEOPOI.StringTable.s)
  s_.Mutable(index)->assign(value);
}
#if LANG_CXX11
inline void StringTable::set_s(int index, ::std::string&& value) {
  // @@protoc_insertion_point(field_set:GEOPOI.StringTable.s)
  s_.Mutable(index)->assign(std::move(value));
}
#endif
inline void StringTable::set_s(int index, const char* value) {
  GOOGLE_DCHECK(value != NULL);
  s_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set_char:GEOPOI.StringTable.s)
}
inline void StringTable::set_s(int index, const char* value, size_t size) {
  s_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:GEOPOI.StringTable.s)
}
inline ::std::string* StringTable::add_s() {
  // @@protoc_insertion_point(field_add_mutable:GEOPOI.StringTable.s)
  return s_.Add();
}
inline void StringTable::add_s(const ::std::string& value) {
  s_.Add()->assign(value);
  // @@protoc_insertion_point(field_add:GEOPOI.StringTable.s)
}
#if LANG_CXX11
inline void StringTable::add_s(::std::string&& value) {
  s_.Add(std::move(value));
  // @@protoc_insertion_point(field_add:GEOPOI.StringTable.s)
}
#endif
inline void StringTable::add_s(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  s_.Add()->assign(value);
  // @@protoc_insertion_point(field_add_char:GEOPOI.StringTable.s)
}
inline void StringTable::add_s(const char* value, size_t size) {
  s_.Add()->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_add_pointer:GEOPOI.StringTable.s)
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
StringTable::s() const {
  // @@protoc_insertion_point(field_list:GEOPOI.StringTable.s)
  return s_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
StringTable::mutable_s() {
  // @@protoc_insertion_point(field_mutable_list:GEOPOI.StringTable.s)
  return &s_;
}

// -------------------------------------------------------------------

// PBPOITile

// required int64 Version = 1;
inline bool PBPOITile::has_version() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void PBPOITile::set_has_version() {
  _has_bits_[0] |= 0x00000002u;
}
inline void PBPOITile::clear_has_version() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void PBPOITile::clear_version() {
  version_ = GOOGLE_LONGLONG(0);
  clear_has_version();
}
inline ::google::protobuf::int64 PBPOITile::version() const {
  // @@protoc_insertion_point(field_get:GEOPOI.PBPOITile.Version)
  return version_;
}
inline void PBPOITile::set_version(::google::protobuf::int64 value) {
  set_has_version();
  version_ = value;
  // @@protoc_insertion_point(field_set:GEOPOI.PBPOITile.Version)
}

// required int64 TileKey = 2;
inline bool PBPOITile::has_tilekey() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void PBPOITile::set_has_tilekey() {
  _has_bits_[0] |= 0x00000004u;
}
inline void PBPOITile::clear_has_tilekey() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void PBPOITile::clear_tilekey() {
  tilekey_ = GOOGLE_LONGLONG(0);
  clear_has_tilekey();
}
inline ::google::protobuf::int64 PBPOITile::tilekey() const {
  // @@protoc_insertion_point(field_get:GEOPOI.PBPOITile.TileKey)
  return tilekey_;
}
inline void PBPOITile::set_tilekey(::google::protobuf::int64 value) {
  set_has_tilekey();
  tilekey_ = value;
  // @@protoc_insertion_point(field_set:GEOPOI.PBPOITile.TileKey)
}

// required .GEOPOI.StringTable StringTable = 3;
inline bool PBPOITile::has_stringtable() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void PBPOITile::set_has_stringtable() {
  _has_bits_[0] |= 0x00000001u;
}
inline void PBPOITile::clear_has_stringtable() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void PBPOITile::clear_stringtable() {
  if (stringtable_ != NULL) stringtable_->::GEOPOI::StringTable::Clear();
  clear_has_stringtable();
}
inline const ::GEOPOI::StringTable& PBPOITile::stringtable() const {
  const ::GEOPOI::StringTable* p = stringtable_;
  // @@protoc_insertion_point(field_get:GEOPOI.PBPOITile.StringTable)
  return p != NULL ? *p : *reinterpret_cast<const ::GEOPOI::StringTable*>(
      &::GEOPOI::_StringTable_default_instance_);
}
inline ::GEOPOI::StringTable* PBPOITile::mutable_stringtable() {
  set_has_stringtable();
  if (stringtable_ == NULL) {
    stringtable_ = new ::GEOPOI::StringTable;
  }
  // @@protoc_insertion_point(field_mutable:GEOPOI.PBPOITile.StringTable)
  return stringtable_;
}
inline ::GEOPOI::StringTable* PBPOITile::release_stringtable() {
  // @@protoc_insertion_point(field_release:GEOPOI.PBPOITile.StringTable)
  clear_has_stringtable();
  ::GEOPOI::StringTable* temp = stringtable_;
  stringtable_ = NULL;
  return temp;
}
inline void PBPOITile::set_allocated_stringtable(::GEOPOI::StringTable* stringtable) {
  delete stringtable_;
  stringtable_ = stringtable;
  if (stringtable) {
    set_has_stringtable();
  } else {
    clear_has_stringtable();
  }
  // @@protoc_insertion_point(field_set_allocated:GEOPOI.PBPOITile.StringTable)
}

// repeated .GEOPOI.PBPOI POIS = 4;
inline int PBPOITile::pois_size() const {
  return pois_.size();
}
inline void PBPOITile::clear_pois() {
  pois_.Clear();
}
inline const ::GEOPOI::PBPOI& PBPOITile::pois(int index) const {
  // @@protoc_insertion_point(field_get:GEOPOI.PBPOITile.POIS)
  return pois_.Get(index);
}
inline ::GEOPOI::PBPOI* PBPOITile::mutable_pois(int index) {
  // @@protoc_insertion_point(field_mutable:GEOPOI.PBPOITile.POIS)
  return pois_.Mutable(index);
}
inline ::GEOPOI::PBPOI* PBPOITile::add_pois() {
  // @@protoc_insertion_point(field_add:GEOPOI.PBPOITile.POIS)
  return pois_.Add();
}
inline ::google::protobuf::RepeatedPtrField< ::GEOPOI::PBPOI >*
PBPOITile::mutable_pois() {
  // @@protoc_insertion_point(field_mutable_list:GEOPOI.PBPOITile.POIS)
  return &pois_;
}
inline const ::google::protobuf::RepeatedPtrField< ::GEOPOI::PBPOI >&
PBPOITile::pois() const {
  // @@protoc_insertion_point(field_list:GEOPOI.PBPOITile.POIS)
  return pois_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


}  // namespace GEOPOI

namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::GEOPOI::enumGeometryType> : ::google::protobuf::internal::true_type {};

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_geopoi_2eproto__INCLUDED
