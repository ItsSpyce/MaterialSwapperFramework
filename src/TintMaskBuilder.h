#pragma once

#include <dxgi.h>
#include "Graphics/ShaderFactory.h"

namespace TintMaskBuilder {
inline bool Initialize() {
  
}

inline bool ApplyTintMask(RE::NiSourceTexture* texture,
                          const MaterialRecord& record) {
  RETURN_IF_FALSE(texture)
  auto rendererTexture = texture->rendererTexture;
  RETURN_IF_FALSE(rendererTexture)
  if (record.color->at(0) == 0 && record.color->at(1) == 0 &&
      record.color->at(2) == 0 && record.color->at(3) == 0) {
    return false;  // No tint mask to apply
  }
  return false;
}
}  // namespace TintMaskBuilder