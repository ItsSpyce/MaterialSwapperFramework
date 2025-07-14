using System.Collections.Generic;
using Avalonia.Data.Converters;
using MaterialSwapperFramework.Gui.Converters;
using MaterialSwapperFramework.Gui.Models;
using Mutagen.Bethesda.Skyrim;
using Noggog;

namespace MaterialSwapperFramework.Gui;

public static class StaticConverters
{
  public static FuncValueConverter<bool, bool> InverseBool { get; } =
    new(x => !x);

  public static FuncValueConverter<object?, bool> IsNotNull { get; } =
    new(x => x is not null);

  public static FuncValueConverter<object?, bool> IsNull { get; } =
    new(x => x is null);

  public static FuncValueConverter<ISkyrimMajorRecordGetter, string> RecordGetterDisplay { get; } =
    new(x => $"{x.FormKey.IDString()} | {x.EditorID ?? string.Empty}");

  public static FuncValueConverter<IEnumerable<MaterialRecord>, bool> DoesHaveRecords { get; } =
    new(x => x is not null && x.Any());

  public static FuncValueConverter<IEnumerable<MaterialRecord>, bool> DoesNotHaveRecords { get; } =
    new(x => x is null || !x.Any());

  public static FuncValueConverter<MsfProj?, bool> IsProjectSelected { get; } =
    new(x => x is not null && !string.IsNullOrEmpty(x.Filename));

  public static FuncValueConverter<MsfProj?, string> GetHeaderText { get; } =
    new(x => x is null ? "No project loaded" : $"{(x.Filename ?? $"New project") + (x.IsDirty ? " *" : "")}");

  public static FuncValueConverter<ISkyrimMajorRecordGetter, bool> IsRecordArmor { get; } =
    new(x =>
      x is IArmorGetter);

  public static GenderEnumConverter GenderConverter { get; } = new();
}