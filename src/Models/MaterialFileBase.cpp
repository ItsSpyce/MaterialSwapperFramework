#include "Models/MaterialFileBase.h"

void MaterialFileBase::Read(MaterialStream& stream) {
  stream.read(version);
  stream.read(clamp);
  stream.read(uvOffset);
  stream.read(uvScale);
  stream.read(transparency);
  stream.read(alphaBlend);
  stream.read(sourceBlendMode);
  stream.read(destinationBlendMode);
  stream.read(alphaTestThreshold);
  stream.read(alphaTest);
  stream.read(depthWrite);
  stream.read(depthTest);
  stream.read(ssr);
  stream.read(wetnessControlSsr);
  stream.read(decal);
  stream.read(twoSided);
  stream.read(decalNoFade);
  stream.read(nonOccluder);
  stream.read(refraction);
  stream.read(refractionalFalloff);
  stream.read(refractionPower);
  if (version < 10) {
    stream.read(envMapEnabled);
    stream.read(envMapMaskScale);
  } else {
    stream.read(depthBias);
  }
  stream.read(grayscaleToPaletteColor);
  if (version >= 6) {
    stream.read(maskWrites);
  }
}

void BGSMFile::Read(MaterialStream& stream) {
  MaterialFileBase::Read(stream);
  stream.read(diffuseMap);
  stream.read(normalMap);
  stream.read(smoothSpecMap);
  stream.read(grayscaleMap);
  if (version >= 3) {
    stream.read(glowMap);
    stream.read(wrinkleMap);
    stream.read(specularMap);
    stream.read(lightingMap);
    stream.read(flowMap);
    if (version >= 17) {
      stream.read(distanceFieldAlphaMap);
    }
  } else {
    stream.read(envMap);
    stream.read(glowMap);
    stream.read(innerLayerMap);
    stream.read(wrinkleMap);
    stream.read(displacementMap);
  }
  stream.read(enableEditorAlphaThreshold);
  if (version >= 8) {
    stream.read(translucency);
    stream.read(translucencyThickObject);
    stream.read(translucencyMixAlbedoWithSubsurfaceColor);
    stream.read(translucencySubsurfaceColor);
    stream.read(translucencyTransmissiveScale);
    stream.read(translucencyTurbulence);
  } else {
    stream.read(rimLighting);
    stream.read(rimPower);
    stream.read(backLightPower);
    stream.read(subsurfaceLighting);
    stream.read(subsurfaceLightingRolloff);
  }
  stream.read(specularEnabled);
  stream.read(specularColor);
  stream.read(specularMult);
  stream.read(smoothness);
  stream.read(fresnelPower);
  stream.read(wetnessControlSpecScale);
  stream.read(wetnessControlSpecPowerScale);
  stream.read(wetnessControlSpecMinvar);
  if (version < 10) {
    stream.read(wetnessControlEnvMapScale);
  }
  stream.read(wetnessControlFresnelPower);
  stream.read(wetnessControlMetalness);
  if (version >= 3) {
    stream.read(pbr);
    if (version >= 9) {
      stream.read(customPorosity);
      stream.read(porosityValue);
    }
  }
  stream.read(rootMaterialPath);
  stream.read(anisoLighting);
  stream.read(emitEnabled);
  if (emitEnabled) {
    stream.read(emitColor);
  }
  stream.read(emitMult);
  stream.read(modelSpaceNormals);
  stream.read(externalEmit);
  if (version >= 12) {
    stream.read(lumEmit);
    if (version >= 13) {
      stream.read(useAdaptiveEmissive);
      stream.read(adaptiveEmissiveExposureParams);
    }
  } else if (version < 8) {
    stream.read(backLighting);
  }
  stream.read(receiveShadows);
  stream.read(hideSecret);
  stream.read(castShadows);
  stream.read(dissolveFade);
  stream.read(assumeShadowmask);
  stream.read(glowMapEnabled);
  if (version < 7) {
    stream.read(envMapWindow);
    stream.read(envMapEye);
  }
  stream.read(hair);
  stream.read(hairTintColor);
  stream.read(tree);
  stream.read(facegen);
  stream.read(skinTint);
  stream.read(tessellate);
  if (version < 3) {
    stream.read(displacementMapParams);
    stream.read(tessellationParams);
  }
  stream.read(grayscaleToPaletteScale);
  if (version >= 1) {
    stream.read(skewSpecularAlpha);
    stream.read(terrain);
    if (terrain) {
      if (version == 3) {
        stream.skip(4);
      }
      stream.read(terrainParams);
    }
  }
}

void BGEMFile::Read(MaterialStream& stream) {
  MaterialFileBase::Read(stream);
  stream.read(baseMap);
  stream.read(grayscaleMap);
  stream.read(envMap);
  stream.read(normalMap);
  stream.read(envMapMask);
  if (version >= 11) {
    stream.read(specularMap);
    stream.read(lightingMap);
    stream.read(glowMap);
  }
  if (version >= 10) {
    stream.read(envMapEnabled);
    stream.read(envMapMaskScale);
  }
  stream.read(blood);
  stream.read(effectLighting);
  stream.read(falloff);
  stream.read(falloffColor);
  stream.read(grayscaleToPaletteAlpha);
  stream.read(soft);
  stream.read(baseColor);
  stream.read(baseColorScale);
  stream.read(falloffParams);
  stream.read(lightingInfluence);
  stream.read(envMapMinLod);
  stream.read(softDepth);
  if (version >= 11) {
    stream.read(emitColor);
  }
  if (version >= 15) {
    stream.read(adaptiveEmissiveExposureParams);
  }
  if (version >= 16) {
    stream.read(glowMapEnabled);
  }
  if (version >= 20) {
    stream.read(effectPbrSpecular);
  }
}