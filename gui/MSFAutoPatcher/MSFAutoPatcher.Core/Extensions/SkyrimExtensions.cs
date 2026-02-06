using Mutagen.Bethesda.Plugins.Records;

namespace MSFAutoPatcher.Core.Extensions;
public static class SkyrimExtensions
{
  public static T GetGendered<T>(this IGenderedItemGetter<T> self, MaleFemaleGender gender) => gender switch
  {
    MaleFemaleGender.Male => self.Male,
    MaleFemaleGender.Female => self.Female,
    _ => throw new NotImplementedException()
  };
}
