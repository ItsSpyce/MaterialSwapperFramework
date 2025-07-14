using System.IO;
using System.Runtime.Serialization;
using MaterialSwapperFramework.Gui.Converters;
using Newtonsoft.Json;

namespace MaterialSwapperFramework.Gui.Models
{
  [DataContract]
  public class BGSMFile : MaterialFileBase
  {
    public const uint Signature = 0x4D534742u;

    protected override void SetDefaults()
    {
      base.SetDefaults();

      DiffuseTexture = "";
      NormalTexture = "";
      SmoothSpecTexture = "";
      GreyscaleTexture = "";
      EnvmapTexture = "";
      GlowTexture = "";
      InnerLayerTexture = "";
      WrinklesTexture = "";
      DisplacementTexture = "";
      SpecularTexture = "";
      LightingTexture = "";
      FlowTexture = "";
      DistanceFieldAlphaTexture = "";
      RimPower = 2.0f;
      SubsurfaceLightingRolloff = 0.3f;
      SpecularColor = 0xFFFFFFFFu;
      SpecularMult = 1.0f;
      Smoothness = 1.0f;
      FresnelPower = 5.0f;
      WetnessControlSpecScale = -1.0f;
      WetnessControlSpecPowerScale = -1.0f;
      WetnessControlSpecMinvar = -1.0f;
      WetnessControlEnvMapScale = -1.0f;
      WetnessControlFresnelPower = -1.0f;
      WetnessControlMetalness = -1.0f;
      RootMaterialPath = "";
      EmittanceColor = 0xFFFFFFFFu;
      EmittanceMult = 1.0f;
      HairTintColor = 0x808080u;
      DisplacementTextureBias = -0.5f;
      DisplacementTextureScale = 10.0f;
      TessellationPnScale = 1.0f;
      TessellationBaseFactor = 1.0f;
      GrayscaleToPaletteScale = 1.0f;
    }

    public BGSMFile()
        : base(Signature)
    {
    }
    #region Fields

    public override ShaderType Type => ShaderType.Lighting;

    [JsonProperty("diffuseMap")]
    public string? DiffuseTexture { get; set; }

    [JsonProperty("normalMap")]
    public string? NormalTexture { get; set; }

    [JsonProperty("smoothSpecMap")]
    public string? SmoothSpecTexture { get; set; }

    [JsonProperty("grayscaleMap")]
    public string? GreyscaleTexture { get; set; }

    [JsonProperty("envMap")]
    public string? EnvmapTexture { get; set; }

    [JsonProperty("glowMap")]
    public string? GlowTexture { get; set; }

    [JsonProperty("innerLayerMap")]
    public string? InnerLayerTexture { get; set; }

    [JsonProperty("wrinklesMap")]
    public string? WrinklesTexture { get; set; }

    [JsonProperty("displacementMap")]
    public string? DisplacementTexture { get; set; }

    [JsonProperty("specularMap")]
    public string? SpecularTexture { get; set; }

    [JsonProperty("lightingMap")]
    public string? LightingTexture { get; set; }

    [JsonProperty("flowMap")]
    public string? FlowTexture { get; set; }

    [JsonProperty("distanceFieldAlphaMap")]
    public string? DistanceFieldAlphaTexture { get; set; }

    [JsonProperty("enableEditorAlphaRef")]
    public bool EnableEditorAlphaRef { get; set; }

    [JsonProperty("rimLighting")]
    public bool RimLighting { get; set; }

    [JsonProperty("rimPower")]
    public float RimPower { get; set; }

    [JsonProperty("backLightPower")]
    public float BackLightPower { get; set; }

    [JsonProperty("subsurfaceLighting")]
    public bool SubsurfaceLighting { get; set; }

    [JsonProperty("subsurfaceLightingRolloff")]
    public float SubsurfaceLightingRolloff { get; set; }

    [JsonProperty("specularEnabled")]
    public bool SpecularEnabled { get; set; }

    public uint SpecularColor { get; set; }

    [JsonProperty("specularColor")]
    [JsonConverter(typeof(ByteArrayConverter))]
    private byte[] SpecularColorVector
    {
      get =>
      [
        (byte)(SpecularColor >> 16),
        (byte)(SpecularColor >> 8),
        (byte)SpecularColor
      ];
      set => SpecularColor = 
      (
        (uint)value[1] << 16 |
        (uint)value[2] << 8 |
        (uint)value[3]
      );
    }

