using System.IO;
using MaterialSwapperFramework.Gui.Extensions;
using System.Numerics;
using System.Text.Json.Serialization;

namespace MaterialSwapperFramework.Gui.Models;

public class BGSMFile : MaterialFileBase
{
  [JsonPropertyName("diffuseMap")]
  public string? DiffuseMap { get; set; }
  [JsonPropertyName("normalMap")]
  public string? NormalMap { get; set; }
  [JsonPropertyName("smoothSpecularMap")]
  public string? SmoothSpecularMap { get; set; }
  [JsonPropertyName("grayscaleMap")]
  public string? GrayscaleMap { get; set; }
  [JsonPropertyName("glowMap")]
  public string? GlowMap { get; set; }
  [JsonPropertyName("wrinkleMap")]
  public string? WrinkleMap { get; set; }
  [JsonPropertyName("specularMap")]
  public string? SpecularMap { get; set; }
  [JsonPropertyName("lightingMap")]
  public string? LightingMap { get; set; }
  [JsonPropertyName("flowMap")]
  public string? FlowMap { get; set; }
  [JsonPropertyName("distanceFieldAlphaMap")]
  public string? DistanceFieldAlphaMap { get; set; }
  [JsonPropertyName("envMap")]
  public string? EnvMap { get; set; }
  [JsonPropertyName("innerLayerMap")]
  public string? InnerLayerMap { get; set; }
  [JsonPropertyName("displacementMap")]
  public string? DisplacementMap { get; set; }
  [JsonPropertyName("enableEditorAlphaThreshold")]
  public bool EnableEditorAlphaThreshold { get; set; }
  [JsonPropertyName("translucency")]
  public bool Translucency { get; set; }
  [JsonPropertyName("translucencyThickObject")]
  public bool TranslucencyThickObject { get; set; }
  [JsonPropertyName("translucencyMixAlbedoWithSubsurfaceColor")]
  public bool TranslucencyMixAlbedoWithSubsurfaceColor { get; set; }
  [JsonPropertyName("translucencySubsurfaceColor")]
  public Vector3 TranslucencySubsurfaceColor { get; set; } = Vector3.Zero;
  [JsonPropertyName("translucencyTransmissiveScale")]
  public float TranslucencyTransmissiveScale { get; set; }
  [JsonPropertyName("translucencyTurbulence")]
  public float TranslucencyTurbulence { get; set; }
  [JsonPropertyName("rimLighting")]
  public bool RimLighting { get; set; }
  [JsonPropertyName("rimPower")]
  public float RimPower { get; set; }
  [JsonPropertyName("backLightPower")]
  public float BackLightPower { get; set; }
  [JsonPropertyName("subsurfaceLighting")]
  public bool SubsurfaceLighting { get; set; }
  [JsonPropertyName("subsurfaceLightingRollOff")]
  public float SubsurfaceLightingRollOff { get; set; }
  [JsonPropertyName("specularEnabled")]
  public bool SpecularEnabled { get; set; }
  [JsonPropertyName("specularColor")]
  public Vector3 SpecularColor { get; set; } = Vector3.Zero;
  [JsonPropertyName("specularMult")]
  public float SpecularMult { get; set; }
  [JsonPropertyName("smoothness")]
  public float Smoothness { get; set; }
  [JsonPropertyName("fresnelPower")]
  public float FresnelPower { get; set; }
  [JsonPropertyName("wetnessControlSpecScale")]
  public float WetnessControlSpecScale { get; set; }
  [JsonPropertyName("wetnessControlSpecPowerScale")]
  public float WetnessControlSpecPowerScale { get; set; }
  [JsonPropertyName("wetnessControlSpecMinVar")]
  public float WetnessControlSpecMinVar { get; set; }
  [JsonPropertyName("wetnessControlEnvMapScale")]
  public float WetnessControlEnvMapScale { get; set; }
  [JsonPropertyName("wetnessControlFresnelPower")]
  public float WetnessControlFresnelPower { get; set; }
  [JsonPropertyName("wetnessControlMetalness")]
  public float WetnessControlMetalness { get; set; }
  [JsonPropertyName("pbr")]
  public bool PBR { get; set; }
  [JsonPropertyName("customPorosity")]
  public bool CustomPorosity { get; set; }
  [JsonPropertyName("porosityValue")]
  public float PorosityValue { get; set; }
  [JsonPropertyName("rootMaterialPath")]
  public string? RootMaterialPath { get; set; }
  [JsonPropertyName("anisoLighting")]
  public bool AnisoLighting { get; set; }
  [JsonPropertyName("emitEnabled")]
  public bool EmitEnabled { get; set; }
  [JsonPropertyName("emitColor")]
  public Vector3 EmitColor { get; set; } = Vector3.Zero;
  [JsonPropertyName("emitMult")]
  public float EmitMult { get; set; }
  [JsonPropertyName("modelSpaceNormals")]
  public bool ModelSpaceNormals { get; set; }
  [JsonPropertyName("externalEmit")]
  public bool ExternalEmit { get; set; }
  [JsonPropertyName("lumEmit")]
  public float LumEmit { get; set; }
  [JsonPropertyName("useAdaptiveEmissive")]
  public bool UseAdaptiveEmissive { get; set; }
  [JsonPropertyName("adaptiveEmissiveExposureParams")]
  public Vector3 AdaptiveEmissiveExposureParams { get; set; }
  [JsonPropertyName("backLighting")]
  public bool BackLighting { get; set; }
  [JsonPropertyName("receiveShadows")]
  public bool ReceiveShadows { get; set; }
  [JsonPropertyName("hideSecret")]
  public bool HideSecret { get; set; }
  [JsonPropertyName("castShadows")]
  public bool CastShadows { get; set; }
  [JsonPropertyName("dissolveFade")]
  public bool DissolveFade { get; set; }
  [JsonPropertyName("assumeShadowMask")]
  public bool AssumeShadowMask { get; set; }
  [JsonPropertyName("glowMapEnabled")]
  public bool GlowMapEnabled { get; set; }
  [JsonPropertyName("envMapWindow")]
  public bool EnvMapWindow { get; set; }
  [JsonPropertyName("envMapEye")]
  public bool EnvMapEye { get; set; }
  [JsonPropertyName("hair")]
  public bool Hair { get; set; }
  [JsonPropertyName("hairTintColor")]
  public Vector3 HairTintColor { get; set; } = Vector3.Zero;
  [JsonPropertyName("tree")]
  public bool Tree { get; set; }
  [JsonPropertyName("faceGen")]
  public bool FaceGen { get; set; }
  [JsonPropertyName("skinTint")]
  public bool SkinTint { get; set; }
  [JsonPropertyName("tessellate")]
  public bool Tessellate { get; set; }
  [JsonPropertyName("displacementMapParams")]
  public Vector2 DisplacementMapParams { get; set; } = Vector2.Zero;
  [JsonPropertyName("tessellationParams")]
  public Vector3 TessellationParams { get; set; } = Vector3.Zero;
  [JsonPropertyName("grayscaleToPaletteColor")]
  public new float GrayscaleToPaletteColor { get; set; }
  [JsonPropertyName("skewSpecularAlpha")]
  public bool SkewSpecularAlpha { get; set; }
  [JsonPropertyName("terrain")]
  public bool Terrain { get; set; }
  [JsonPropertyName("terrainParams")]
  public Vector3 TerrainParams { get; set; } = Vector3.Zero;

