using System;
using System.Globalization;
using System.IO;
using System.Numerics;
using System.Runtime.Serialization;
using MaterialSwapperFramework.Gui.Converters;
using Newtonsoft.Json;

namespace MaterialSwapperFramework.Gui.Models
{
  // https://github.com/ousnius/Material-Editor/blob/master/MaterialLib/BaseMaterialFile.cs
  [DataContract]
  public abstract class MaterialFileBase
  {
    private readonly uint _Signature;

    #region Fields
    protected virtual void SetDefaults()
    {
      Version = 2;
      TileU = true;
      TileV = true;
      UVScale = [1.0f, 1.0f];
      Alpha = 1.0f;
      AlphaTestRef = 128;
      ZBufferWrite = true;
      ZBufferTest = true;
      EnvironmentMappingMaskScale = 1.0f;
      MaskWrites = MaskWriteFlags.ALBEDO | MaskWriteFlags.NORMAL | MaskWriteFlags.SPECULAR | MaskWriteFlags.AMBIENT_OCCLUSION | MaskWriteFlags.EMISSIVE | MaskWriteFlags.GLOSS;
    }
    #endregion

    protected MaterialFileBase(uint signature)
    {
      _Signature = signature;
      SetDefaults();
    }

    #region Properties
    [JsonProperty("shaderType")]
    public abstract ShaderType Type { get; }

    public uint Version { get; set; }

    [JsonProperty("tileU")]
    public bool TileU { get; set; }

    [JsonProperty("tileV")]
    public bool TileV { get; set; }

    [JsonProperty("uvOffset")]
    public float[] UVOffset { get; set; }

    [JsonProperty("uvScale")]
    public float[] UVScale { get; set; }

    [JsonProperty("transparency")]
    public float Alpha { get; set; }

    public AlphaBlendModeType AlphaBlendMode { get; set; }

    [JsonProperty("alphaBlendMode")]
    private string AlphaBlendModeString
    {
      get => AlphaBlendMode.ToString();
      set => AlphaBlendMode = Enum.TryParse(value, true, out AlphaBlendModeType mode) ? mode : AlphaBlendModeType.None;
    }

    [JsonProperty("alphaTestRef")]
    public byte AlphaTestRef { get; set; }

    [JsonProperty("alphaTest")]
    public bool AlphaTest { get; set; }

    [JsonProperty("zBufferWrite")]
    public bool ZBufferWrite { get; set; }

    [JsonProperty("zBufferTest")]
    public bool ZBufferTest { get; set; }

    [JsonProperty("screenSpaceReflections")]
    public bool ScreenSpaceReflections { get; set; }

    [JsonProperty("wetnessControlScreenSpaceReflections")]
    public bool WetnessControlScreenSpaceReflections { get; set; }

    [JsonProperty("decal")]
    public bool Decal { get; set; }

    [JsonProperty("twoSided")]
    public bool TwoSided { get; set; }

    [JsonProperty("decalNoFade")]
    public bool DecalNoFade { get; set; }

    [JsonProperty("nonOccluder")]
    public bool NonOccluder { get; set; }

    [JsonProperty("refraction")]
    public bool Refraction { get; set; }

    [JsonProperty("refractionFalloff")]
    public bool RefractionFalloff { get; set; }

    [JsonProperty("refractionPower")]
    public float RefractionPower { get; set; }

    [JsonProperty("environmentMapping")]
    public bool EnvironmentMapping { get; set; }

    [JsonProperty("environmentMappingMaskScale")]
    public float EnvironmentMappingMaskScale { get; set; }

    [JsonProperty("depthBias")]
    public bool DepthBias { get; set; }

    [JsonProperty("grayscaleToPaletteColor")]
    public bool GrayscaleToPaletteColor { get; set; }

    [JsonProperty("writeMaskAlbedo")]
    private bool WriteMaskAlbedo
    {
      get => MaskWrites.HasFlag(MaskWriteFlags.ALBEDO);
      set => MaskWrites |= MaskWriteFlags.ALBEDO;
    }

    [JsonProperty("writeMaskNormal")]
    private bool WriteMaskNormal
    {
      get => MaskWrites.HasFlag(MaskWriteFlags.NORMAL);
      set => MaskWrites |= MaskWriteFlags.NORMAL;
    }

    [JsonProperty("writeMaskSpecular")]
    private bool WriteMaskSpecular
    {
      get => MaskWrites.HasFlag(MaskWriteFlags.SPECULAR);
      set => MaskWrites |= MaskWriteFlags.SPECULAR;
    }

