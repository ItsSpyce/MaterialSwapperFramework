#pragma once

#include <DirectXMath.h>

enum class MaterialRenderingFlags : uint8_t {
  kTwoSided = 1 << 0,
  kReceiveShadows = 1 << 1,
  kCastShadows = 1 << 2,
  kDecal = 1 << 3,
};

typedef struct MaterialRecordHeader {
  string schema = "2.0";
  optional<string> inherits = nullopt;
} material_record_header_t;

typedef struct MaterialRecordTextures {
  optional<string> diffuse = nullopt, normal = nullopt, smoothSpec = nullopt,
                   environment = nullopt, environmentMask = nullopt,
                   colorChannels = nullopt;
} material_record_textures_t;