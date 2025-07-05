using System.IO;
using System.Numerics;
using System.Text.Json.Serialization;
using MaterialSwapperFramework.Gui.Extensions;
using Newtonsoft.Json;

namespace MaterialSwapperFramework.Gui.Models;

[JsonObject(MemberSerialization.OptIn)]
public abstract class MaterialFileBase
{
  public enum ShaderType
  {
    Lighting,
    Effect,
  }

  [JsonPropertyName("type")]
  public ShaderType Type { get; set; }
  [JsonPropertyName("version")]
  public uint Version { get; set; }
  [JsonPropertyName("clamp")]
  public uint Clamp { get; set; }
  [JsonPropertyName("uvOffset")]
  public Vector2 UVOffset { get; set; }
  [JsonPropertyName("uvScale")]
  public Vector2 UVScale { get; set; }
  [JsonPropertyName("transparency")]
  public float Transparency { get; set; }
  [JsonPropertyName("alphaBlend")]
  public bool AlphaBlend { get; set; }
  [JsonPropertyName("sourceBlendMode")]
  public uint SourceBlendMode { get; set; }
  [JsonPropertyName("destinationBlendMode")]
  public uint DestinationBlendMode { get; set; }
  [JsonPropertyName("alphaTestThreshold")]
  public byte AlphaTestThreshold { get; set; }
  [JsonPropertyName("alphaTest")]
  public bool AlphaTest { get; set; }
  [JsonPropertyName("depthWrite")]
  public bool DepthWrite { get; set; }
  [JsonPropertyName("depthTest")]
  public bool DepthTest { get; set; }
  [JsonPropertyName("ssr")]
  public bool SSR { get; set; }
  [JsonPropertyName("wetnessControlSsr")]
  public bool WetnessControlSSR { get; set; }
  [JsonPropertyName("decal")]
  public bool Decal { get; set; }
  [JsonPropertyName("twoSided")]
  public bool TwoSided { get; set; }
  [JsonPropertyName("decalNoFade")]
  public bool DecalNoFade { get; set; }
  [JsonPropertyName("nonOccluder")]
  public bool NonOccluder { get; set; }
  [JsonPropertyName("refraction")]
  public bool Refraction { get; set; }
  [JsonPropertyName("refractionalFallOff")]
  public bool RefractionalFallOff { get; set; }
  [JsonPropertyName("refractionPower")]
  public float RefractionPower { get; set; }
  [JsonPropertyName("envMapEnabled")]
  public bool EnvMapEnabled { get; set; }
  [JsonPropertyName("envMapMaskScale")]
  public float EnvMapMaskScale { get; set; }
  [JsonPropertyName("depthBias")]
  public bool DepthBias { get; set; }
  [JsonPropertyName("grayscaleToPaletteColor")]
  public bool GrayscaleToPaletteColor { get; set; }
  [JsonPropertyName("maskWrites")]
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