  public override void ReadFromStream(BinaryReader reader)
  {
    base.ReadFromStream(reader);
    DiffuseMap = reader.ReadNullTerminatedString();
    NormalMap = reader.ReadNullTerminatedString();
    SmoothSpecularMap = reader.ReadNullTerminatedString();
    GrayscaleMap = reader.ReadNullTerminatedString();
    if (Version >= 3)
    {
      GlowMap = reader.ReadNullTerminatedString();
      WrinkleMap = reader.ReadNullTerminatedString();
      SpecularMap = reader.ReadNullTerminatedString();
      LightingMap = reader.ReadNullTerminatedString();
      FlowMap = reader.ReadNullTerminatedString();
      if (Version >= 17)
      {
        DistanceFieldAlphaMap = reader.ReadNullTerminatedString();
      }
    }
    else
    {
      EnvMap = reader.ReadNullTerminatedString();
      GlowMap = reader.ReadNullTerminatedString();
      InnerLayerMap = reader.ReadNullTerminatedString();
      WrinkleMap = reader.ReadNullTerminatedString();
      DisplacementMap = reader.ReadNullTerminatedString();
    }
    EnableEditorAlphaThreshold = reader.ReadBoolean();
    if (Version >= 8)
    {
      Translucency = reader.ReadBoolean();
      TranslucencyThickObject = reader.ReadBoolean();
      TranslucencyMixAlbedoWithSubsurfaceColor = reader.ReadBoolean();
      TranslucencySubsurfaceColor = reader.ReadVec3();
      TranslucencyTransmissiveScale = reader.ReadSingle();
      TranslucencyTurbulence = reader.ReadSingle();
    }
    else
    {
      RimLighting = reader.ReadBoolean();
      RimPower = reader.ReadSingle();
      BackLightPower = reader.ReadSingle();
      SubsurfaceLighting = reader.ReadBoolean();
      SubsurfaceLightingRollOff = reader.ReadSingle();
    }
    SpecularEnabled = reader.ReadBoolean();
    SpecularColor = reader.ReadVec3();
    SpecularMult = reader.ReadSingle();
    Smoothness = reader.ReadSingle();
    FresnelPower = reader.ReadSingle();
    WetnessControlSpecScale = reader.ReadSingle();
    WetnessControlSpecPowerScale = reader.ReadSingle();
    WetnessControlSpecMinVar = reader.ReadSingle();
    if (Version < 10)
    {
      WetnessControlEnvMapScale = reader.ReadSingle();
    }
    WetnessControlFresnelPower = reader.ReadSingle();
    WetnessControlMetalness = reader.ReadSingle();
    if (Version >= 3)
    {
      PBR = reader.ReadBoolean();
      if (Version >= 9)
      {
        CustomPorosity = reader.ReadBoolean();
        PorosityValue = reader.ReadSingle();
      }
    }
    RootMaterialPath = reader.ReadNullTerminatedString();
    AnisoLighting = reader.ReadBoolean();
    EmitEnabled = reader.ReadBoolean();
    if (EmitEnabled)
    {
      EmitColor = reader.ReadVec3();
    }
    EmitMult = reader.ReadSingle();
    ModelSpaceNormals = reader.ReadBoolean();
    ExternalEmit = reader.ReadBoolean();
    if (Version >= 12)
    {
      LumEmit = reader.ReadSingle();
      if (Version >= 13)
      {
        UseAdaptiveEmissive = reader.ReadBoolean();
        AdaptiveEmissiveExposureParams = reader.ReadVec3();
      }
    }
    else if (Version < 8)
    {
      BackLighting = reader.ReadBoolean();
    }
    ReceiveShadows = reader.ReadBoolean();
    HideSecret = reader.ReadBoolean();
    CastShadows = reader.ReadBoolean();
    DissolveFade = reader.ReadBoolean();
    AssumeShadowMask = reader.ReadBoolean();
    GlowMapEnabled = reader.ReadBoolean();
    if (Version < 7)
    {
      EnvMapWindow = reader.ReadBoolean();
      EnvMapEye = reader.ReadBoolean();
    }
    Hair = reader.ReadBoolean();
    HairTintColor = reader.ReadVec3();
    Tree = reader.ReadBoolean();
    FaceGen = reader.ReadBoolean();
    SkinTint = reader.ReadBoolean();
    Tessellate = reader.ReadBoolean();
    if (Version < 3)
    {
      DisplacementMapParams = reader.ReadVec2();
      TessellationParams = reader.ReadVec3();
    }
    GrayscaleToPaletteColor = reader.ReadSingle();
    if (Version >= 1)
    {
      SkewSpecularAlpha = reader.ReadBoolean();
      Terrain = reader.ReadBoolean();
      if (Terrain)
      {
        if (Version == 3)
        {
          reader.ReadUInt32(); // Skip unknown uint
        }
        TerrainParams = reader.ReadVec3();
      }
    }
  }
}