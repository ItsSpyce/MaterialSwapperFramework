using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Platform.Storage;
using MaterialSwapperFramework.Gui.Models;
using MaterialSwapperFramework.Gui.ViewModels;
using Mutagen.Bethesda.Skyrim;

namespace MaterialSwapperFramework.Gui.Views;

public partial class MainView : UserControl
{
  public MainViewModel Model => DataContext as MainViewModel;

  private static readonly FilePickerFileType PluginFilePickerType = new("Plugin files")
  {
    Patterns = ["*.esm", "*.esp", "*.esl"],
    AppleUniformTypeIdentifiers = ["public.plugin"],
    MimeTypes = []
  };

  private static readonly IEnumerable<string> MaterialFileExtensions = [".json", ".bgem", ".bgsm"];

  private IEnumerable<string> _materialFiles = [];
  private List<MsfConfigJson> _configFiles = [];

  public MainView()
  {
    InitializeComponent();
  }

  public async void OnOpenPluginClicked(object sender, RoutedEventArgs args)
  {
    var topLevel = TopLevel.GetTopLevel(this)!;
    var files = await topLevel.StorageProvider.OpenFilePickerAsync(new()
    {
      Title = "Select ESM/ESM/ESL",
      AllowMultiple = false,
      FileTypeFilter = [PluginFilePickerType]
    });
    if (files.Any())
    {
      var path = files[0].Path.LocalPath;
      Model.LoadedModFiles = [path];
    }
  }

  public async void OnOpenFolderClicked(object sender, RoutedEventArgs args)
  {
    var topLevel = TopLevel.GetTopLevel(this)!;
    var folders = await topLevel.StorageProvider.OpenFolderPickerAsync(new()
    {
      Title = "Select the Data directory",
      AllowMultiple = false,
    });
    if (folders.Any())
    {
      var baseDirectory = folders[0].Path.LocalPath;
      var files = Directory.EnumerateFiles(baseDirectory, "*.es*").OrderByDescending(File.GetCreationTime);
      Model.LoadedModFiles = [..files];
    }
  }
}
