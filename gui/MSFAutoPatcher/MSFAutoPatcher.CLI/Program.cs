using DynamicData;
using MSFAutoPatcher.Core.Collections;
using Mutagen.Bethesda.Plugins;
using Mutagen.Bethesda.Plugins.Binary.Parameters;
using Mutagen.Bethesda.Plugins.Cache;
using Mutagen.Bethesda.Plugins.Cache.Internals.Implementations;
using Mutagen.Bethesda.Skyrim;
using Noggog;
using System.CommandLine;

var mo2PathOption = new Option<DirectoryInfo?>("--mo2-path")
{
  Description = "MO2 installation path. Use this if you're running for an MO2 installation.",
  DefaultValueFactory = (_) => null,
};
var gamePathOption = new Option<DirectoryInfo?>("--game-path")
{
  Description = "Skyrim installation path. Use this if you're running from Vortex or standalone.",
  DefaultValueFactory = (_) => null,
};
var outDirOption = new Option<DirectoryInfo?>("--out")
{
  Description = "Output directory path. Only taken into account with --mo2-path.",
  DefaultValueFactory = (_) => null,
};
var rootCommand = new RootCommand("Automatically convert armor variants into material swaps");
rootCommand.Options.Add(mo2PathOption);
rootCommand.Options.Add(gamePathOption);
rootCommand.Options.Add(outDirOption);
var cliResult = rootCommand.Parse(args);
if (cliResult.Errors.Count > 0)
{
  Console.WriteLine(string.Join(Environment.NewLine, cliResult.Errors.Select(x => x.Message)));
  Console.ReadKey();
  return 1;
}

try
{
  var mo2Path = cliResult.GetValue(mo2PathOption);
  var gamePath = cliResult.GetValue(gamePathOption);
  var outDir = cliResult.GetValue(outDirOption);


  if (gamePath is not null || (gamePath is null && mo2Path is null && outDir is null))
  {
    var actualOutDir = gamePath is null ? Environment.CurrentDirectory : Path.Join(gamePath.FullName, "Data");
    var loadOrderTxt = File.ReadAllLines(
     Path.Join(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "Skyrim Special Edition", "loadorder.txt"));
    var modFiles = Directory.EnumerateFiles(actualOutDir, "*.es*")
      .Where(x => loadOrderTxt.Contains(Path.GetFileName(x)))
      .OrderBy(x => loadOrderTxt.IndexOf(Path.GetFileName(x)))
      .Select(x => SkyrimMod.CreateFromBinary(ModPath.FromPath(x), SkyrimRelease.SkyrimSE));
    using var linkCache = new ImmutableLoadOrderLinkCache<ISkyrimMod, ISkyrimModGetter>(modFiles, LinkCachePreferences.Default);
    foreach (var mod in linkCache.ListedOrder.Cast<ISkyrimModGetter>())
    {
      var armorNifCollector = ArmorNifCollector.Hydrate(linkCache, mod.Armors);
      var materialCollector = MaterialConfigCollector.ProcessFromNifCollector(linkCache, armorNifCollector);
      materialCollector.WriteConfigurationsToDisk(actualOutDir);
      materialCollector.WriteMaterialsToDisk(actualOutDir);
    }
  }
  else if (mo2Path is not null)
  {
    var overwriteDir = Path.Join(mo2Path.FullName, "overwrite");
    var modsDir = Path.Join(mo2Path.FullName, "mods");
    foreach (var modDir in Directory.EnumerateDirectories(modsDir))
    {
      var modFiles = Directory.EnumerateFiles(modDir, "*.es*")
        .Select(x =>
        {
          try
          {
            return SkyrimMod.CreateFromBinary(ModPath.FromPath(x), SkyrimRelease.SkyrimSE);
          }
          catch
          {
            return null;
          }
        })
        .Where(x => x is not null)
        .Cast<ISkyrimModGetter>();
      using var linkCache = new ImmutableLoadOrderLinkCache<ISkyrimMod, ISkyrimModGetter>(modFiles, LinkCachePreferences.Default);
      foreach (var mod in linkCache.ListedOrder.Cast<ISkyrimModGetter>())
      {
        var armorNifCollector = ArmorNifCollector.Hydrate(linkCache, mod.Armors);
        var materialCollector = MaterialConfigCollector.ProcessFromNifCollector(linkCache, armorNifCollector);
        materialCollector.WriteConfigurationsToDisk(outDir?.FullName ?? overwriteDir);
        materialCollector.WriteMaterialsToDisk(outDir?.FullName ?? overwriteDir);
      }
    }
  }

  Console.WriteLine("Press any key to continue...");
  Console.ReadKey();

  return 0;
}
catch (Exception ex)
{
  Console.WriteLine(ex);
  Console.WriteLine("Press any key to continue...");
  Console.ReadKey();
  return 1;
}