using System.Collections.Generic;
using System.IO;
using System.Numerics;
using System.Text;

namespace MaterialSwapperFramework.Gui.Extensions;

public static class BinaryExtensions
{
  public static string ReadNullTerminatedString(this BinaryReader reader)
  {
    var bytes = new List<byte>();
    byte b;
    while ((b = reader.ReadByte()) != 0)
    {
      bytes.Add(b);
    }
    return Encoding.UTF8.GetString(bytes.ToArray());
  }

  public static Vector2 ReadVec2(this BinaryReader reader)
  {
    return new Vector2(reader.ReadSingle(), reader.ReadSingle());
  }

  public static Vector3 ReadVec3(this BinaryReader reader)
  {
    return new Vector3(reader.ReadSingle(), reader.ReadSingle(), reader.ReadSingle());
  }

  public static Vector4 ReadVec4(this BinaryReader reader)
  {
    return new Vector4(reader.ReadSingle(), reader.ReadSingle(), reader.ReadSingle(), reader.ReadSingle());
  }
}