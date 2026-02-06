using Newtonsoft.Json;

namespace MSFAutoPatcher.Core.Models;
public class MaterialJson
{
  private string? _diffuseMap;

  [JsonProperty("diffuseMap")]
  public string? DiffuseMap
  {
    get => _diffuseMap;
    set =>
      _diffuseMap = value?.StartsWith("textures", StringComparison.CurrentCultureIgnoreCase) is true
        ? value[("textures".Length + 1)..]
        : value;
  }

  private string? _normalMap;

  [JsonProperty("normalMap")]
  public string? NormalMap
  {
    get => _normalMap;
    set =>
      _normalMap = value?.StartsWith("textures", StringComparison.CurrentCultureIgnoreCase) is true
        ? value[("textures".Length + 1)..]
        : value;
  }

  private string? _specularMap;

  [JsonProperty("specularMap")]
  public string? SpecularMap
  {
    get => _specularMap;
    set =>
      _specularMap = value?.StartsWith("textures", StringComparison.CurrentCultureIgnoreCase) is true
        ? value[("textures".Length + 1)..]
        : value;
  }

  private string? _environmentMap;

  [JsonProperty("environmentMap")]
  public string? EnvironmentMap
  {
    get => _environmentMap;
    set => _environmentMap = value?.StartsWith("textures", StringComparison.CurrentCultureIgnoreCase) is true
      ? value[("textures".Length + 1)..]
      : value;
  }

  private string? _environmentMaskMap;

  [JsonProperty("environmentMaskMap")]
  public string? EnvironmentMaskMap
  {
    get => _environmentMaskMap;
    set => _environmentMaskMap = value?.StartsWith("textures", StringComparison.CurrentCultureIgnoreCase) is true
      ? value[("textures".Length + 1)..]
      : value;
  }

  private string? _glowMap;

  [JsonProperty("glowMap")]
  public string? GlowMap
  {
    get => _glowMap;
    set => _glowMap = value?.StartsWith("textures", StringComparison.CurrentCultureIgnoreCase) is true
      ? value[("textures".Length + 1)..]
      : value;
  }
}