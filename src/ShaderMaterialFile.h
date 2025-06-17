#pragma once
#include "Helpers.h"
#include "MaterialStream.h"

struct ShaderMaterialFile;

enum class ShaderType {
  kLighting,
  kEffect,
};

struct BGSMFile;
struct BGEMFile;

struct ShaderMaterialFile {
  ShaderType shader_type;
  uint32_t version;
  uint32_t clamp;
  Vector2 uv_offset, uv_scale;
  float transparency;
  bool alpha_blend;
  uint32_t source_blend_mode;
  uint32_t destination_blend_mode;
  uint8_t alpha_test_threshold;
  bool alpha_test;
  bool depth_write;
  bool depth_test;
  bool ssr;
  bool wetness_control_ssr;
  bool decal;
  bool two_sided;
  bool decal_no_fade;
  bool non_occluder;
  bool refaction;
  bool refractional_falloff;
  float refraction_power;
  bool env_map_enabled;
  float env_map_mask_scale;
  bool depth_bias;
  bool grayscale_to_palette_color;
  uint8_t mask_writes;

  ShaderMaterialFile() = default;
  virtual void read(MaterialStream& stream);
  virtual ~ShaderMaterialFile() = default;
  _NODISCARD virtual size_t GetHashCode() const {
    size_t hash = 0;
    uint16_t flags = 0;
    flags |= (alpha_blend ? 1 : 0) << 0;
    flags |= (depth_write ? 1 : 0) << 1;
    flags |= (depth_test ? 1 : 0) << 2;
    flags |= (ssr ? 1 : 0) << 3;
    flags |= (wetness_control_ssr ? 1 : 0) << 4;
    flags |= (decal ? 1 : 0) << 5;
    flags |= (two_sided ? 1 : 0) << 6;
    flags |= (decal_no_fade ? 1 : 0) << 7;
    flags |= (non_occluder ? 1 : 0) << 8;
    flags |= (refaction ? 1 : 0) << 9;
    flags |= (refractional_falloff ? 1 : 0) << 10;
    flags |= (env_map_enabled ? 1 : 0) << 11;
    flags |= (depth_bias ? 1 : 0) << 12;
    flags |= (grayscale_to_palette_color ? 1 : 0) << 13;
    flags |= (alpha_test ? 1 : 0) << 14;
    Helpers::HashCombine(hash, flags);
    Helpers::HashCombine(hash, shader_type);
    Helpers::HashCombine(hash, version);
    Helpers::HashCombine(hash, clamp);
    Helpers::HashCombine(hash, uv_offset);
    Helpers::HashCombine(hash, uv_scale);
    Helpers::HashCombine(hash, transparency);
    Helpers::HashCombine(hash, source_blend_mode);
    Helpers::HashCombine(hash, destination_blend_mode);
    Helpers::HashCombine(hash, alpha_test_threshold);
    Helpers::HashCombine(hash, refraction_power);
    Helpers::HashCombine(hash, env_map_mask_scale);
    Helpers::HashCombine(hash, mask_writes);

    return hash;
  }

  template <typename T>
  _NODISCARD T* As() {
    return dynamic_cast<T*>(this);
  }
};

