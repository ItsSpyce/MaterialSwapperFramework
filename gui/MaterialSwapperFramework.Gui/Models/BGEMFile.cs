using MaterialSwapperFramework.Gui.Extensions;
using Newtonsoft.Json;
using System.IO;
using System.Numerics;
using System.Text.Json.Serialization;

namespace MaterialSwapperFramework.Gui.Models;

public class BGEMFile : MaterialFileBase
{
  [JsonPropertyName("baseMap")]
  public string? BaseMap { get; set; }
  [JsonPropertyName("grayscaleMap")]
  public string? GrayscaleMap { get; set; }
  [JsonPropertyName("envMap")]
  public string? EnvMap { get; set; }
  [JsonPropertyName("normalMap")]
  public string? NormalMap { get; set; }
  [JsonPropertyName("envMapMask")]
  public string? EnvMapMask { get; set; }
  [JsonPropertyName("specularMap")]
  public string? SpecularMap { get; set; }
  [JsonPropertyName("lightingMap")]
  public string? LightingMap { get; set; }
  [JsonPropertyName("glowMap")]
  public string? GlowMap { get; set; }
  [JsonPropertyName("blood")]
  public bool Blood { get; set; }
  [JsonPropertyName("effectLighting")]
  public bool EffectLighting { get; set; }
  [JsonPropertyName("fallOff")]
  public bool FallOff { get; set; }
  [JsonPropertyName("fallOffColor")]
  public bool FallOffColor { get; set; }
  [JsonPropertyName("grayscaleToPaletteAlpha")]
  public bool GrayscaleToPaletteAlpha { get; set; }
  [JsonPropertyName("soft")]
  public bool Soft { get; set; }
  [JsonPropertyName("baseColor")]
  public Vector3 BaseColor { get; set; } = Vector3.Zero;
  [JsonPropertyName("baseColorScale")]
  public float BaseColorScale { get; set; }
  [JsonPropertyName("fallOffParams")]
  public Vector4 FallOffParams { get; set; } = Vector4.Zero;
  [JsonPropertyName("lightingInfluence")]
  public float LightingInfluence { get; set; }
  [JsonPropertyName("envMapMinLOD")]
  public byte EnvMapMinLOD { get; set; }
  [JsonPropertyName("softDepth")]
  public float SoftDepth { get; set; }
  [JsonPropertyName("emitColor")]
  public Vector3 EmitColor { get; set; } = Vector3.Zero;
  [JsonPropertyName("adaptiveEmissiveExposureParams")]
  public Vector3 AdaptiveEmissiveExposureParams { get; set; } = Vector3.Zero;
  [JsonPropertyName("glowMapEnabled")]
  public bool GlowMapEnabled { get; set; }
  [JsonPropertyName("effectPBRSpecular")]
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