using System;
using System.Collections.Generic;
using System.Linq;

namespace MaterialSwapperFramework.Gui.Extensions;

public static class EnumerableExtensions
{
  public static Tuple<T, T> ToTuple<T>(this IEnumerable<T> self) =>
    new(self.ElementAtOrDefault(0), self.ElementAtOrDefault(1));
}