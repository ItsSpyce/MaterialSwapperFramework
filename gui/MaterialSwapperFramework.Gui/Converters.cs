using Avalonia.Data.Converters;
using Mutagen.Bethesda.Skyrim;

namespace MaterialSwapperFramework.Gui;

public static class Converters
{ public static FuncValueConverter<ISkyrimMajorRecordGetter, string> RecordGetterDisplay { get; } =
    new(x => $"{x.FormKey.IDString()} | {x.EditorID ?? string.Empty}");

}