#pragma once

#include <RE/Skyrim.h>
#include <REL/Relocation.h>
#include <REX/REX/Singleton.h>
#include <SKSE/SKSE.h>
#include <half/half.h>
#include <wrl/client.h>

#include <magic_enum/magic_enum.hpp>

#include "Macros.h"
#include "STL.h"

// Definitions
namespace logger = SKSE::log;
namespace fs = std::filesystem;

template <class T>
using Singleton = REX::Singleton<T>;
using Microsoft::WRL::ComPtr;

template <typename T>
using opt = std::optional<T>;
using IStreamPtr = std::unique_ptr<std::istream>;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;
using half = FLOAT16;

typedef u16 FileID;
typedef u32 UniqueID;

#include "MaterialFunctionID.h"

enum class ColorBlendMode : u8;
enum class MaterialFlags : u64;
enum class MaterialOpCode : u8;
struct opt_bool;
struct uv;
struct MaterialCondition;
struct MATR;
struct MATC;
using MaterialConditionParam =
    std::optional<std::variant<std::string, bool, double, int>>;

// Implementations

struct _NODISCARD opt_bool {
  constexpr opt_bool() = default;
  constexpr opt_bool(bool value) : value_(value ? kTrue : kFalse) {}

  constexpr bool has_value() const { return value_ != kNone; }

  constexpr bool value() const { return static_cast<bool>(value_); }

  constexpr bool value_or(bool other) const {
    return value_ == kNone ? other : value();
  }

  constexpr explicit operator bool() const noexcept { return value_ != kNone; }

  constexpr bool operator==(const opt_bool& rhs) const {
    return value_ == rhs.value_;
  }

  constexpr bool operator==(const bool rhs) const {
    return value_ != kNone && value() == rhs;
  }

 private:
  enum tribool : u8 {
    kFalse = FALSE,
    kTrue = TRUE,
    kNone = 2,
  };
  tribool value_{kNone};
};

struct _NODISCARD uv {
  uv(const half& scaleX, const half& scaleY, const half& offsetX,
     const half& offsetY)
      : data_(scaleX, scaleY, offsetX, offsetY) {}

  uv(const std::array<half, 2>& scale, const std::array<half, 2>& offset)
      : data_(scale[0], scale[1], offset[0], offset[1]) {}

  auto scale() const -> RE::NiPoint2 {
    return {half::ToFloat32Fast(data_[0]), half::ToFloat32Fast(data_[1])};
  }

  auto set_scale(const half& x, const half& y) {
    data_[0] = x;
    data_[1] = y;
  }

  auto offset() const -> RE::NiPoint2 {
    return {half::ToFloat32Fast(data_[2]), half::ToFloat32Fast(data_[3])};
  }

  auto set_offset(const half& x, const half& y) {
    data_[2] = x;
    data_[3] = y;
  }

 private:
  half data_[4];
};

enum class ColorBlendMode : u8 {
  Normal,
  Add,
  Multiply,
  Screen,
  Overlay,
  Darken,
  Lighten,
  ColorDodge,
  ColorBurn,
  HardLight,
  SoftLight,
};

enum class MaterialFlags : u64 {
  None = 0,
  Decal = ENUMI(0),
  TwoSided = ENUMI(1),
  EnvMapEnabled = ENUMI(2),
  SpecularEnabled = ENUMI(3),
  ReceiveShadows = ENUMI(4),
  CastShadows = ENUMI(5),
  Facegen = ENUMI(6),
  Hair = ENUMI(7),
  DecalNoFade = ENUMI(8),
  NonOccluder = ENUMI(9),
  Refraction = ENUMI(10),
  RefractionFalloff = ENUMI(11),
  GrayscaleToPaletteColor = ENUMI(12),
  DepthWrite = ENUMI(13),
  SubsurfaceLighting = ENUMI(14),
  EnvMapEye = ENUMI(15),
  EmitEnabled = ENUMI(16),
  Pbr = ENUMI(17),
  BackLighting = ENUMI(18),
  HideSecret = ENUMI(19),
  DissolveFade = ENUMI(20),
  AssumeShadowMask = ENUMI(21),
  GlowMapEnabled = ENUMI(22),
  EnableEditorAlphaThreshold = ENUMI(23),
  Translucency = ENUMI(24),
  TranslucencyThickObject = ENUMI(25),
  TranslucencyMixAlbedoWithSubsurfaceColor = ENUMI(26),
  RimLighting = ENUMI(27),
  AlphaBlend = ENUMI(28),
  AlphaTest = ENUMI(29),
  DepthBias = ENUMI(30),
  CustomPorosity = ENUMI(31),
  AnisoLighting = ENUMI(32),
  ModelSpaceNormals = ENUMI(33),
  ExternalEmit = ENUMI(34),
  UseAdaptiveEmissive = ENUMI(35),
  DepthTest = ENUMI(36),
  Invisible = ENUMI(37),

