using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Platform.Storage;
using MaterialSwapperFramework.Gui.Models;
using MaterialSwapperFramework.Gui.Services;
using MaterialSwapperFramework.Gui.ViewModels;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Newtonsoft.Json;

namespace MaterialSwapperFramework.Gui.Views;

public partial class MainWindow : Window
{
  public MainWindowViewModel Model => (MainWindowViewModel)DataContext!;
  private bool _didShowPluginWindow = false;
  public static readonly FilePickerFileType PluginFilePickerType = new("Plugin files")
  {
    Patterns = ["*.esm", "*.esp", "*.esl"],
    AppleUniformTypeIdentifiers = ["public.plugin"],
    MimeTypes = []
  };

  private static readonly IEnumerable<string> MaterialFileExtensions = [".json", ".bgem", ".bgsm"];

  public MainWindow()
  {
    DataContext = new MainViewModel();
    InitializeComponent();
  }

  public async void OnOpenPluginClicked(object sender, RoutedEventArgs args)
  {
    var files = await StorageProvider.OpenFilePickerAsync(new()
    {
      Title = "Select ESM/ESM/ESL",
      AllowMultiple = false,
      FileTypeFilter = [PluginFilePickerType]
    });
    if (files.Any())
    {
      var path = files[0].Path.LocalPath;
      Model.DataDir = Path.GetDirectoryName(path);
      Model.AllModFiles = [path];
      Model.IsReady = true;
    }
  }

  public async void OnOpenFolderClicked(object sender, RoutedEventArgs args)
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
      Model.DataDir = baseDirectory;
      Model.AllModFiles = [..files];
      Model.IsReady = true;
    }
  }


  private async void OnActivated(object? sender, EventArgs e)
  {
    if (_didShowPluginWindow)
    {
      return;
    }
    _didShowPluginWindow = true;
    var availablePlugins = Directory.EnumerateFiles(".", "*.es*", SearchOption.TopDirectoryOnly);
    Window pluginSelectionWindow;
    if (availablePlugins.Any())
    {
      Model.AllModFiles = availablePlugins;
      pluginSelectionWindow = new PluginsTxtWindow();
    }
    else
    {
      pluginSelectionWindow = new PluginsPickerWindow();
    }

    var selectedPlugins = await pluginSelectionWindow.ShowDialog<IEnumerable<string>>(this);
    MainViewInstance.EspFiles = selectedPlugins;
    Model.IsReady = true;
  }

  private async void OnOpenConsolidationWizard(object? sender, RoutedEventArgs e)
  {
    var wizard = new ConsolidationWizard() { SelectedModFiles = MainViewInstance.EspFiles };
    await wizard.ShowDialog(this);
  }

  private async void OnConvertMaterialFileClicked(object? sender, RoutedEventArgs e)
  {
    var folder = await StorageProvider.OpenFolderPickerAsync(new()
    {
      Title = "Select the folder containing material files",
    });
    IsEnabled = false;
    if (folder.Any())
    {
      var dataDir = folder[0].Path.LocalPath;
      var files = Directory.EnumerateFiles(dataDir, "*.*", SearchOption.AllDirectories)
        .Where(f => MaterialFileExtensions.Contains(Path.GetExtension(f).ToLowerInvariant()));
      foreach (var file in files)
      {
        MaterialFileBase material;
        if (Path.GetExtension(file).Equals(".bgsm", StringComparison.OrdinalIgnoreCase))
        {
          material = new BGSMFile();
        }
        else if (Path.GetExtension(file).Equals(".bgem", StringComparison.OrdinalIgnoreCase))
        {
          material = new BGEMFile();
        }
        else
        {
          continue;
        }
        var filenameAsJson = Path.ChangeExtension(file, ".json");
        if (File.Exists(filenameAsJson) || !File.Exists(file))
        {
          continue;
        }
        await using var fs = File.OpenRead(file);
        material.Deserialize(new(fs));
        await using var jsonStream = File.Create(filenameAsJson);
        var json = JsonConvert.SerializeObject(material, Formatting.Indented);
        await using var writer = new StreamWriter(jsonStream);
        await writer.WriteAsync(json);
      }
    }
    IsEnabled = true;
  }
}
