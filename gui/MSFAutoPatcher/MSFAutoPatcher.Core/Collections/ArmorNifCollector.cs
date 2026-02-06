using MSFAutoPatcher.Core.Extensions;
using Mutagen.Bethesda.Plugins;
using Mutagen.Bethesda.Plugins.Cache;
using Mutagen.Bethesda.Plugins.Records;
using Mutagen.Bethesda.Skyrim;

namespace MSFAutoPatcher.Core.Collections;
public class ArmorNifCollector : Dictionary<string, GenderedList<ArmorNifCollector.ArmorNifRecord>>
{
  public record ArmorNifRecord(IArmorGetter Armor, IArmorAddonGetter Armature);

  public static ArmorNifCollector Hydrate(ILinkCache cache, IEnumerable<IArmorGetter> armors)
  {
    var result = new ArmorNifCollector();
    foreach (var armor in armors)
    {
      var armatureLink = armor.Armature.FirstOrDefault();
      var armature = armatureLink?.TryResolve(cache);
      if (armature is null) continue;
      result.AddBipedModel(MaleFemaleGender.Male, armor, armature);
      result.AddBipedModel(MaleFemaleGender.Female, armor, armature);
    }
    return result;
  }

  public void ForEach(Action<GenderedList<ArmorNifRecord>> visitor)
  {
    foreach (var (_, dataList) in this)
    {
      visitor(dataList);
    }
  }

  private void AddBipedModel(MaleFemaleGender gender, IArmorGetter armor, IArmorAddonGetter armature)
  {
    var bipedModel = armature.WorldModel?.GetGendered(gender);
    if (bipedModel is null) return;
    if (!this.TryGetValue(bipedModel.File.GivenPath, out var others))
    {
      others = new();
      this[bipedModel.File.GivenPath] = others;
    }
    others.AddItem(gender, new(armor, armature));
  }
}
