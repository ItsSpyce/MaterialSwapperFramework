using System.Numerics;
using MaterialSwapperFramework.Gui.Extensions;

namespace MaterialSwapperFramework.Gui.Models;

public class BGEMFile : MaterialFileBase
{
  public string? BaseMap { get; set; }
  public string? GrayscaleMap { get; set; }
  public string? EnvMap { get; set; }
  public string? NormalMap { get; set; }
  public string? EnvMapMask { get; set; }
  public string? SpecularMap { get; set; }
  public string? LightingMap { get; set; }
  public string? GlowMap { get; set; }
  public bool Blood { get; set; }
  public bool EffectLighting { get; set; }
  public bool FallOff { get; set; }
  public bool FallOffColor { get; set; }
  public bool GrayscaleToPaletteAlpha { get; set; }
  public bool Soft { get; set; }
  public Vector3 BaseColor { get; set; } = Vector3.Zero;
  public float BaseColorScale { get; set; }
  public Vector4 FallOffParams { get; set; } = Vector4.Zero;
  public float LightingInfluence { get; set; }
  public byte EnvMapMinLOD { get; set; }
  public float SoftDepth { get; set; }
  public Vector3 EmitColor { get; set; } = Vector3.Zero;
  public Vector3 AdaptiveEmissiveExposureParams { get; set; } = Vector3.Zero;
  public bool GlowMapEnabled { get; set; }
  public bool EffectPBRSpecular { get; set; }

  public override void Read(BinaryReader reader)
  {
    base.Read(reader);
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