    [JsonProperty("specularMult")]
    public float SpecularMult { get; set; }

    [JsonProperty("smoothness")]
    public float Smoothness { get; set; }

    [JsonProperty("fresnelPower")]
    public float FresnelPower { get; set; }

    [JsonProperty("wetnessControlSpecScale")]
    public float WetnessControlSpecScale { get; set; }

    [JsonProperty("wetnessControlSpecPowerScale")]
    public float WetnessControlSpecPowerScale { get; set; }

    [JsonProperty("wetnessControlSpecMinvar")]
    public float WetnessControlSpecMinvar { get; set; }

    [JsonProperty("wetnessControlEnvMapScale")]
    public float WetnessControlEnvMapScale { get; set; }

    [JsonProperty("wetnessControlFresnelPower")]
    public float WetnessControlFresnelPower { get; set; }

    [JsonProperty("wetnessControlMetalness")]
    public float WetnessControlMetalness { get; set; }

    [JsonProperty("rootMaterialPath")]
    public string? RootMaterialPath { get; set; }

    [JsonProperty("anisoLighting")]
    public bool AnisoLighting { get; set; }

    [JsonProperty("emitEnabled")]
    public bool EmitEnabled { get; set; }

    public uint EmittanceColor { get; set; }

    [JsonProperty("emittanceColor")]
    [JsonConverter(typeof(ByteArrayConverter))]
    private byte[] EmittanceColorVector
    {
      get =>
      [
        (byte)(EmittanceColor >> 24),
        (byte)(EmittanceColor >> 16),
        (byte)(EmittanceColor >> 8),
        (byte)EmittanceColor
      ];
      set => EmittanceColor = 
      (
        (uint)value[0] << 24 |
        (uint)value[1] << 16 |
        (uint)value[2] << 8 |
        (uint)value[3]
      );
    }

    [JsonProperty("emittanceMult")]
    public float EmittanceMult { get; set; }

    [JsonProperty("modelSpaceNormals")]
    public bool ModelSpaceNormals { get; set; }

    [JsonProperty("externalEmittance")]
    public bool ExternalEmittance { get; set; }

    [JsonProperty("backLighting")]
    public bool BackLighting { get; set; }

    [JsonProperty("receiveShadows")]
    public bool ReceiveShadows { get; set; }

    [JsonProperty("hideSecret")]
    public bool HideSecret { get; set; }

    [JsonProperty("castShadows")]
    public bool CastShadows { get; set; }

    [JsonProperty("dissolveFade")]
    public bool DissolveFade { get; set; }

    [JsonProperty("assumeShadowmask")]
    public bool AssumeShadowmask { get; set; }

    [JsonProperty("glowmap")]
    public bool Glowmap { get; set; }

    [JsonProperty("environmentMappingWindow")]
    public bool EnvironmentMappingWindow { get; set; }

    [JsonProperty("environmentMappingEye")]
    public bool EnvironmentMappingEye { get; set; }

    [JsonProperty("hair")]
    public bool Hair { get; set; }

    public uint HairTintColor { get; set; }

    [JsonProperty("hairTintColor")]
    [JsonConverter(typeof(ByteArrayConverter))]
    private byte[] HairTintColorVector
    {
      get =>
      [
        (byte)(HairTintColor >> 16),
        (byte)(HairTintColor >> 8),
        (byte)HairTintColor,
      ];
      set => HairTintColor = 
      (
        (uint)value[0] << 16 |
        (uint)value[1] << 8 |
        (uint)value[2]
      );
    }

    [JsonProperty("tree")]
    public bool Tree { get; set; }

    [JsonProperty("facegen")]
    public bool Facegen { get; set; }

    [JsonProperty("skinTint")]
    public bool SkinTint { get; set; }

    [JsonProperty("tessellate")]
    public bool Tessellate { get; set; }

    [JsonProperty("displacementTextureBias")]
    public float DisplacementTextureBias { get; set; }

    [JsonProperty("displacementTextureScale")]
    public float DisplacementTextureScale { get; set; }

