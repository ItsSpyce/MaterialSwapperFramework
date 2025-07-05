using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;
using System.Text.Json.Nodes;
using System.Threading.Tasks;
using MaterialSwapperFramework.Gui.Extensions;
using MaterialSwapperFramework.Gui.Models;
using Newtonsoft.Json.Linq;
using Noggog;

namespace MaterialSwapperFramework.Gui.Services;

public interface IMaterialService
{
  Task<IEnumerable<MsfConfigJson>> LoadConfigJsonAsync(string filename);
  Task<IEnumerable<MsfConfigJson>> LoadConfigJsonsAsync(string dataDir);
  Task SaveConfigJsonAsync(string filename, MsfConfigJson config);
  Task<MaterialFileBase> LoadMaterialFileAsync(string filename);
  Task<IEnumerable<MaterialFileBase>> LoadMaterialFilesAsync(string dataDir);
  Task SaveMaterialFileAsync(string filename, MaterialFileBase materialFile);
}

public class MaterialService : IMaterialService
{
  public async Task<IEnumerable<MsfConfigJson>> LoadConfigJsonAsync(string filename)
  {
    var result = new HashSet<MsfConfigJson>();
    await using var fs = File.OpenRead(filename);
    var configJson = (await JsonNode.ParseAsync(fs))?.AsObject();
    if (configJson is null) return result;
    foreach (var (pluginAndFormID, config) in configJson)
    {
      if (config?.GetValueKind() != JsonValueKind.Array) continue;
      var (pluginName, formID) = pluginAndFormID.Split('|').ToTuple();
      var records = config.Deserialize<IEnumerable<MaterialRecord>>();
      result.Add(new(pluginName, formID, records));
    }

    return result;
  }

  public async Task<IEnumerable<MsfConfigJson>> LoadConfigJsonsAsync(string dataDir)
  {
    var result = new HashSet<MsfConfigJson>();
    var configsDir = Path.Join(dataDir, Constants.ConfigsDir);
    if (string.IsNullOrEmpty(configsDir) || !Directory.Exists(configsDir))
    {
      return result;
    }
    var files = Directory.EnumerateFiles(Path.Join(dataDir, Constants.ConfigsDir),
      "*.json", SearchOption.AllDirectories);
    foreach (var file in files)
    {
      var configs = await LoadConfigJsonAsync(file);
      result.Add(configs);
    }

    return result;
  }

  public Task SaveConfigJsonAsync(string filename, MsfConfigJson config)
  {
    throw new System.NotImplementedException();
  }

  public Task<MaterialFileBase> LoadMaterialFileAsync(string filename)
  {
    throw new System.NotImplementedException();
  }

  public Task<IEnumerable<MaterialFileBase>> LoadMaterialFilesAsync(string dataDir)
  {
    throw new System.NotImplementedException();
  }

  public Task SaveMaterialFileAsync(string filename, MaterialFileBase materialFile)
  {
    throw new System.NotImplementedException();
  }
}