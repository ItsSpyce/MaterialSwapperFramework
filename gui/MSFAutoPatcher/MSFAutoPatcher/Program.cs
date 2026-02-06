using MSFAutoPatcher.Core.Collections;
using Mutagen.Bethesda;
using Mutagen.Bethesda.Plugins.Assets;
using Mutagen.Bethesda.Skyrim;
using Mutagen.Bethesda.Synthesis;

namespace MSFAutoPatcher.Synthesis;

public class Program
{
  public static async Task<int> Main(string[] args)
  {
    try
    {
      return await SynthesisPipeline.Instance
        .AddPatch<ISkyrimMod, ISkyrimModGetter>(RunPatch)
        .SetTypicalOpen(GameRelease.SkyrimSE, "MaterialSwapperFramework-Patches.esp")
        .Run(args);
    }
    catch (Exception e)
    {
      Console.WriteLine(e);
    }

#if DEBUG
    Console.ReadLine();
#endif
    return -1;
  }

  public static void RunPatch(IPatcherState<ISkyrimMod, ISkyrimModGetter> state)
  {
    var armorNifCollector = ArmorNifCollector.Hydrate(
      state.LinkCache,
      state.LoadOrder.PriorityOrder.Armor().WinningOverrides());
    var materialCollector = MaterialConfigCollector.ProcessFromNifCollector(state.LinkCache, armorNifCollector);
    materialCollector.WriteConfigurationsToDisk(state.DataFolderPath.Path);
    materialCollector.WriteMaterialsToDisk(state.DataFolderPath.Path);
  }
}
