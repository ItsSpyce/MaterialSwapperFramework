using System.Text.Json.Serialization;

namespace MaterialSwapperFramework.Gui.Models;

public class MaterialRecord
{
  [JsonPropertyName("name")]
  public string Name { get; set; } = string.Empty;
  [JsonPropertyName("shapes")]
  public List<string> Shapes { get; set; } = [];
  [JsonPropertyName("filename")]
  public string Filename { get; set; } = string.Empty;
  [JsonPropertyName("isHidden")]
  public bool IsHidden { get; set; }
  [JsonPropertyName("isDefault")]
  public bool ModifyName { get; set; }
}