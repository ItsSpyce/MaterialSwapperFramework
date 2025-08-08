using System;
using System.Collections.Generic;
using System.Numerics;
using Newtonsoft.Json;

namespace MaterialSwapperFramework.Gui.Converters;

public class VectorConverter : JsonConverter
{
  public override void WriteJson(
    JsonWriter writer,
    object? value,
    JsonSerializer serializer)
  {
    if (value is null)
    {
      writer.WriteNull();
      return;
    }

    var data = ConvertVectorToArray(value);

    // Compose an array.
    writer.WriteStartArray();

    foreach (var t in data)
    {
      writer.WriteValue(t);
    }

    writer.WriteEndArray();
  }

  public override object ReadJson(
    JsonReader reader,
    Type objectType,
    object? existingValue,
    JsonSerializer serializer)
  {
    if (reader.TokenType is not JsonToken.StartArray)
      throw new(
        "Unexpected token parsing binary. " + $"Expected StartArray, got {reader.TokenType}.");
    var data = new List<float>(GetArraySize(objectType));

    while (reader.Read())
    {
      switch (reader.TokenType)
      {
        case JsonToken.Integer:
        case JsonToken.Float:
          data.Add(Convert.ToSingle(reader.Value));
          break;
        case JsonToken.EndArray:
          return data.ToArray();
        case JsonToken.Comment:
          // skip
          break;
        default:
          throw new(
            $"Unexpected token when reading bytes: {reader.TokenType}");
      }
    }

    throw new("Unexpected end when reading bytes.");

  }

  public override bool CanConvert(Type objectType)
  {
    return objectType == typeof(float[]);
  }

  private static float[] ConvertVectorToArray(object? value) => value switch
  {
    null => [],
    Vector2 vector2 => [vector2.X, vector2.Y],
    Vector3 vector3 => [vector3.X, vector3.Y, vector3.Z],
    Vector4 vector4 => [vector4.X, vector4.Y, vector4.Z, vector4.W],
    _ => throw new NotSupportedException($"Vector type {value.GetType().Name} is not supported.")
  };

  private static int GetArraySize(Type vectorType)
  {
    if (vectorType == typeof(Vector2))
    {
      return 2;
    }
    if (vectorType == typeof(Vector3))
    {
      return 3;
    }
    if (vectorType == typeof(Vector4))
    {
      return 4;
    }
    throw new NotSupportedException($"Vector type {vectorType.Name} is not supported.");
  }
}