using MSFAutoPatcher.Core.Extensions;
using MSFAutoPatcher.Core.Helpers;
using MSFAutoPatcher.Core.Models;
using Mutagen.Bethesda.Plugins;
using Mutagen.Bethesda.Plugins.Cache;
using Mutagen.Bethesda.Plugins.Records;
using Mutagen.Bethesda.Skyrim;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MSFAutoPatcher.Core.Collections;
public class MaterialConfigCollector(ArmorNifCollector armorNifCollector)
{
  public ArmorNifCollector ArmorNifCollector => armorNifCollector;
  /// <summary>
  ///   MaterialName;MaterialData
  /// </summary>
  private readonly Dictionary<string, MaterialJson> _materials = new();
  /// <summary>
  ///   ModName;FormEditorID;Materials
  /// </summary>
  private readonly Dictionary<string, Dictionary<string, List<MaterialRecord>>> _materialConfig = new();

  public static MaterialConfigCollector ProcessFromNifCollector(ILinkCache cache, ArmorNifCollector armorNifCollector)
  {
    var result = new MaterialConfigCollector(armorNifCollector);
    result.ProcessMaterialsForGender(cache, MaleFemaleGender.Male);
    result.ProcessMaterialsForGender(cache, MaleFemaleGender.Female);
    return result;
  }

  public void WriteConfigurationsToDisk(string directory)
  {
    foreach (var (mod, armors) in _materialConfig)
    {
      var materialOutPath = Path.Combine(
        directory,
        "SKSE",
        "Plugins",
        "MaterialSwapperFramework",
        mod);
      Directory.CreateDirectory(materialOutPath);
      File.WriteAllText(Path.Combine(materialOutPath, "config.json"),
        JsonConvert.SerializeObject(armors, Formatting.Indented,
          new JsonSerializerSettings { NullValueHandling = NullValueHandling.Ignore }));
    }
  }


  public void WriteMaterialsToDisk(string directory)
  {
    var materialOutputDirectory = Path.Combine(directory, "materials", "auto-generated");
    Directory.CreateDirectory(materialOutputDirectory);
    foreach (var (name, material) in _materials)
    {
      // write to auto-generated directory
      var materialFilename = Path.Join(materialOutputDirectory, $"{name}.json");
      File.WriteAllText(materialFilename,
        JsonConvert.SerializeObject(material, Formatting.Indented,
          new JsonSerializerSettings { NullValueHandling = NullValueHandling.Ignore }));
    }
  }

  private void ProcessMaterialsForGender(ILinkCache cache, MaleFemaleGender gender)
  {
    ArmorNifCollector.ForEach((dataList) =>
    {
      var records = dataList.GetEnumerable(gender).ToList();
      if (records.Count <= 1) return;
      var record = records
        .FirstOrDefault(x => x.Armature.WorldModel!.GetGendered(gender)!.AlternateTextures is null);
      if (record is null)
      {
        return;
      }
      var clones = records
        .Where(x => x.Armor.FormKey != record.Armor.FormKey).ToList();
      if (!clones.Any())
      {
        return;
      }
      var modKey = record.Armor.FormKey.ModKey.FileName.String;
      var formKeyString = StringHelpers.SwapFormKeyString(record.Armor.FormKey);
      if (!_materialConfig.ContainsKey(modKey))
      {
        _materialConfig[modKey] = new();
      }
      if (!_materialConfig[modKey].ContainsKey(formKeyString))
      {
        _materialConfig[modKey][formKeyString] = [];
      }

      foreach (var (_, armorAddon) in clones)
      {
        var textureSwaps = armorAddon!.WorldModel!.GetGendered(gender)!.AlternateTextures;
        if (textureSwaps is null or [])
        {
          continue;
        }

        var materialRecords = new List<MaterialRecord>();
        var shapeMaterials = new Dictionary<string, string>();
        foreach (var textureSwap in textureSwaps)
        {
          var shape = textureSwap.Name;
          var textureSet = textureSwap.NewTexture.TryResolve(cache);
          if (textureSet is null) continue;
          if (!_materials.ContainsKey(textureSet.EditorID!))
          {
            var materialJson = new MaterialJson();
            if (textureSet.Diffuse?.IsNull is false)
            {
              materialJson.DiffuseMap = textureSet.Diffuse.GivenPath;
            }

            if (textureSet.NormalOrGloss?.IsNull is false)
            {
              materialJson.NormalMap = textureSet.NormalOrGloss.GivenPath;
            }

            if (textureSet.BacklightMaskOrSpecular?.IsNull is false)
            {
              materialJson.SpecularMap = textureSet.BacklightMaskOrSpecular.GivenPath;
            }

            if (textureSet.Environment?.IsNull is false)
            {
              materialJson.EnvironmentMap = textureSet.Environment.GivenPath;
            }

            if (textureSet.EnvironmentMaskOrSubsurfaceTint?.IsNull is false)
            {
              materialJson.EnvironmentMaskMap = textureSet.EnvironmentMaskOrSubsurfaceTint.GivenPath;
            }

            if (textureSet.GlowOrDetailMap?.IsNull is false)
            {
              materialJson.GlowMap = textureSet.GlowOrDetailMap.GivenPath;
            }

            _materials[textureSet.EditorID!] = materialJson;
          }

          var materialFilename = $"{textureSet.EditorID!}.json";
          shapeMaterials[shape] = Path.Join("auto-generated", materialFilename);
        }

        var materialRecord = new MaterialRecord
        {
          Name = StringHelpers.TryToHumanizeEditorID(armorAddon.EditorID!),
          Applies = shapeMaterials,
          IsHidden = false,
          ModifyName = true,
          Gender = gender == MaleFemaleGender.Female
            ? MaterialRecord.MaterialRecordGender.Female
            : MaterialRecord.MaterialRecordGender.Male,
        };
        Console.WriteLine($"""
                         Created new material record for armor {formKeyString}
                         - Material: {materialRecord.Name}
                         - Shapes: {string.Join(", ", shapeMaterials.Keys)}
                         """);
        materialRecords.Add(materialRecord);
        _materialConfig[modKey][formKeyString].AddRange(materialRecords);
      }
    });
  }
}
