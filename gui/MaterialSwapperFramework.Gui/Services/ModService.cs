using System.Collections.Generic;
using System.IO;
using System.Linq;
using Mutagen.Bethesda.Plugins;
using Mutagen.Bethesda.Skyrim;

namespace MaterialSwapperFramework.Gui.Services;

public interface IModService
{
  ISkyrimModDisposableGetter LoadMod(string filename);
  IEnumerable<ISkyrimModDisposableGetter> LoadMods(IEnumerable<string> filenames);
}

public class ModService : IModService
{
  public ISkyrimModDisposableGetter LoadMod(string filename)
  {
    return SkyrimMod.Create(SkyrimRelease.SkyrimSE)
      .FromPath(ModPath.FromPath(new(filename)))
      .WithDataFolder(Path.GetDirectoryName(filename))
      .Parallel()
      .Construct();
  }

  public IEnumerable<ISkyrimModDisposableGetter> LoadMods(IEnumerable<string> filenames)
  {
    return filenames.Select(LoadMod);
  }
}