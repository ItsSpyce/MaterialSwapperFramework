#include "Models/MaterialFileBase.h"

void MaterialFileBase::Read(MaterialStream& stream) {
  stream.Read(version);
  stream.Read(clamp);
  stream.Read(uvOffset);
  stream.Read(uvScale);
  stream.Read(transparency);
  stream.Read(alphaBlend);
  stream.Read(sourceBlendMode);
  stream.Read(destinationBlendMode);
  stream.Read(alphaTestThreshold);
  stream.Read(alphaTest);
  stream.Read(depthWrite);
  stream.Read(depthTest);
  stream.Read(ssr);
  stream.Read(wetnessControlSsr);
  stream.Read(decal);
  stream.Read(twoSided);
  stream.Read(decalNoFade);
  stream.Read(nonOccluder);
  stream.Read(refraction);
  stream.Read(refractionalFalloff);
  stream.Read(refractionPower);
  if (version < 10) {
    stream.Read(envMapEnabled);
    stream.Read(envMapMaskScale);
  } else {
    stream.Read(depthBias);
  }
  stream.Read(grayscaleToPaletteColor);
  if (version >= 6) {
    stream.Read(maskWrites);
  }
}

void BGSMFile::Read(MaterialStream& stream) {
  MaterialFileBase::Read(stream);
  stream.Read(diffuseMap);
  stream.Read(normalMap);
  stream.Read(smoothSpecMap);
  stream.Read(grayscaleMap);
  if (version >= 3) {
    stream.Read(glowMap);
    stream.Read(wrinkleMap);
    stream.Read(specularMap);
    stream.Read(lightingMap);
    stream.Read(flowMap);
    if (version >= 17) {
      stream.Read(distanceFieldAlphaMap);
    }
  } else {
    stream.Read(envMap);
    stream.Read(glowMap);
    stream.Read(innerLayerMap);
    stream.Read(wrinkleMap);
    stream.Read(displacementMap);
  }
  stream.Read(enableEditorAlphaThreshold);
  if (version >= 8) {
    stream.Read(translucency);
    stream.Read(translucencyThickObject);
    stream.Read(translucencyMixAlbedoWithSubsurfaceColor);
    stream.Read(translucencySubsurfaceColor);
    stream.Read(translucencyTransmissiveScale);
    stream.Read(translucencyTurbulence);
  } else {
    stream.Read(rimLighting);
    stream.Read(rimPower);
    stream.Read(backLightPower);
    stream.Read(subsurfaceLighting);
    stream.Read(subsurfaceLightingRolloff);
  }
  stream.Read(specularEnabled);
  stream.Read(specularColor);
  stream.Read(specularMult);
  stream.Read(smoothness);
  stream.Read(fresnelPower);
  stream.Read(wetnessControlSpecScale);
  stream.Read(wetnessControlSpecPowerScale);
  stream.Read(wetnessControlSpecMinvar);
  if (version < 10) {
    stream.Read(wetnessControlEnvMapScale);
  }
  stream.Read(wetnessControlFresnelPower);
  stream.Read(wetnessControlMetalness);
  if (version >= 3) {
    stream.Read(pbr);
    if (version >= 9) {
      stream.Read(customPorosity);
      stream.Read(porosityValue);
    }
  }
  stream.Read(rootMaterialPath);
  stream.Read(anisoLighting);
  stream.Read(emitEnabled);
  if (emitEnabled) {
    stream.Read(emitColor);
  }
  stream.Read(emitMult);
  stream.Read(modelSpaceNormals);
  stream.Read(externalEmit);
  if (version >= 12) {
    stream.Read(lumEmit);
    if (version >= 13) {
      stream.Read(useAdaptiveEmissive);
      stream.Read(adaptiveEmissiveExposureParams);
    }
  } else if (version < 8) {
    stream.Read(backLighting);
  }
  stream.Read(receiveShadows);
  stream.Read(hideSecret);
  stream.Read(castShadows);
  stream.Read(dissolveFade);
  stream.Read(assumeShadowmask);
  stream.Read(glowMapEnabled);
  if (version < 7) {
    stream.Read(envMapWindow);
    stream.Read(envMapEye);
  }
  stream.Read(hair);
  stream.Read(hairTintColor);
  stream.Read(tree);
  stream.Read(facegen);
  stream.Read(skinTint);
  stream.Read(tessellate);
  if (version < 3) {
    stream.Read(displacementMapParams);
    stream.Read(tessellationParams);
  }
  stream.Read(grayscaleToPaletteScale);
  if (version >= 1) {
    stream.Read(skewSpecularAlpha);
    stream.Read(terrain);
    if (terrain) {
      if (version == 3) {
        stream.Skip(4);
      }
      stream.Read(terrainParams);
    }
  }
}

void BGEMFile::Read(MaterialStream& stream) {
  MaterialFileBase::Read(stream);
  stream.Read(baseMap);
  stream.Read(grayscaleMap);
  stream.Read(envMap);
  stream.Read(normalMap);
  stream.Read(envMapMask);
  if (version >= 11) {
    stream.Read(specularMap);
    stream.Read(lightingMap);
    stream.Read(glowMap);
  }
  if (version >= 10) {
    stream.Read(envMapEnabled);
    stream.Read(envMapMaskScale);
  }
  stream.Read(blood);
  stream.Read(effectLighting);
  stream.Read(falloff);
  stream.Read(falloffColor);
  stream.Read(grayscaleToPaletteAlpha);
  stream.Read(soft);
  stream.Read(baseColor);
  stream.Read(baseColorScale);
  stream.Read(falloffParams);
  stream.Read(lightingInfluence);
  stream.Read(envMapMinLod);
  stream.Read(softDepth);
  if (version >= 11) {
    stream.Read(emitColor);
  }
  if (version >= 15) {
    stream.Read(adaptiveEmissiveExposureParams);
  }
  if (version >= 16) {
    stream.Read(glowMapEnabled);
  }
  if (version >= 20) {
    stream.Read(effectPbrSpecular);
  }
}