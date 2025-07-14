using Avalonia.Controls;
using Avalonia.Interactivity;
using Mutagen.Bethesda.Skyrim;
using System;
using System.IO;
using System.Linq;

namespace MaterialSwapperFramework.Gui.Views;

public partial class PluginsPickerWindow : Window
{
  public PluginsPickerWindow()
  {
    InitializeComponent();
  }

  private async void OnOpenPluginClicked(object? sender, RoutedEventArgs e)
  {
    var files = await StorageProvider.OpenFilePickerAsync(new()
    {
      Title = "Select ESM/ESM/ESL",
      AllowMultiple = false,
      FileTypeFilter = [MainWindow.PluginFilePickerType]
    });
    if (files.Any())
    {
      var path = files[0].Path.LocalPath;
      var dataDir = Path.GetDirectoryName(path);
      Close(new[]{path});
    }
  }

  private async void OnOpenFolderClicked(object? sender, RoutedEventArgs e)
  {
    var folders = await StorageProvider.OpenFolderPickerAsync(new()
    {
      Title = "Select the Data directory",
      AllowMultiple = false,
    });
    if (folders.Any())
    {
      var baseDirectory = folders[0].Path.LocalPath;
      var files = Directory.EnumerateFiles(baseDirectory, "*.es*").OrderByDescending(File.GetCreationTime);
      Close(files);
    }
  }

  private void CancelClicked(object? sender, RoutedEventArgs e)
  {
    Close(Array.Empty<string>());
  }

  private void Window_OnClosing(object? sender, WindowClosingEventArgs e)
  {
    if (!e.IsProgrammatic)
    {
      e.Cancel = true;
      Close(Array.Empty<string>());
    }
  }
}