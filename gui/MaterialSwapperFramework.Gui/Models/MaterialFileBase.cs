using System.IO;
using System.Numerics;
using FluentResults;
using MaterialSwapperFramework.Gui.Extensions;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace MaterialSwapperFramework.Gui.Models;

[JsonObject(MemberSerialization.OptIn)]
public abstract class MaterialFileBase
{
  public enum ShaderType
  {
    Lighting,
    Effect,
  }

  [JsonProperty]
  public ShaderType Type { get; set; }
  [JsonProperty]
  public uint Version { get; set; }
  [JsonProperty]
  public uint Clamp { get; set; }
  [JsonProperty]
  public Vector2 UVOffset { get; set; }
  [JsonProperty]
  public Vector2 UVScale { get; set; }
  [JsonProperty]
  public float Transparency { get; set; }
  [JsonProperty]
  public bool AlphaBlend { get; set; }
  [JsonProperty]
  public uint SourceBlendMode { get; set; }
  [JsonProperty]
  public uint DestinationBlendMode { get; set; }
  [JsonProperty]
  public byte AlphaTestThreshold { get; set; }
  [JsonProperty]
  public bool AlphaTest { get; set; }
  [JsonProperty]
  public bool DepthWrite { get; set; }
  [JsonProperty]
  public bool DepthTest { get; set; }
  [JsonProperty]
  public bool SSR { get; set; }
  [JsonProperty]
  public bool WetnessControlSSR { get; set; }
  [JsonProperty]
  public bool Decal { get; set; }
  [JsonProperty]
  public bool TwoSided { get; set; }
  [JsonProperty]
  public bool DecalNoFade { get; set; }
  [JsonProperty]
  public bool NonOccluder { get; set; }
  [JsonProperty]
  public bool Refraction { get; set; }
  [JsonProperty]
  public bool RefractionalFallOff { get; set; }
  [JsonProperty]
  public float RefractionPower { get; set; }
  [JsonProperty]
  public bool EnvMapEnabled { get; set; }
  [JsonProperty]
  public float EnvMapMaskScale { get; set; }
  [JsonProperty]
  public bool DepthBias { get; set; }
  [JsonProperty]
  public bool GrayscaleToPaletteColor { get; set; }
  [JsonProperty]
  public byte MaskWrites { get; set; }

  public virtual void ReadFromStream(BinaryReader reader)
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

  public virtual void ReadFromJson(string json)
  {
    JsonConvert.PopulateObject(json, this);
  }
}