    [JsonProperty("writeMaskAmbientOcclusion")]
    private bool WriteMaskAmbientOcclusion
    {
      get => MaskWrites.HasFlag(MaskWriteFlags.AMBIENT_OCCLUSION);
      set => MaskWrites |= MaskWriteFlags.AMBIENT_OCCLUSION;
    }

    [JsonProperty("writeMaskEmissive")]
    private bool WriteMaskEmissive
    {
      get => MaskWrites.HasFlag(MaskWriteFlags.EMISSIVE);
      set => MaskWrites |= MaskWriteFlags.EMISSIVE;
    }

    [JsonProperty("writeMaskGloss")]
    private bool WriteMaskGloss
    {
      get => MaskWrites.HasFlag(MaskWriteFlags.GLOSS);
      set => MaskWrites |= MaskWriteFlags.GLOSS;
    }

    public MaskWriteFlags MaskWrites { get; set; }
    #endregion

    public virtual void Deserialize(BinaryReader input)
    {
      var magic = input.ReadUInt32();
      if (magic != _Signature)
      {
        throw new FormatException();
      }

      Version = input.ReadUInt32();

      var tileFlags = input.ReadUInt32();
      TileU = (tileFlags & 2) != 0;
      TileV = (tileFlags & 1) != 0;
      UVOffset = [input.ReadSingle(), input.ReadSingle()];
      UVScale = [input.ReadSingle(), input.ReadSingle()];

      Alpha = input.ReadSingle();
      var alphaBlendMode0 = input.ReadByte();
      var alphaBlendMode1 = input.ReadUInt32();
      var alphaBlendMode2 = input.ReadUInt32();
      AlphaBlendMode = ConvertAlphaBlendMode(alphaBlendMode0, alphaBlendMode1, alphaBlendMode2);
      AlphaTestRef = input.ReadByte();
      AlphaTest = input.ReadBoolean();

      ZBufferWrite = input.ReadBoolean();
      ZBufferTest = input.ReadBoolean();
      ScreenSpaceReflections = input.ReadBoolean();
      WetnessControlScreenSpaceReflections = input.ReadBoolean();
      Decal = input.ReadBoolean();
      TwoSided = input.ReadBoolean();
      DecalNoFade = input.ReadBoolean();
      NonOccluder = input.ReadBoolean();

      Refraction = input.ReadBoolean();
      RefractionFalloff = input.ReadBoolean();
      RefractionPower = input.ReadSingle();

      if (Version < 10)
      {
        EnvironmentMapping = input.ReadBoolean();
        EnvironmentMappingMaskScale = input.ReadSingle();
      }
      else
      {
        DepthBias = input.ReadBoolean();
      }

      GrayscaleToPaletteColor = input.ReadBoolean();

      if (Version >= 6)
      {
        MaskWrites = (MaskWriteFlags)input.ReadByte();
      }
    }

    public virtual void Serialize(BinaryWriter output)
    {
      output.Write(_Signature);
      output.Write(Version);

      uint tileFlags = 0;
      if (TileU) tileFlags += 2;
      if (TileV) tileFlags += 1;
      output.Write(tileFlags);

      output.Write(UVOffset[0]);
      output.Write(UVOffset[1]);
      output.Write(UVScale[0]);
      output.Write(UVScale[1]);

      output.Write(Alpha);

      byte alphaBlendMode0 = 0;
      uint alphaBlendMode1 = 0;
      uint alphaBlendMode2 = 0;
      ConvertAlphaBlendMode(AlphaBlendMode, ref alphaBlendMode0, ref alphaBlendMode1, ref alphaBlendMode2);
      output.Write(alphaBlendMode0);
      output.Write(alphaBlendMode1);
      output.Write(alphaBlendMode2);

      output.Write(AlphaTestRef);
      output.Write(AlphaTest);

      output.Write(ZBufferWrite);
      output.Write(ZBufferTest);
      output.Write(ScreenSpaceReflections);
      output.Write(WetnessControlScreenSpaceReflections);
      output.Write(Decal);
      output.Write(TwoSided);
      output.Write(DecalNoFade);
      output.Write(NonOccluder);

      output.Write(Refraction);
      output.Write(RefractionFalloff);
      output.Write(RefractionPower);

      if (Version < 10)
      {
        output.Write(EnvironmentMapping);
        output.Write(EnvironmentMappingMaskScale);
      }
      else
      {
        output.Write(DepthBias);
      }

      output.Write(GrayscaleToPaletteColor);

      if (Version >= 6)
      {
        output.Write((byte)MaskWrites);
      }
    }

    public bool Open(FileStream file)
    {
      try
      {
        using BinaryReader reader = new(file);
        Deserialize(reader);
      }
      catch
      {
        return false;
      }

      return true;
    }

