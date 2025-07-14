using System.IO;
using System.Runtime.Serialization;
using Newtonsoft.Json;

namespace MaterialSwapperFramework.Gui.Models
{
  [DataContract]
  public class BGEMFile : MaterialFileBase
  {
    public const uint Signature = 0x4D454742u;

    #region Fields
    protected override void SetDefaults()
    {
      base.SetDefaults();

      BaseTexture = "";
      GrayscaleTexture = "";
      EnvmapTexture = "";
      NormalTexture = "";
      EnvmapMaskTexture = "";
      SpecularTexture = "";
      LightingTexture = "";
      GlowTexture = "";

      BaseColor = 0xFFFFFFFFu;
      BaseColorScale = 1.0f;
      FalloffStartAngle = 1.0f;
      FalloffStopAngle = 1.0f;
      LightingInfluence = 1.0f;
      SoftDepth = 100.0f;
      EmittanceColor = 0xFFFFFFFFu;

      GlassRoughnessScratch = "";
      GlassDirtOverlay = "";

      GlassFresnelColor = 0xFFFFFFFFu;
      GlassRefractionScaleBase = 0.05f;
      GlassBlurScaleBase = 0.4f;
      GlassBlurScaleFactor = 1.0f;
    }
    #endregion

    public BGEMFile()
        : base(Signature)
    {
    }

    #region Properties

    public override ShaderType Type => ShaderType.Effect;

    [JsonProperty("baseMap")]
    public string? BaseTexture { get; set; }

    [JsonProperty("grayscaleTexture")]
    public string? GrayscaleTexture { get; set; }

    [JsonProperty("envMap")]
    public string? EnvmapTexture { get; set; }

    [JsonProperty("normalMap")]
    public string? NormalTexture { get; set; }

    [JsonProperty("envMapMask")]
    public string? EnvmapMaskTexture { get; set; }

    [JsonProperty("specularMap")]
    public string? SpecularTexture { get; set; }

    [JsonProperty("lightingMap")]
    public string? LightingTexture { get; set; }

    [JsonProperty("glowMap")]
    public string? GlowTexture { get; set; }

    [JsonProperty("bloodEnabled")]
    public bool BloodEnabled { get; set; }

    [JsonProperty("effectLightingEnabled")]
    public bool EffectLightingEnabled { get; set; }

    [JsonProperty("falloffEnabled")]
    public bool FalloffEnabled { get; set; }

    [JsonProperty("falloffColorEnabled")]
    public bool FalloffColorEnabled { get; set; }

    [JsonProperty("grayscaleToPaletteAlpha")]
    public bool GrayscaleToPaletteAlpha { get; set; }

    [JsonProperty("softEnabled")]
    public bool SoftEnabled { get; set; }

    public uint BaseColor { get; set; }

    [JsonProperty("baseColor")]
    private string BaseColorString
    {
      get => Color.FromUInt32(BaseColor).ToHexString();
      set => BaseColor = Color.FromHexString(value).ToUInt32();
    }

    [JsonProperty("baseColorScale")]
    public float BaseColorScale { get; set; }

    [JsonProperty("falloffStartAngle")]
    public float FalloffStartAngle { get; set; }

    [JsonProperty("falloffStopAngle")]
    public float FalloffStopAngle { get; set; }

    [JsonProperty("falloffStartOpacity")]
    public float FalloffStartOpacity { get; set; }

    [JsonProperty("falloffStopOpacity")]
    public float FalloffStopOpacity { get; set; }

    [JsonProperty("lightingInfluence")]
    public float LightingInfluence { get; set; }

    [JsonProperty("envmapMinLOD")]
    public byte EnvmapMinLOD { get; set; }

    [JsonProperty("softDepth")]
    public float SoftDepth { get; set; }

    public uint EmittanceColor { get; set; }

    [JsonProperty("emittanceColor")]
    private string EmittanceColorString
    {
      get => Color.FromUInt32(EmittanceColor).ToHexString();
      set => EmittanceColor = Color.FromHexString(value).ToUInt32();
    }

    [JsonProperty("adaptativeEmissive_ExposureOffset")]
    public float AdaptativeEmissive_ExposureOffset { get; set; }

    [JsonProperty("adaptativeEmissive_FinalExposureMin")]
    public float AdaptativeEmissive_FinalExposureMin { get; set; }

    [JsonProperty("adaptativeEmissive_FinalExposureMax")]
    public float AdaptativeEmissive_FinalExposureMax { get; set; }

    [JsonProperty("glowmap")]
    public bool Glowmap { get; set; }

    [JsonProperty("effectPbrSpecular")]
    public bool EffectPbrSpecular { get; set; }

    [JsonProperty("glassRoughnessScratch")]
    public string? GlassRoughnessScratch { get; set; }

    [JsonProperty("glassDirtOverlay")]
    public string? GlassDirtOverlay { get; set; }

    [JsonProperty("glassEnabled")]
    public bool GlassEnabled { get; set; }

    public uint GlassFresnelColor { get; set; }

    [JsonProperty("glassFresnelColor")]
    private string GlassFresnelColorString
    {
      get => Color.FromUInt32(GlassFresnelColor).ToHexString();
      set => GlassFresnelColor = Color.FromHexString(value).ToUInt32();
    }

    [JsonProperty("glassRefractionScaleBase")]
    public float GlassRefractionScaleBase { get; set; }

