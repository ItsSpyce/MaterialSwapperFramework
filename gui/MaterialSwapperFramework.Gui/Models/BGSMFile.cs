using System.Numerics;
using MaterialSwapperFramework.Gui.Extensions;

namespace MaterialSwapperFramework.Gui.Models;

public class BGSMFile : MaterialFileBase
{
  public string? DiffuseMap { get; set; }
  public string? NormalMap { get; set; }
  public string? SmoothSpecularMap { get; set; }
  public string? GrayscaleMap { get; set; }
  public string? GlowMap { get; set; }
  public string? WrinkleMap { get; set; }
  public string? SpecularMap { get; set; }
  public string? LightingMap { get; set; }
  public string? FlowMap { get; set; }
  public string? DistanceFieldAlphaMap { get; set; }
  public string? EnvMap { get; set; }
  public string? InnerLayerMap { get; set; }
  public string? DisplacementMap { get; set; }
  public bool EnableEditorAlphaThreshold { get; set; }
  public bool Translucency { get; set; }
  public bool TranslucencyThickObject { get; set; }
  public bool TranslucencyMixAlbedoWithSubsurfaceColor { get; set; }
  public Vector3 TranslucencySubsurfaceColor { get; set; } = Vector3.Zero;
  public float TranslucencyTransmissiveScale { get; set; }
  public float TranslucencyTurbulence { get; set; }
  public bool RimLighting { get; set; }
  public float RimPower { get; set; }
  public float BackLightPower { get; set; }
  public bool SubsurfaceLighting { get; set; }
  public float SubsurfaceLightingRollOff { get; set; }
  public bool SpecularEnabled { get; set; }
  public Vector3 SpecularColor { get; set; } = Vector3.Zero;
  public float SpecularMult { get; set; }
  public float Smoothness { get; set; }
  public float FresnelPower { get; set; }
  public float WetnessControlSpecScale { get; set; }
  public float WetnessControlSpecPowerScale { get; set; }
  public float WetnessControlSpecMinVar { get; set; }
  public float WetnessControlEnvMapScale { get; set; }
  public float WetnessControlFresnelPower { get; set; }
  public float WetnessControlMetalness { get; set; }
  public bool PBR { get; set; }
  public bool CustomPorosity { get; set; }
  public float PorosityValue { get; set; }
  public string? RootMaterialPath { get; set; }
  public bool AnisoLighting { get; set; }
  public bool EmitEnabled { get; set; }
  public Vector3 EmitColor { get; set; } = Vector3.Zero;
  public float EmitMult { get; set; }
  public bool ModelSpaceNormals { get; set; }
  public bool ExternalEmit { get; set; }
  public float LumEmit { get; set; }
  public bool UseAdaptiveEmissive { get; set; }
  public Vector3 AdaptiveEmissiveExposureParams { get; set; }
  public bool BackLighting { get; set; }
  public bool ReceiveShadows { get; set; }
  public bool HideSecret { get; set; }
  public bool CastShadows { get; set; }
  public bool DissolveFade { get; set; }
  public bool AssumeShadowMask { get; set; }
  public bool GlowMapEnabled { get; set; }
  public bool EnvMapWindow { get; set; }
  public bool EnvMapEye { get; set; }
  public bool Hair { get; set; }
  public Vector3 HairTintColor { get; set; } = Vector3.Zero;
  public bool Tree { get; set; }
  public bool FaceGen { get; set; }
  public bool SkinTint { get; set; }
  public bool Tessellate { get; set; }
  public Vector2 DisplacementMapParams { get; set; } = Vector2.Zero;
  public Vector3 TessellationParams { get; set; } = Vector3.Zero;
  public new float GrayscaleToPaletteColor { get; set; }
  public bool SkewSpecularAlpha { get; set; }
  public bool Terrain { get; set; }
  public Vector3 TerrainParams { get; set; } = Vector3.Zero;

  public override void Read(BinaryReader reader)
  {
    base.Read(reader);
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