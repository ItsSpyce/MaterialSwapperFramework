using MaterialSwapperFramework.Gui.Extensions;
using Newtonsoft.Json;
using System.IO;
using System.Numerics;

namespace MaterialSwapperFramework.Gui.Models;

public class BGEMFile : MaterialFileBase
{
  [JsonProperty]
  public string? BaseMap { get; set; }
  [JsonProperty]
  public string? GrayscaleMap { get; set; }
  [JsonProperty]
  public string? EnvMap { get; set; }
  [JsonProperty]
  public string? NormalMap { get; set; }
  [JsonProperty]
  public string? EnvMapMask { get; set; }
  [JsonProperty]
  public string? SpecularMap { get; set; }
  [JsonProperty]
  public string? LightingMap { get; set; }
  [JsonProperty]
  public string? GlowMap { get; set; }
  [JsonProperty]
  public bool Blood { get; set; }
  [JsonProperty]
  public bool EffectLighting { get; set; }
  [JsonProperty]
  public bool FallOff { get; set; }
  [JsonProperty]
  public bool FallOffColor { get; set; }
  [JsonProperty]
  public bool GrayscaleToPaletteAlpha { get; set; }
  [JsonProperty]
  public bool Soft { get; set; }
  [JsonProperty]
  public Vector3 BaseColor { get; set; } = Vector3.Zero;
  [JsonProperty]
  public float BaseColorScale { get; set; }
  [JsonProperty]
  public Vector4 FallOffParams { get; set; } = Vector4.Zero;
  [JsonProperty]
  public float LightingInfluence { get; set; }
  [JsonProperty]
  public byte EnvMapMinLOD { get; set; }
  [JsonProperty]
  public float SoftDepth { get; set; }
  [JsonProperty]
  public Vector3 EmitColor { get; set; } = Vector3.Zero;
  [JsonProperty]
  public Vector3 AdaptiveEmissiveExposureParams { get; set; } = Vector3.Zero;
  [JsonProperty]
  public bool GlowMapEnabled { get; set; }
  [JsonProperty]
  public bool EffectPBRSpecular { get; set; }

  public override void ReadFromStream(BinaryReader reader)
  {
    base.ReadFromStream(reader);
    BaseMap = reader.ReadNullTerminatedString();
    GrayscaleMap = reader.ReadNullTerminatedString();
    EnvMap = reader.ReadNullTerminatedString();
    NormalMap = reader.ReadNullTerminatedString();
    EnvMapMask = reader.ReadNullTerminatedString();

    if (Version >= 11)
    {
      SpecularMap = reader.ReadNullTerminatedString();
      LightingMap = reader.ReadNullTerminatedString();
      GlowMap = reader.ReadNullTerminatedString();
    }

    if (Version >= 10)
    {
      EnvMapEnabled = reader.ReadBoolean();
      EnvMapMaskScale = reader.ReadSingle();
    }
    Blood = reader.ReadBoolean();
    EffectLighting = reader.ReadBoolean();
    FallOff = reader.ReadBoolean();
    FallOffColor = reader.ReadBoolean();
    GrayscaleToPaletteAlpha = reader.ReadBoolean();
    Soft = reader.ReadBoolean();
    BaseColor = reader.ReadVec3();
    BaseColorScale = reader.ReadSingle();
    FallOffParams = reader.ReadVec4();
    LightingInfluence = reader.ReadSingle();
    EnvMapMinLOD = reader.ReadByte();
    SoftDepth = reader.ReadSingle();

    if (Version >= 11)
    {
      EmitColor = reader.ReadVec3();
    }

    if (Version >= 15)
    {
      AdaptiveEmissiveExposureParams = reader.ReadVec3();
    }

    if (Version >= 16)
    {
      GlowMapEnabled = reader.ReadBoolean();
    }

    if (Version >= 20)
    {
      EffectPBRSpecular = reader.ReadBoolean();
    }
  }
}