    [JsonProperty("glassBlurScaleBase")]
    public float GlassBlurScaleBase { get; set; }

    [JsonProperty("glassBlurScaleFactor")]
    public float GlassBlurScaleFactor { get; set; }
    #endregion

    public override void Deserialize(BinaryReader input)
    {
      base.Deserialize(input);

      BaseTexture = ReadString(input);
      GrayscaleTexture = ReadString(input);
      EnvmapTexture = ReadString(input);
      NormalTexture = ReadString(input);
      EnvmapMaskTexture = ReadString(input);

      if (Version >= 11)
      {
        SpecularTexture = ReadString(input);
        LightingTexture = ReadString(input);
        GlowTexture = ReadString(input);
      }

      if (Version >= 21)
      {
        GlassRoughnessScratch = ReadString(input);
        GlassDirtOverlay = ReadString(input);
        GlassEnabled = input.ReadBoolean();

        if (GlassEnabled)
        {
          GlassFresnelColor = Color.Read(input).ToUInt32(); // 1.0, 1.0, 1.0

          // FIXME: Order might be wrong
          GlassBlurScaleBase = input.ReadSingle(); // Occurring values: 0.05, 0.005, 0.01, -0.09, 0.0

          if (Version >= 22)
            GlassBlurScaleFactor = input.ReadSingle(); // Occurring values: 1.0, 0.0

          GlassRefractionScaleBase = input.ReadSingle(); // Occurring values: 0.4, 0.1, 0.01, 0.0
        }
      }

      if (Version >= 10)
      {
        EnvironmentMapping = input.ReadBoolean();
        EnvironmentMappingMaskScale = input.ReadSingle();
      }

      BloodEnabled = input.ReadBoolean();
      EffectLightingEnabled = input.ReadBoolean();
      FalloffEnabled = input.ReadBoolean();
      FalloffColorEnabled = input.ReadBoolean();
      GrayscaleToPaletteAlpha = input.ReadBoolean();
      SoftEnabled = input.ReadBoolean();

      BaseColor = Color.Read(input).ToUInt32();
      BaseColorScale = input.ReadSingle();

      FalloffStartAngle = input.ReadSingle();
      FalloffStopAngle = input.ReadSingle();
      FalloffStartOpacity = input.ReadSingle();
      FalloffStopOpacity = input.ReadSingle();

      LightingInfluence = input.ReadSingle();
      EnvmapMinLOD = input.ReadByte();
      SoftDepth = input.ReadSingle();

      if (Version >= 11)
      {
        EmittanceColor = Color.Read(input).ToUInt32();
      }

      if (Version >= 15)
      {
        AdaptativeEmissive_ExposureOffset = input.ReadSingle();
        AdaptativeEmissive_FinalExposureMin = input.ReadSingle();
        AdaptativeEmissive_FinalExposureMax = input.ReadSingle();
      }

      if (Version >= 16)
      {
        Glowmap = input.ReadBoolean();
      }

      if (Version >= 20)
      {
        EffectPbrSpecular = input.ReadBoolean();
      }
    }

    public override void Serialize(BinaryWriter output)
    {
      base.Serialize(output);

      WriteString(output, BaseTexture);
      WriteString(output, GrayscaleTexture);
      WriteString(output, EnvmapTexture);
      WriteString(output, NormalTexture);
      WriteString(output, EnvmapMaskTexture);

      if (Version >= 11)
      {
        WriteString(output, SpecularTexture);
        WriteString(output, LightingTexture);
        WriteString(output, GlowTexture);
      }

      if (Version >= 21)
      {
        WriteString(output, GlassRoughnessScratch);
        WriteString(output, GlassDirtOverlay);
        output.Write(GlassEnabled);

        if (GlassEnabled)
        {
          Color.FromUInt32(GlassFresnelColor).Write(output);

          // FIXME: Order might be wrong
          output.Write(GlassBlurScaleBase);

          if (Version >= 22)
            output.Write(GlassBlurScaleFactor);

          output.Write(GlassRefractionScaleBase);
        }
      }

      if (Version >= 10)
      {
        output.Write(EnvironmentMapping);
        output.Write(EnvironmentMappingMaskScale);
      }

      output.Write(BloodEnabled);
      output.Write(EffectLightingEnabled);
      output.Write(FalloffEnabled);
      output.Write(FalloffColorEnabled);
      output.Write(GrayscaleToPaletteAlpha);
      output.Write(SoftEnabled);

      Color.FromUInt32(BaseColor).Write(output);
      output.Write(BaseColorScale);

      output.Write(FalloffStartAngle);
      output.Write(FalloffStopAngle);
      output.Write(FalloffStartOpacity);
      output.Write(FalloffStopOpacity);

      output.Write(LightingInfluence);
      output.Write(EnvmapMinLOD);
      output.Write(SoftDepth);

      if (Version >= 11)
      {
        Color.FromUInt32(EmittanceColor).Write(output);
      }

      if (Version >= 15)
      {
        output.Write(AdaptativeEmissive_ExposureOffset);
        output.Write(AdaptativeEmissive_FinalExposureMin);
        output.Write(AdaptativeEmissive_FinalExposureMax);
      }

      if (Version >= 16)
      {
        output.Write(Glowmap);
      }

      if (Version >= 20)
      {
        output.Write(EffectPbrSpecular);
      }
    }
  }
}