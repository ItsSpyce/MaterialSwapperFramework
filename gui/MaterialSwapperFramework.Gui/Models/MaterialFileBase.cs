using System.Numerics;
using MaterialSwapperFramework.Gui.Extensions;

namespace MaterialSwapperFramework.Gui.Models;

public abstract class MaterialFileBase
{
  public enum ShaderType
  {
    Lighting,
    Effect,
  }

  public ShaderType Type { get; set; }
  public uint Version { get; set; }
  public uint Clamp { get; set; }
  public Vector2 UVOffset { get; set; }
  public Vector2 UVScale { get; set; }
  public float Transparency { get; set; }
  public bool AlphaBlend { get; set; }
  public uint SourceBlendMode { get; set; }
  public uint DestinationBlendMode { get; set; }
  public byte AlphaTestThreshold { get; set; }
  public bool AlphaTest { get; set; }
  public bool DepthWrite { get; set; }
  public bool DepthTest { get; set; }
  public bool SSR { get; set; }
  public bool WetnessControlSSR { get; set; }
  public bool Decal { get; set; }
  public bool TwoSided { get; set; }
  public bool DecalNoFade { get; set; }
  public bool NonOccluder { get; set; }
  public bool Refraction { get; set; }
  public bool RefractionalFallOff { get; set; }
  public float RefractionPower { get; set; }
  public bool EnvMapEnabled { get; set; }
  public float EnvMapMaskScale { get; set; }
  public bool DepthBias { get; set; }
  public bool GrayscaleToPaletteColor { get; set; }
  public byte MaskWrites { get; set; }

  public virtual void Read(BinaryReader reader)
  {
    Version = reader.ReadUInt32();
    Clamp = reader.ReadUInt32();
    UVOffset = reader.ReadVec2();
    UVScale = reader.ReadVec2();
    Transparency = reader.ReadSingle();
    AlphaBlend = reader.ReadBoolean();
    SourceBlendMode = reader.ReadUInt32();
    DestinationBlendMode = reader.ReadUInt32();
    AlphaTestThreshold = reader.ReadByte();
    AlphaTest = reader.ReadBoolean();
    DepthWrite = reader.ReadBoolean();
    DepthTest = reader.ReadBoolean();
    SSR = reader.ReadBoolean();
    WetnessControlSSR = reader.ReadBoolean();
    Decal = reader.ReadBoolean();
    TwoSided = reader.ReadBoolean();
    DecalNoFade = reader.ReadBoolean();
    NonOccluder = reader.ReadBoolean();
    Refraction = reader.ReadBoolean();
    RefractionalFallOff = reader.ReadBoolean();
    RefractionPower = reader.ReadSingle();
    if (Version < 10)
    {
      EnvMapEnabled = reader.ReadBoolean();
      EnvMapMaskScale = reader.ReadSingle();
    }
    else
    {
      DepthBias = reader.ReadBoolean();
    }
    GrayscaleToPaletteColor = reader.ReadBoolean();
    if (Version >= 6)
    {
      MaskWrites = reader.ReadByte();
    }
  }
}
