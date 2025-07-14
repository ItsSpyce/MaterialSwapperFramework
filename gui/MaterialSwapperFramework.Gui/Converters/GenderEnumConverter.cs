using Avalonia.Data.Converters;
using MaterialSwapperFramework.Gui.Models;
using System;
using System.Globalization;
using Avalonia.Controls;

namespace MaterialSwapperFramework.Gui.Converters;

public class GenderEnumConverter : IValueConverter
{
  public object? Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
  {
    if (value is MaterialRecord.MaterialRecordGender x)
    {
      return x switch
      {
        MaterialRecord.MaterialRecordGender.Male => "Male",
        MaterialRecord.MaterialRecordGender.Female => "Female",
        _ => "Both"
      };
    }

    return null;
  }

  public object? ConvertBack(object? value, Type targetType, object? parameter, CultureInfo culture)
  {
    if (value is string x)
    {
      return x.ToLower() switch
      {
        "male" => MaterialRecord.MaterialRecordGender.Male,
        "female" => MaterialRecord.MaterialRecordGender.Female,
        _ => MaterialRecord.MaterialRecordGender.Unisex
      };
    }

    if (value is ContentControl control)
    {
      return control.Content?.ToString()?.ToLower() switch
      {
        "male" => MaterialRecord.MaterialRecordGender.Male,
        "female" => MaterialRecord.MaterialRecordGender.Female,
        _ => MaterialRecord.MaterialRecordGender.Unisex
      };
    }

    return null;
  }
}