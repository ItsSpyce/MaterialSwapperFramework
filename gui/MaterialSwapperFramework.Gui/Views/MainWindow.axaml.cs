using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Platform.Storage;
using MaterialSwapperFramework.Gui.Models;
using MaterialSwapperFramework.Gui.Services;
using MaterialSwapperFramework.Gui.ViewModels;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using Newtonsoft.Json;
using NiflySharp;
using NiflySharp.Blocks;
using NiflySharp.Enums;

namespace MaterialSwapperFramework.Gui.Views;

public partial class MainWindow : Window
{
  public MainWindowViewModel Model => (MainWindowViewModel)DataContext!;
  private bool _didShowPluginWindow;
  private IMaterialService _materialService = new MaterialService();
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
      Model.AllModFiles = [.. files];
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

  private async void OnPatchNifsClicked(object? sender, RoutedEventArgs e)
  {
    var nifFolder = await StorageProvider.OpenFolderPickerAsync(new()
    {
      Title = "Select the folder containing NIF files",
    });
    var materialsFolder = await StorageProvider.OpenFolderPickerAsync(new()
    {
      Title = "Select the materials folder",
    });
    IsEnabled = false;
    var patchedNifFiles = new List<string>();
    if (nifFolder.Any())
    {
      var files = Directory.EnumerateFiles(nifFolder[0].Path.LocalPath, "*.nif", SearchOption.AllDirectories);
      foreach (var file in files)
      {
        var readStream = File.OpenRead(file);
        Debug.WriteLine($"Processing NIF file {file}");
        var nifFile = new NifFile();
        nifFile.Load(readStream, new() { IsTerrainFile = false });
        readStream.Close();
        await readStream.DisposeAsync();
        foreach (var shape in nifFile.GetShapes())
        {
          if (!shape.HasShaderProperty)
          {
            Debug.WriteLine("Shape has no shader property");
            continue;
          }

          if (nifFile.GetShader(shape) is not BSLightingShaderProperty shaderProp || shaderProp.Name.Length == 0)
          {
            Debug.WriteLine("Not a ShaderProp or has no name");
            continue;
          }
          Console.WriteLine($"Converting shader name {shaderProp.Name.String}");
          if (shaderProp.Name.String.ToLower().StartsWith("materials"))
          {
            // trim "Materials/" prefix
            shaderProp.Name.String = shaderProp.Name.String[10..];
          }
          if (shaderProp.Name.String.EndsWith(".bgsm", StringComparison.OrdinalIgnoreCase) || shaderProp.Name.String.EndsWith(".bgem", StringComparison.OrdinalIgnoreCase))
          {
            shaderProp.Name.String = shaderProp.Name.String[..^5] + ".json";
          }
          var materialFile = Path.Join(materialsFolder[0].Path.LocalPath, shaderProp.Name.String);
          if (!File.Exists(materialFile))
          {
            Debug.WriteLine($"Material file {materialFile} does not exist, skipping.");
            continue;
          }
          Debug.WriteLine($"Patching shape {shape.Name.String} with material {materialFile}");
          var material = await _materialService.LoadMaterialAsync(materialFile);
          shaderProp.Name.Clear();
          if (material is BGSMFile bgsm)
          {
            var textureSet = nifFile.GetBlock(shaderProp.TextureSetRef);
            textureSet.Textures[0] = new(bgsm.DiffuseTexture);
            textureSet.Textures[1] = new(bgsm.NormalTexture);
            textureSet.Textures[2] = new(bgsm.GlowTexture);
            textureSet.Textures[3] = new(bgsm.GreyscaleTexture);
            textureSet.Textures[4] = new(bgsm.EnvmapTexture);
            // textureSet.Textures[5] = new(bgsm.EnvmapMaskTexture);
            textureSet.Textures[7] = new(bgsm.SmoothSpecTexture);
          }
          // TODO: BGEM support
        }

        nifFile.RemoveUnreferencedBlocks();
        nifFile.PrettySortBlocks();
        nifFile.FinalizeData();

        var saveCount = 0;
        while (saveCount < 5)
        {
          try
          {
            await using var filestream = File.Open(file, File.Exists(file) ? FileMode.Truncate : FileMode.OpenOrCreate,
              FileAccess.ReadWrite);
            if (nifFile.Save(filestream) == 0)
            {
              break;
            }
            throw new IOException($"Failed to save NIF file {file}");
          }
          catch
          {
            saveCount++;
            Debug.WriteLine($"Failed to save NIF file {file}, retrying ({saveCount}/5)...");
            Thread.Sleep(1000); // Wait a second before retrying
          }
        }
        if (saveCount == 5)
        {
          Debug.WriteLine($"Failed to save NIF file {file} after 5 attempts, skipping.");
          continue;
        }
        patchedNifFiles.Add(file);
      }
    }

    IsEnabled = true;
  }
}
