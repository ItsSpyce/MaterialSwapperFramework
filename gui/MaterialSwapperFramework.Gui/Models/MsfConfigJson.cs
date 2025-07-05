using System.Collections.Generic;

namespace MaterialSwapperFramework.Gui.Models;

public record MsfConfigJson(string PluginName, string FormID, IEnumerable<MaterialRecord> Records);