using Mutagen.Bethesda.Plugins;
using System.Text.RegularExpressions;

namespace MSFAutoPatcher.Core.Helpers;
public static class StringHelpers
{
  public static string TryToHumanizeEditorID(string editorID)
  {
    // some editor IDs start with numbers to be first, skip over those
    int i;
    for (i = 0; i < editorID.Length; i++)
    {
      if (!char.IsDigit(editorID[i]))
      {
        break;
      }
    }
    var result = editorID[i..];
    // now that we have the true starting point, lets split by PascalCase
    result = Regex.Replace(result, "([a-z])([A-Z])", "$1 $2");
    // and also split by underscores
    result = result.Replace("_", " ");
    // capitalize the first letter of each word
    result = Regex.Replace(result, @"\b[a-z]", m => m.Value.ToUpper());
    // add spaces before any remaining numbers
    result = Regex.Replace(result, "([a-zA-Z])([0-9])", "$1 $2");
    // replace trailing AA with nothing (used in some armor addons)
    result = Regex.Replace(result, "AA$", "");
    return result.Trim();
  }

  public static string SwapFormKeyString(FormKey formKey)
  {
    var formKeyString = formKey.ToString();
    if (formKeyString.StartsWith("000"))
    {
      formKeyString = formKeyString.Substring(3); // trim starting 000
    }
    // swaps left and right at ':' if it exists
    var parts = formKeyString.Split(':');
    return parts.Length == 2 ? $"{parts[1]}|{parts[0]}" : formKeyString;
  }
}
