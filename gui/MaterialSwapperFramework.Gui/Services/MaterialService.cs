using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using MaterialSwapperFramework.Gui.Extensions;
using MaterialSwapperFramework.Gui.Models;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Noggog;

namespace MaterialSwapperFramework.Gui.Services;

public interface IMaterialService
{
  Task<IEnumerable<MsfConfigJson>> LoadConfigJsonAsync(string filename);
  Task<IEnumerable<MsfConfigJson>> LoadConfigJsonsAsync(string dataDir);
  Task SaveConfigJsonAsync(string filename, MsfConfigJson config);
  Task<MaterialFileBase> LoadMaterialAsync(string filename);
  Task<IEnumerable<MaterialFileBase>> LoadMaterialsAsync(string dataDir);
  IEnumerable<string> GetMaterialFiles(string dataDir);
  Task SaveMaterialFileAsync(string filename, MaterialFileBase materialFile);
}

public class MaterialService : IMaterialService
{
  public async Task<IEnumerable<MsfConfigJson>> LoadConfigJsonAsync(string filename)
  {
    var result = new HashSet<MsfConfigJson>();
    await using var fs = File.OpenRead(filename);
    var configJson = await JObject.LoadAsync(new JsonTextReader(new StreamReader(fs)));
    foreach (var (pluginAndFormID, config) in configJson)
    {
      if (config?.Type != JTokenType.Array) continue;
      var (pluginName, formID) = pluginAndFormID.Split('|').ToTuple();
      var records = config.ToObject<MaterialRecord[]>() ?? [];
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

  public IEnumerable<string> GetMaterialFiles(string dataDir)
  {
    return Directory.EnumerateFiles(Path.Join(dataDir, "Materials"), "*.*", SearchOption.AllDirectories)
      .Where(f => f.EndsWith(".bgsm", StringComparison.OrdinalIgnoreCase) ||
                  f.EndsWith(".bgem", StringComparison.OrdinalIgnoreCase) ||
                  f.EndsWith(".json", StringComparison.OrdinalIgnoreCase))
      .Select(x => Path.GetRelativePath(Path.Join(dataDir, "Materials"), x));
  }

  public async Task<MaterialFileBase> LoadMaterialAsync(string filename)
  {
    MaterialFileBase matFile;
    if (Path.GetExtension(filename).Equals(".json", StringComparison.CurrentCultureIgnoreCase))
    {
      var jsonStr = await File.ReadAllTextAsync(filename);
      var json = JObject.Parse(jsonStr);
      if (json.TryGetValue("shaderType", out var token))
      {
        if (token.Value<int>() == 0)
        {
          matFile = json.ToObject<BGSMFile>() ?? new BGSMFile();
        }
        else
        {
          matFile = json.ToObject<BGEMFile>() ?? new BGEMFile();
        }
      }
      else
      {
        throw new InvalidDataException("Invalid material file format: missing 'shaderType' key.");
      }
    }
    else if (Path.GetExtension(filename).Equals("*.bgsm", StringComparison.CurrentCultureIgnoreCase))
    {
      matFile = new BGSMFile();
      await using var fs = File.OpenRead(filename);
      matFile.Deserialize(new(fs));
    }
    else if (Path.GetExtension(filename).Equals("*.bgem", StringComparison.CurrentCultureIgnoreCase))
    {
      matFile = new BGEMFile();
      await using var fs = File.OpenRead(filename);
      matFile.Deserialize(new(fs));
    }
    else
    {
      throw new NotSupportedException($"Unsupported material file type: {Path.GetExtension(filename)}");
    }

    return matFile;
  }

  public async Task<IEnumerable<MaterialFileBase>> LoadMaterialsAsync(string dataDir)
  {
    var files = new List<MaterialFileBase>();
    var materialsDir = Path.Join(dataDir, Constants.MaterialsDir);
    if (string.IsNullOrEmpty(materialsDir) || !Directory.Exists(materialsDir))
    {
      return files;
    }
    foreach (var file in GetMaterialFiles(dataDir))
    {
      try
      {
        var matFile = await LoadMaterialAsync(Path.Join(dataDir, "Materials", file));
        files.Add(matFile);
      }
      catch (Exception ex)
      {
        Console.WriteLine($"Error loading material file {file}: {ex.Message}");
      }
    }

    return files;
  }

  public Task SaveMaterialFileAsync(string filename, MaterialFileBase materialFile)
  {
    throw new System.NotImplementedException();
  }
}