    [JsonProperty("tessellationPnScale")]
    public float TessellationPnScale { get; set; }

    [JsonProperty("tessellationBaseFactor")]
    public float TessellationBaseFactor { get; set; }

    [JsonProperty("tessellationFadeDistance")]
    public float TessellationFadeDistance { get; set; }

    [JsonProperty("grayscaleToPaletteScale")]
    public float GrayscaleToPaletteScale { get; set; }

    [JsonProperty("skewSpecularAlpha")]
    public bool SkewSpecularAlpha { get; set; }

    [JsonProperty("translucency")]
    public bool Translucency { get; set; }

    public uint TranslucencySubsurfaceColor { get; set; }

    [JsonProperty("translucencySubsurfaceColor")]
    [JsonConverter(typeof(ByteArrayConverter))]
    private byte[] TranslucencySubsurfaceColorVector
    {
      get =>
      [
        (byte)(TranslucencySubsurfaceColor >> 24),
        (byte)(TranslucencySubsurfaceColor >> 16),
        (byte)(TranslucencySubsurfaceColor >> 8),
        (byte)TranslucencySubsurfaceColor,
      ];
      set => TranslucencySubsurfaceColor = 
      (
        (uint)value[0] << 24 |
        (uint)value[1] << 16 |
        (uint)value[2] << 8 |
        (uint)value[3]
      );
    }

    [JsonProperty("translucencyTransmissiveScale")]
    public float TranslucencyTransmissiveScale { get; set; }

    [JsonProperty("translucencyTurbulence")]
    public float TranslucencyTurbulence { get; set; }

    [JsonProperty("pbr")]
    public bool PBR { get; set; }

    [JsonProperty("customPorosity")]
    public bool CustomPorosity { get; set; }

    [JsonProperty("porosityValue")]
    public float PorosityValue { get; set; }

    [JsonProperty("lumEmittance")]
    public float LumEmittance { get; set; }

    [JsonProperty("translucencyThickObject")]
    public bool TranslucencyThickObject { get; set; }

    [JsonProperty("translucencyMixAlbedoWithSubsurfaceColor")]
    public bool TranslucencyMixAlbedoWithSubsurfaceColor { get; set; }

    [JsonProperty("useAdaptativeEmissive")]
    public bool UseAdaptativeEmissive { get; set; }

    [JsonProperty("adaptativeEmissiveExposureOffset")]
    public float AdaptativeEmissive_ExposureOffset { get; set; }

    [JsonProperty("adaptativeEmissiveFinalExposureMin")]
    public float AdaptativeEmissive_FinalExposureMin { get; set; }

    [JsonProperty("adaptativeEmissiveFinalExposureMax")]
    public float AdaptativeEmissive_FinalExposureMax { get; set; }

    [JsonProperty("terrain")]
    public bool Terrain { get; set; }

    [JsonProperty("terrainThresholdFalloff")]
    public float TerrainThresholdFalloff { get; set; }

    [JsonProperty("terrainTilingDistance")]
    public float TerrainTilingDistance { get; set; }

    [JsonProperty("terrainRotationAngle")]
    public float TerrainRotationAngle { get; set; }

    [JsonProperty("unkInt1")]
    public uint UnkInt1 { get; set; }
    #endregion