struct BGSMFile : ShaderMaterialFile {
  std::string diffuse_map;
  std::string normal_map;
  std::string smooth_spec_map;
  std::string grayscale_map;
  std::string glow_map;
  std::string wrinkle_map;
  std::string specular_map;
  std::string lighting_map;
  std::string flow_map;
  std::string distance_field_alpha_map;
  std::string env_map;
  std::string inner_layer_map;
  std::string displacement_map;
  bool enable_editor_alpha_threshold;
  bool translucency;
  bool translucency_thick_object;
  bool translucency_mix_albedo_with_subsurface_color;
  Vector3 translucency_subsurface_color;
  float translucency_transmissive_scale;
  float translucency_turbulence;
  bool rim_lighting;
  float rim_power;
  float back_light_power;
  bool subsurface_lighting;
  float subsurface_lighting_rolloff;
  bool specular_enabled;
  Vector3 specular_color;
  float specular_mult;
  float smoothness;
  float fresnel_power;
  float wetness_control_spec_scale;
  float wetness_control_spec_power_scale;
  float wetness_control_spec_minvar;
  float wetness_control_env_map_scale;
  float wetness_control_fresnel_power;
  float wetness_control_metalness;
  bool pbr;
  bool custom_porosity;
  float porosity_value;
  std::string root_material_path;
  bool aniso_lighting;
  bool emit_enabled;
  Vector3 emittance_color;
  float emittance_mult;
  bool model_space_normals;
  bool external_emittance;
  float lum_emittance;
  bool use_adaptive_emissive;
  Vector3 adaptive_emissive_exposure_params;
  bool back_lighting;
  bool receive_shadows;
  bool hide_secret;
  bool cast_shadows;
  bool dissolve_fade;
  bool assume_shadowmask;
  bool glow_map_enabled;
  bool env_map_window;
  bool env_map_eye;
  bool hair;
  Vector3 hair_tint_color;
  bool tree;
  bool facegen;
  bool skin_tint;
  bool tessellate;
  Vector2 displacement_map_params;
  Vector3 tessellation_params;
  float grayscale_to_palette_scale;
  bool skew_specular_alpha;
  bool terrain;
  Vector3 terrain_params;

  void read(MaterialStream& stream) override;
  size_t GetHashCode() const override {
    auto orig = ShaderMaterialFile::GetHashCode();
    uint32_t flags = 0;
    flags |= (enable_editor_alpha_threshold ? 1 : 0) << 0;
    flags |= (translucency ? 1 : 0) << 1;
    flags |= (translucency_thick_object ? 1 : 0) << 2;
    flags |= (translucency_mix_albedo_with_subsurface_color ? 1 : 0) << 3;
    flags |= (rim_lighting ? 1 : 0) << 4;
    flags |= (subsurface_lighting ? 1 : 0) << 5;
    flags |= (specular_enabled ? 1 : 0) << 6;
    flags |= (pbr ? 1 : 0) << 7;
    flags |= (custom_porosity ? 1 : 0) << 8;
    flags |= (aniso_lighting ? 1 : 0) << 9;
    flags |= (emit_enabled ? 1 : 0) << 10;
    flags |= (model_space_normals ? 1 : 0) << 11;
    flags |= (external_emittance ? 1 : 0) << 12;
    flags |= (use_adaptive_emissive ? 1 : 0) << 13;
    flags |= (back_lighting ? 1 : 0) << 14;
    flags |= (receive_shadows ? 1 : 0) << 15;
    flags |= (hide_secret ? 1 : 0) << 16;
    flags |= (cast_shadows ? 1 : 0) << 17;
    flags |= (dissolve_fade ? 1 : 0) << 18;
    flags |= (assume_shadowmask ? 1 : 0) << 19;
    flags |= (glow_map_enabled ? 1 : 0) << 20;
    flags |= (env_map_window ? 1 : 0) << 21;
    flags |= (env_map_eye ? 1 : 0) << 22;
    flags |= (hair ? 1 : 0) << 23;
    flags |= (tree ? 1 : 0) << 24;
    flags |= (facegen ? 1 : 0) << 25;
    flags |= (skin_tint ? 1 : 0) << 26;
    flags |= (tessellate ? 1 : 0) << 27;
    flags |= (skew_specular_alpha ? 1 : 0) << 28;
    flags |= (terrain ? 1 : 0) << 29;
    Helpers::HashCombine(orig, flags);
    Helpers::HashCombine(orig, diffuse_map);
    Helpers::HashCombine(orig, normal_map);
    Helpers::HashCombine(orig, smooth_spec_map);
    Helpers::HashCombine(orig, grayscale_map);
    Helpers::HashCombine(orig, glow_map);
    Helpers::HashCombine(orig, wrinkle_map);
    Helpers::HashCombine(orig, specular_map);
    Helpers::HashCombine(orig, lighting_map);
    Helpers::HashCombine(orig, flow_map);
    Helpers::HashCombine(orig, distance_field_alpha_map);
    Helpers::HashCombine(orig, env_map);
    Helpers::HashCombine(orig, inner_layer_map);
    Helpers::HashCombine(orig, displacement_map);
    Helpers::HashCombine(orig, translucency_subsurface_color);
    Helpers::HashCombine(orig, translucency_transmissive_scale);
    Helpers::HashCombine(orig, translucency_turbulence);
    Helpers::HashCombine(orig, rim_power);
    Helpers::HashCombine(orig, back_light_power);
    Helpers::HashCombine(orig, subsurface_lighting_rolloff);
    Helpers::HashCombine(orig, specular_color);
    Helpers::HashCombine(orig, specular_mult);
    Helpers::HashCombine(orig, smoothness);
    Helpers::HashCombine(orig, fresnel_power);
    Helpers::HashCombine(orig, wetness_control_spec_scale);
    Helpers::HashCombine(orig, wetness_control_spec_power_scale);
    Helpers::HashCombine(orig, wetness_control_spec_minvar);
    Helpers::HashCombine(orig, wetness_control_env_map_scale);
    Helpers::HashCombine(orig, wetness_control_fresnel_power);
    Helpers::HashCombine(orig, wetness_control_metalness);
    Helpers::HashCombine(orig, porosity_value);
    Helpers::HashCombine(orig, root_material_path);
    Helpers::HashCombine(orig, hair_tint_color);
    Helpers::HashCombine(orig, emittance_color);
    Helpers::HashCombine(orig, emittance_mult);
    Helpers::HashCombine(orig, lum_emittance);
    Helpers::HashCombine(orig, adaptive_emissive_exposure_params);
    Helpers::HashCombine(orig, displacement_map_params);
    Helpers::HashCombine(orig, tessellation_params);
    Helpers::HashCombine(orig, terrain_params);
    Helpers::HashCombine(orig, grayscale_to_palette_scale);
    return orig;
  }
};

