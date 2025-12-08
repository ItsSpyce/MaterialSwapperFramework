#pragma once

#include "BSLightingShaderMaterialPBR.h"
#include "ENB/ENBSeriesAPI.h"

namespace CommunityShaders {
inline bool IsInstalled() {
  if (ENB_API::RequestENBAPI()) {
    _TRACE("ENB installed");
    return false;  // CS will deactivate itself if ENB is present
  }
  if (GetModuleHandle(L"CommunityShaders")) {
    _TRACE("Module handle found for CS");
    return true;
  }
  return false;
}
}  // namespace CommunityShaders