    public override void Deserialize(BinaryReader input)
    {
      base.Deserialize(input);

      DiffuseTexture = ReadString(input);
      NormalTexture = ReadString(input);
      SmoothSpecTexture = ReadString(input);
      GreyscaleTexture = ReadString(input);

      if (Version > 2)
      {
        GlowTexture = ReadString(input);
        WrinklesTexture = ReadString(input);
        SpecularTexture = ReadString(input);
        LightingTexture = ReadString(input);
        FlowTexture = ReadString(input);

        if (Version >= 17)
        {
          DistanceFieldAlphaTexture = ReadString(input);
        }
      }
      else
      {
        EnvmapTexture = ReadString(input);
        GlowTexture = ReadString(input);
        InnerLayerTexture = ReadString(input);
        WrinklesTexture = ReadString(input);
        DisplacementTexture = ReadString(input);
      }

      EnableEditorAlphaRef = input.ReadBoolean();

      if (Version >= 8)
      {
        Translucency = input.ReadBoolean();
        TranslucencyThickObject = input.ReadBoolean();
        TranslucencyMixAlbedoWithSubsurfaceColor = input.ReadBoolean();
        TranslucencySubsurfaceColor = Color.Read(input).ToUInt32();
        TranslucencyTransmissiveScale = input.ReadSingle();
        TranslucencyTurbulence = input.ReadSingle();
      }
      else
      {
        RimLighting = input.ReadBoolean();
        RimPower = input.ReadSingle();
        BackLightPower = input.ReadSingle();

        SubsurfaceLighting = input.ReadBoolean();
        SubsurfaceLightingRolloff = input.ReadSingle();
      }

      SpecularEnabled = input.ReadBoolean();
      SpecularColor = Color.Read(input).ToUInt32();
      SpecularMult = input.ReadSingle();
      Smoothness = input.ReadSingle();

      FresnelPower = input.ReadSingle();
      WetnessControlSpecScale = input.ReadSingle();
      WetnessControlSpecPowerScale = input.ReadSingle();
      WetnessControlSpecMinvar = input.ReadSingle();

      if (Version < 10)
      {
        WetnessControlEnvMapScale = input.ReadSingle();
      }

      WetnessControlFresnelPower = input.ReadSingle();
      WetnessControlMetalness = input.ReadSingle();

      if (Version > 2)
      {
        PBR = input.ReadBoolean();

        if (Version >= 9)
        {
          CustomPorosity = input.ReadBoolean();
          PorosityValue = input.ReadSingle();
        }
      }

      RootMaterialPath = ReadString(input);

      AnisoLighting = input.ReadBoolean();
      EmitEnabled = input.ReadBoolean();

      if (EmitEnabled)
      {
        EmittanceColor = Color.Read(input).ToUInt32();
      }

      EmittanceMult = input.ReadSingle();
      ModelSpaceNormals = input.ReadBoolean();
      ExternalEmittance = input.ReadBoolean();

      if (Version >= 12)
      {
        LumEmittance = input.ReadSingle();
      }

      if (Version >= 13)
      {
        UseAdaptativeEmissive = input.ReadBoolean();
        AdaptativeEmissive_ExposureOffset = input.ReadSingle();
        AdaptativeEmissive_FinalExposureMin = input.ReadSingle();
        AdaptativeEmissive_FinalExposureMax = input.ReadSingle();
      }

      if (Version < 8)
      {
        BackLighting = input.ReadBoolean();
      }

      ReceiveShadows = input.ReadBoolean();
      HideSecret = input.ReadBoolean();
      CastShadows = input.ReadBoolean();
      DissolveFade = input.ReadBoolean();
      AssumeShadowmask = input.ReadBoolean();

      Glowmap = input.ReadBoolean();

      if (Version < 7)
      {
        EnvironmentMappingWindow = input.ReadBoolean();
        EnvironmentMappingEye = input.ReadBoolean();
      }

      Hair = input.ReadBoolean();
      HairTintColor = Color.Read(input).ToUInt32();

      Tree = input.ReadBoolean();
      Facegen = input.ReadBoolean();
      SkinTint = input.ReadBoolean();
      Tessellate = input.ReadBoolean();

      if (Version < 3)
      {
        DisplacementTextureBias = input.ReadSingle();
        DisplacementTextureScale = input.ReadSingle();
        TessellationPnScale = input.ReadSingle();
        TessellationBaseFactor = input.ReadSingle();
        TessellationFadeDistance = input.ReadSingle();
      }

      GrayscaleToPaletteScale = input.ReadSingle();

      if (Version >= 1)
      {
        SkewSpecularAlpha = input.ReadBoolean();
      }

      if (Version >= 3)
      {
        Terrain = input.ReadBoolean();

        if (Terrain)
        {
          if (Version == 3)
          {
            UnkInt1 = input.ReadUInt32();
          }

          TerrainThresholdFalloff = input.ReadSingle();
          TerrainTilingDistance = input.ReadSingle();
          TerrainRotationAngle = input.ReadSingle();
        }
      }
    }