struct BGEMFile : ShaderMaterialFile {
  std::string base_map;
  std::string grayscale_map;
  std::string env_map;
  std::string normal_map;
  std::string env_map_mask;
  std::string specular_map;
  std::string lighting_map;
  std::string glow_map;
  bool blood;
  bool effect_lighting;
  bool falloff;
  bool falloff_color;
  bool grayscale_to_palette_alpha;
  bool soft;
  Vector3 base_color;
  float base_color_scale;
  Vector4 falloff_params;
  float lighting_influence;
  uint8_t env_map_min_lod;
  float soft_depth;
  Vector3 emittance_color;
  Vector3 adaptive_emissive_exposure_params;
  bool glow_map_enabled;
  bool effect_pbr_specular;

  void read(MaterialStream& stream) override;

  size_t GetHashCode() const override {
    auto orig = ShaderMaterialFile::GetHashCode();
    uint32_t flags = 0;
    flags |= (blood ? 1 : 0) << 0;
    flags |= (effect_lighting ? 1 : 0) << 1;
    flags |= (falloff ? 1 : 0) << 2;
    flags |= (falloff_color ? 1 : 0) << 3;
    flags |= (grayscale_to_palette_alpha ? 1 : 0) << 4;
    flags |= (soft ? 1 : 0) << 5;
    flags |= (glow_map_enabled ? 1 : 0) << 6;
    flags |= (effect_pbr_specular ? 1 : 0) << 7;
    Helpers::HashCombine(orig, flags);
    Helpers::HashCombine(orig, base_map);
    Helpers::HashCombine(orig, grayscale_map);
    Helpers::HashCombine(orig, env_map);
    Helpers::HashCombine(orig, normal_map);
    Helpers::HashCombine(orig, env_map_mask);
    Helpers::HashCombine(orig, specular_map);
    Helpers::HashCombine(orig, lighting_map);
    Helpers::HashCombine(orig, glow_map);
    Helpers::HashCombine(orig, base_color);
    Helpers::HashCombine(orig, base_color_scale);
    Helpers::HashCombine(orig, falloff_params);
    Helpers::HashCombine(orig, lighting_influence);
    Helpers::HashCombine(orig, env_map_min_lod);
    Helpers::HashCombine(orig, soft_depth);
    Helpers::HashCombine(orig, emittance_color);
    Helpers::HashCombine(orig, adaptive_emissive_exposure_params);
    return orig;
  }
};