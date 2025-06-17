#include "ShaderMaterialFile.h"

void ShaderMaterialFile::read(MaterialStream& stream) {
  stream.read(version);
  stream.read(clamp);
  stream.read(uv_offset);
  stream.read(uv_scale);
  stream.read(transparency);
  stream.read(alpha_blend);
  stream.read(source_blend_mode);
  stream.read(destination_blend_mode);
  stream.read(alpha_test_threshold);
  stream.read(alpha_test);
  stream.read(depth_write);
  stream.read(depth_test);
  stream.read(ssr);
  stream.read(wetness_control_ssr);
  stream.read(decal);
  stream.read(two_sided);
  stream.read(decal_no_fade);
  stream.read(non_occluder);
  stream.read(refaction);
  stream.read(refractional_falloff);
  stream.read(refraction_power);
  if (version < 10) {
    stream.read(env_map_enabled);
    stream.read(env_map_mask_scale);
  } else {
    stream.read(depth_bias);
  }
  stream.read(grayscale_to_palette_color);
  if (version >= 6) {
    stream.read(mask_writes);
  }
}

void BGSMFile::read(MaterialStream& stream) {
  ShaderMaterialFile::read(stream);
  stream.read(diffuse_map);
  stream.read(normal_map);
  stream.read(smooth_spec_map);
  stream.read(grayscale_map);
  if (version >= 3) {
    stream.read(glow_map);
    stream.read(wrinkle_map);
    stream.read(specular_map);
    stream.read(lighting_map);
    stream.read(flow_map);
    if (version >= 17) {
      stream.read(distance_field_alpha_map);
    }
  } else {
    stream.read(env_map);
    stream.read(glow_map);
    stream.read(inner_layer_map);
    stream.read(wrinkle_map);
    stream.read(displacement_map);
  }
  stream.read(enable_editor_alpha_threshold);
  if (version >= 8) {
    stream.read(translucency);
    stream.read(translucency_thick_object);
    stream.read(translucency_mix_albedo_with_subsurface_color);
    stream.read(translucency_subsurface_color);
    stream.read(translucency_transmissive_scale);
    stream.read(translucency_turbulence);
  } else {
    stream.read(rim_lighting);
    stream.read(rim_power);
    stream.read(back_light_power);
    stream.read(subsurface_lighting);
    stream.read(subsurface_lighting_rolloff);
  }
  stream.read(specular_enabled);
  stream.read(specular_color);
  stream.read(specular_mult);
  stream.read(smoothness);
  stream.read(fresnel_power);
  stream.read(wetness_control_spec_scale);
  stream.read(wetness_control_spec_power_scale);
  stream.read(wetness_control_spec_minvar);
  if (version < 10) {
    stream.read(wetness_control_env_map_scale);
  }
  stream.read(wetness_control_fresnel_power);
  stream.read(wetness_control_metalness);
  if (version >= 3) {
    stream.read(pbr);
    if (version >= 9) {
      stream.read(custom_porosity);
      stream.read(porosity_value);
    }
  }
  stream.read(root_material_path);
  stream.read(aniso_lighting);
  stream.read(emit_enabled);
  if (emit_enabled) {
    stream.read(emittance_color);
  }
  stream.read(emittance_mult);
  stream.read(model_space_normals);
  stream.read(external_emittance);
  if (version >= 12) {
    stream.read(lum_emittance);
    if (version >= 13) {
      stream.read(use_adaptive_emissive);
      stream.read(adaptive_emissive_exposure_params);
    }
  } else if (version < 8) {
    stream.read(back_lighting);
  }
  stream.read(receive_shadows);
  stream.read(hide_secret);
  stream.read(cast_shadows);
  stream.read(dissolve_fade);
  stream.read(assume_shadowmask);
  stream.read(glow_map_enabled);
  if (version < 7) {
    stream.read(env_map_window);
    stream.read(env_map_eye);
  }
  stream.read(hair);
  stream.read(hair_tint_color);
  stream.read(tree);
  stream.read(facegen);
  stream.read(skin_tint);
  stream.read(tessellate);
  if (version < 3) {
    stream.read(displacement_map_params);
    stream.read(tessellation_params);
  }
  stream.read(grayscale_to_palette_scale);
  if (version >= 1) {
    stream.read(skew_specular_alpha);
    stream.read(terrain);
    if (terrain) {
      if (version == 3) {
        stream.skip(4);
      }
      stream.read(terrain_params);
    }
  }
}

void BGEMFile::read(MaterialStream& stream) {
  ShaderMaterialFile::read(stream);
  stream.read(base_map);
  stream.read(grayscale_map);
  stream.read(env_map);
  stream.read(normal_map);
  stream.read(env_map_mask);
  if (version >= 11) {
    stream.read(specular_map);
    stream.read(lighting_map);
    stream.read(glow_map);
  }
  if (version >= 10) {
    stream.read(env_map_enabled);
    stream.read(env_map_mask_scale);
  }
  stream.read(blood);
  stream.read(effect_lighting);
  stream.read(falloff);
  stream.read(falloff_color);
  stream.read(grayscale_to_palette_alpha);
  stream.read(soft);
  stream.read(base_color);
  stream.read(base_color_scale);
  stream.read(falloff_params);
  stream.read(lighting_influence);
  stream.read(env_map_min_lod);
  stream.read(soft_depth);
  if (version >= 11) {
    stream.read(emittance_color);
  }
  if (version >= 15) {
    stream.read(adaptive_emissive_exposure_params);
  }
  if (version >= 16) {
    stream.read(glow_map_enabled);
  }
  if (version >= 20) {
    stream.read(effect_pbr_specular);
  }
}