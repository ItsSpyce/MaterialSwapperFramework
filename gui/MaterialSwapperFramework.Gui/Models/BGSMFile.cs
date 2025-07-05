using System.IO;
using MaterialSwapperFramework.Gui.Extensions;
using Newtonsoft.Json;
using System.Numerics;

namespace MaterialSwapperFramework.Gui.Models;

public class BGSMFile : MaterialFileBase
{
  [JsonProperty]
  public string? DiffuseMap { get; set; }
  [JsonProperty]
  public string? NormalMap { get; set; }
  [JsonProperty]
  public string? SmoothSpecularMap { get; set; }
  [JsonProperty]
  public string? GrayscaleMap { get; set; }
  [JsonProperty]
  public string? GlowMap { get; set; }
  [JsonProperty]
  public string? WrinkleMap { get; set; }
  [JsonProperty]
  public string? SpecularMap { get; set; }
  [JsonProperty]
  public string? LightingMap { get; set; }
  [JsonProperty]
  public string? FlowMap { get; set; }
  [JsonProperty]
  public string? DistanceFieldAlphaMap { get; set; }
  [JsonProperty]
  public string? EnvMap { get; set; }
  [JsonProperty]
  public string? InnerLayerMap { get; set; }
  [JsonProperty]
  public string? DisplacementMap { get; set; }
  [JsonProperty]
  public bool EnableEditorAlphaThreshold { get; set; }
  [JsonProperty]
  public bool Translucency { get; set; }
  [JsonProperty]
  public bool TranslucencyThickObject { get; set; }
  [JsonProperty]
  public bool TranslucencyMixAlbedoWithSubsurfaceColor { get; set; }
  [JsonProperty]
  public Vector3 TranslucencySubsurfaceColor { get; set; } = Vector3.Zero;
  [JsonProperty]
  public float TranslucencyTransmissiveScale { get; set; }
  [JsonProperty]
  public float TranslucencyTurbulence { get; set; }
  [JsonProperty]
  public bool RimLighting { get; set; }
  [JsonProperty]
  public float RimPower { get; set; }
  [JsonProperty]
  public float BackLightPower { get; set; }
  [JsonProperty]
  public bool SubsurfaceLighting { get; set; }
  [JsonProperty]
  public float SubsurfaceLightingRollOff { get; set; }
  [JsonProperty]
  public bool SpecularEnabled { get; set; }
  [JsonProperty]
  public Vector3 SpecularColor { get; set; } = Vector3.Zero;
  [JsonProperty]
  public float SpecularMult { get; set; }
  [JsonProperty]
  public float Smoothness { get; set; }
  [JsonProperty]
  public float FresnelPower { get; set; }
  [JsonProperty]
  public float WetnessControlSpecScale { get; set; }
  [JsonProperty]
  public float WetnessControlSpecPowerScale { get; set; }
  [JsonProperty]
  public float WetnessControlSpecMinVar { get; set; }
  [JsonProperty]
  public float WetnessControlEnvMapScale { get; set; }
  [JsonProperty]
  public float WetnessControlFresnelPower { get; set; }
  public float WetnessControlMetalness { get; set; }
  [JsonProperty]
  public bool PBR { get; set; }
  [JsonProperty]
  public bool CustomPorosity { get; set; }
  [JsonProperty]
  public float PorosityValue { get; set; }
  [JsonProperty]
  public string? RootMaterialPath { get; set; }
  [JsonProperty]
  public bool AnisoLighting { get; set; }
  [JsonProperty]
  public bool EmitEnabled { get; set; }
  [JsonProperty]
  public Vector3 EmitColor { get; set; } = Vector3.Zero;
  [JsonProperty]
  public float EmitMult { get; set; }
  [JsonProperty]
  public bool ModelSpaceNormals { get; set; }
  [JsonProperty]
  public bool ExternalEmit { get; set; }
  [JsonProperty]
  public float LumEmit { get; set; }
  [JsonProperty]
  public bool UseAdaptiveEmissive { get; set; }
  [JsonProperty]
  public Vector3 AdaptiveEmissiveExposureParams { get; set; }
  [JsonProperty]
  public bool BackLighting { get; set; }
  [JsonProperty]
  public bool ReceiveShadows { get; set; }
  [JsonProperty]
  public bool HideSecret { get; set; }
  [JsonProperty]
  public bool CastShadows { get; set; }
  [JsonProperty]
  public bool DissolveFade { get; set; }
  [JsonProperty]
  public bool AssumeShadowMask { get; set; }
  [JsonProperty]
  public bool GlowMapEnabled { get; set; }
  [JsonProperty]
  public bool EnvMapWindow { get; set; }
  [JsonProperty]
  public bool EnvMapEye { get; set; }
  [JsonProperty]
  public bool Hair { get; set; }
  [JsonProperty]
  public Vector3 HairTintColor { get; set; } = Vector3.Zero;
  [JsonProperty]
  public bool Tree { get; set; }
  [JsonProperty]
  public bool FaceGen { get; set; }
  [JsonProperty]
  public bool SkinTint { get; set; }
  [JsonProperty]
  public bool Tessellate { get; set; }
  [JsonProperty]
  public Vector2 DisplacementMapParams { get; set; } = Vector2.Zero;
  [JsonProperty]
  public Vector3 TessellationParams { get; set; } = Vector3.Zero;
  [JsonProperty]
  public new float GrayscaleToPaletteColor { get; set; }
  [JsonProperty]
  public bool SkewSpecularAlpha { get; set; }
  [JsonProperty]
  public bool Terrain { get; set; }
  [JsonProperty]
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