    public override void Serialize(BinaryWriter output)
    {
      base.Serialize(output);

      WriteString(output, DiffuseTexture);
      WriteString(output, NormalTexture);
      WriteString(output, SmoothSpecTexture);
      WriteString(output, GreyscaleTexture);

      if (Version > 2)
      {
        WriteString(output, GlowTexture);
        WriteString(output, WrinklesTexture);
        WriteString(output, SpecularTexture);
        WriteString(output, LightingTexture);
        WriteString(output, FlowTexture);

        if (Version >= 17)
        {
          WriteString(output, DistanceFieldAlphaTexture);
        }
      }
      else
      {
        WriteString(output, EnvmapTexture);
        WriteString(output, GlowTexture);
        WriteString(output, InnerLayerTexture);
        WriteString(output, WrinklesTexture);
        WriteString(output, DisplacementTexture);
      }

      output.Write(EnableEditorAlphaRef);

      if (Version >= 8)
      {
        output.Write(Translucency);
        output.Write(TranslucencyThickObject);
        output.Write(TranslucencyMixAlbedoWithSubsurfaceColor);
        Color.FromUInt32(TranslucencySubsurfaceColor).Write(output);
        output.Write(TranslucencyTransmissiveScale);
        output.Write(TranslucencyTurbulence);
      }
      else
      {
        output.Write(RimLighting);
        output.Write(RimPower);
        output.Write(BackLightPower);

        output.Write(SubsurfaceLighting);
        output.Write(SubsurfaceLightingRolloff);
      }

      output.Write(SpecularEnabled);
      Color.FromUInt32(SpecularColor).Write(output);
      output.Write(SpecularMult);
      output.Write(Smoothness);

      output.Write(FresnelPower);
      output.Write(WetnessControlSpecScale);
      output.Write(WetnessControlSpecPowerScale);
      output.Write(WetnessControlSpecMinvar);

      if (Version < 10)
      {
        output.Write(WetnessControlEnvMapScale);
      }

      output.Write(WetnessControlFresnelPower);
      output.Write(WetnessControlMetalness);

      if (Version > 2)
      {
        output.Write(PBR);

        if (Version >= 9)
        {
          output.Write(CustomPorosity);
          output.Write(PorosityValue);
        }
      }

      WriteString(output, RootMaterialPath);

      output.Write(AnisoLighting);
      output.Write(EmitEnabled);

      if (EmitEnabled)
      {
        Color.FromUInt32(EmittanceColor).Write(output);
      }

      output.Write(EmittanceMult);
      output.Write(ModelSpaceNormals);
      output.Write(ExternalEmittance);

      if (Version >= 12)
      {
        output.Write(LumEmittance);
      }

      if (Version >= 13)
      {
        output.Write(UseAdaptativeEmissive);
        output.Write(AdaptativeEmissive_ExposureOffset);
        output.Write(AdaptativeEmissive_FinalExposureMin);
        output.Write(AdaptativeEmissive_FinalExposureMax);
      }

      if (Version < 8)
      {
        output.Write(BackLighting);
      }

      output.Write(ReceiveShadows);
      output.Write(HideSecret);
      output.Write(CastShadows);
      output.Write(DissolveFade);
      output.Write(AssumeShadowmask);

      output.Write(Glowmap);

      if (Version < 7)
      {
        output.Write(EnvironmentMappingWindow);
        output.Write(EnvironmentMappingEye);
      }

      output.Write(Hair);
      Color.FromUInt32(HairTintColor).Write(output);

      output.Write(Tree);
      output.Write(Facegen);
      output.Write(SkinTint);
      output.Write(Tessellate);

      if (Version < 3)
      {
        output.Write(DisplacementTextureBias);
        output.Write(DisplacementTextureScale);
        output.Write(TessellationPnScale);
        output.Write(TessellationBaseFactor);
        output.Write(TessellationFadeDistance);
      }

      output.Write(GrayscaleToPaletteScale);

      if (Version >= 1)
      {
        output.Write(SkewSpecularAlpha);
      }

      if (Version >= 3)
      {
        output.Write(Terrain);

        if (Terrain)
        {
          if (Version == 3)
          {
            output.Write(UnkInt1);
          }

          output.Write(TerrainThresholdFalloff);
          output.Write(TerrainTilingDistance);
          output.Write(TerrainRotationAngle);
        }
      }
    }
  }
}