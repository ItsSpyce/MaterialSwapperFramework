using System.Collections.Generic;
using System.Text.Json.Serialization;

namespace MaterialSwapperFramework.Gui.Models;

public class MaterialRecord
{
  public enum MaterialRecordGender
  {
    Unisex,
    Male,
    Female,
  }

  [JsonPropertyName("name")]
  public string Name { get; set; } = string.Empty;

  [JsonPropertyName("inherits")]
  public string? Inherits { get; set; }

  [JsonPropertyName("applies")]
  public Dictionary<string, string> Applies { get; set; } = [];

  [JsonPropertyName("isHidden")]
  public bool? IsHidden { get; set; }

  [JsonPropertyName("modifyName")]
  public bool? ModifyName { get; set; } = true;

  [JsonPropertyName("gender")]
  public MaterialRecordGender? Gender { get; set; }
}