    public bool Save(FileStream file)
    {
      try
      {
        using BinaryWriter writer = new(file);
        Serialize(writer);
        writer.Flush();
      }
      catch
      {
        return false;
      }

      return true;
    }

    protected static string ReadString(BinaryReader input)
    {
      var length = input.ReadUInt32();
      string str = new(input.ReadChars((int)length));

      int index = str.LastIndexOf('\0');
      if (index >= 0)
        str = str.Remove(index, 1);

      return str;
    }

    protected static void WriteString(BinaryWriter output, string? str)
    {
      if (str != null)
      {
        int length = str.Length + 1;
        output.Write(length);
        output.Write((str + "\0").ToCharArray());
      }
      else
      {
        int length = 1;
        output.Write(length);
        output.Write("\0".ToCharArray());
      }
    }

    public enum AlphaBlendModeType
    {
      Unknown = 0,
      None,
      Standard,
      Additive,
      Multiplicative,
    }

    private static AlphaBlendModeType ConvertAlphaBlendMode(byte a, uint b, uint c)
    {
      return a switch
      {
        0 when b == 6 && c == 7 => AlphaBlendModeType.Unknown,
        0 when b == 0 && c == 0 => AlphaBlendModeType.None,
        1 when b == 6 && c == 7 => AlphaBlendModeType.Standard,
        1 when b == 6 && c == 0 => AlphaBlendModeType.Additive,
        1 when b == 4 && c == 1 => AlphaBlendModeType.Multiplicative,
        _ => throw new NotSupportedException()
      };
    }

    private static void ConvertAlphaBlendMode(AlphaBlendModeType type, ref byte a, ref uint b, ref uint c)
    {
      switch (type)
      {
        case AlphaBlendModeType.Unknown:
          a = 0;
          b = 6;
          c = 7;
          break;
        case AlphaBlendModeType.None:
          a = 0;
          b = 0;
          c = 0;
          break;
        case AlphaBlendModeType.Standard:
          a = 1;
          b = 6;
          c = 7;
          break;
        case AlphaBlendModeType.Additive:
          a = 1;
          b = 6;
          c = 0;
          break;
        case AlphaBlendModeType.Multiplicative:
          a = 1;
          b = 4;
          c = 1;
          break;
        default:
          throw new NotSupportedException();
      }
    }

    protected record struct Color(float R, float G, float B)
    {
      public uint ToUInt32()
      {
        uint value = 0;
        value |= (byte)(R * 255);
        value <<= 8;
        value |= (byte)(G * 255);
        value <<= 8;
        value |= (byte)(B * 255);
        return value;
      }

      public static Color FromUInt32(uint value)
      {
        const float multiplier = 1.0f / 255;
        var b = (byte)(value & 0xFF);
        value >>= 8;
        var g = (byte)(value & 0xFF);
        value >>= 8;
        var r = (byte)(value & 0xFF);
        return new Color(r * multiplier, g * multiplier, b * multiplier);
      }

      public string ToHexString()
      {
        return $"#{ToUInt32() & 0xFFFFFFu:x6}";
      }

      public static Color FromHexString(string str)
      {
        var text = str.ToLowerInvariant();
        if (text.StartsWith('#'))
          text = text[1..];

        switch (text)
        {
          case "000":
            return FromUInt32(0x000000u);
          case "fff":
            return FromUInt32(0xFFFFFFu);
        }

        switch (text.Length)
        {
          case 3:
          {
            var val = uint.Parse(text, NumberStyles.AllowHexSpecifier);
            val = ((val & 0xF00) << 8) | ((val & 0x0F0) << 4) | ((val & 0x00F) << 0);
            val |= val << 4;
            return FromUInt32(val);
          }
          case 6:
            return FromUInt32(uint.Parse(text, NumberStyles.AllowHexSpecifier));
          default:
            return new(1.0f, 1.0f, 1.0f);
        }
      }

      public static Color Read(BinaryReader input)
      {
        var r = input.ReadSingle();
        var g = input.ReadSingle();
        var b = input.ReadSingle();
        return new(r, g, b);
      }

      public void Write(BinaryWriter output)
      {
        output.Write(R);
        output.Write(G);
        output.Write(B);
      }
    }

    [Flags]
    public enum MaskWriteFlags
    {
      ALBEDO = 1,
      NORMAL = 2,
      SPECULAR = 4,
      AMBIENT_OCCLUSION = 8,
      EMISSIVE = 16,
      GLOSS = 32
    }

    public enum ShaderType
    {
      Lighting,
      Effect,
    }
  }
}