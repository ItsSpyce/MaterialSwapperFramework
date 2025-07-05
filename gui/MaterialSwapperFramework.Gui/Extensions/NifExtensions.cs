using System.Collections;
using Microsoft.VisualBasic;
using NiflySharp;

namespace MaterialSwapperFramework.Gui.Extensions;

public static class NifExtensions
{
  public static ICollection GetShapeNames(this NifFile nif)
  {
    var shapes = nif.GetShapes();
    var result = new Collection();
    foreach (var shape in shapes)
    {
      result.Add(shape.Name.String, shape.Name.String);
    }
    return result;
  }
}