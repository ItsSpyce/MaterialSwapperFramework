using Newtonsoft.Json;

namespace MSFAutoPatcher.Core.Models;
public class MaterialRecord
{
  public enum MaterialRecordGender
  {
    Unisex,
    Male,
    Female,
  }

  [JsonProperty("name")]
  public string Name { get; set; } = string.Empty;

  [JsonProperty("inherits")]
  public string? Inherits { get; set; }

  [JsonProperty("applies")]
  public Dictionary<string, string> Applies { get; set; } = [];

  [JsonProperty("isHidden")]
  public bool? IsHidden { get; set; }

  [JsonProperty("modifyName")]
  public bool? ModifyName { get; set; } = true;

  [JsonProperty("gender")]
  public MaterialRecordGender? Gender { get; set; }
}