  MAX = ENUMI(38),
};

struct MaterialCondition {
  MaterialFunctionID function;
  std::string variable;
  RE::CONDITION_ITEM_DATA::OpCode op;
  MaterialConditionParam param;
};

struct MATR {
  FileID id;
  opt_bitset<37> flags;

  FileID inherits{0};

  // textures
  FileID diffuseMap{0};
  FileID normalMap{0};
  FileID smoothSpecMap{0};
  FileID grayscaleMap{0};
  FileID glowMap{0};
  FileID wrinkleMap{0};
  FileID specularMap{0};
  FileID lightingMap{0};
  FileID flowMap{0};
  FileID distanceFieldAlphaMap{0};
  FileID envMap{0};
  FileID innerLayerMap{0};
  FileID displacementMap{0};
  FileID baseMap{0};
  FileID envMapMask{0};
  FileID faceTintMap{0};
  FileID detailMap{0};
  FileID subsurfaceMap{0};
  FileID parallaxMap{0};
  FileID rmaosMap{0};
  FileID coatMap{0};
  FileID coatNormalMap{0};
  FileID fuzzMap{0};
  FileID emissionMap{0};
  FileID colorBlendMap{0};

  opt<u8> clamp;
  opt<uv> uv;
  opt<half> transparency;
  opt<u8> sourceBlendMode;
  opt<u8> destinationBlendMode;
  opt<u8> alphaTestThreshold;
  opt<u8> maskWrites;
  opt<half> refractionPower;
  opt<half> envMapMaskScale;
  opt<half> rimPower;
  opt<half> backLightPower;
  opt<half> specularPower;
  opt<half> subsurfaceLightingRolloff;
  opt<std::array<u8, 3>> specularColor;
  opt<half> specularMult;
  opt<half> smoothness;
  opt<half> fresnelPower;
  opt<std::array<u8, 3>> emitColor;
  opt<half> emitMult;

  // custom fields
  opt<ColorBlendMode> colorBlendMode;
  opt<std::array<u8, 4>> colorChannelR;
  opt<std::array<u8, 4>> colorChannelG;
  opt<std::array<u8, 4>> colorChannelB;

  bool operator==(const MATR& rhs) const { return id != NULL && rhs.id == id; }
  bool operator!=(const MATR& rhs) const { return id != NULL && rhs.id != id; }
};

struct LegacyMaterialCondition {
  std::string type;
  f32 value;
};

struct JsonMATCRecord {
  std::string name;
  u8 layer = 0;
  u8 gender = RE::SEXES::kTotal;
  bool isHidden = false;
  bool modifyName = true;
  std::unordered_map<std::string, std::string> applies{};
  std::vector<LegacyMaterialCondition> conditions{}; // LEGACY
  std::vector<std::string> filters;
};

struct MATC {
  RE::FormID form;
  std::string name;
  std::vector<std::tuple<std::string, FileID>> applies;
  std::vector<std::string> keywords;
  std::vector<MaterialCondition> conditions;
  u8 layer;
  u8 gender;
  bool isHidden = false;
  bool modifyName = true;

  _NODISCARD auto find_material(const std::string& shape) const
      -> std::optional<uint16_t> {
    for (const auto [key, value] : applies) {
      if (key == shape) return std::optional(value);
    }
    return std::nullopt;
  }

  auto visit_shapes(const Visitor<const std::string&, FileID>& visitor) const {
    for (const auto [key, fileID] : applies) {
      if (visitor(key, fileID) == RE::BSVisit::BSVisitControl::kStop) {
        return;
      }
    }
  }

  bool operator==(const MATC& rhs) const {
    return rhs.form == form && rhs.name == name;
  }
};