using System.Collections.Generic;
using System.Collections.ObjectModel;
using ReactiveUI;

namespace MaterialSwapperFramework.Gui.Models;

public class MsfProj : ReactiveObject
{
  public const string Extension = ".msfproj";

  private ObservableCollection<string> _modFiles = [];
  public ObservableCollection<string> ModFiles
  {
    get => _modFiles;
    set => this.RaiseAndSetIfChanged(ref _modFiles, value);
  }

  private string? _filename;
  public string? Filename
  {
    get => _filename;
    set => this.RaiseAndSetIfChanged(ref _filename, value);
  }

  private bool _isDirty = false;
  public bool IsDirty
  {
    get => _isDirty;
    set => this.RaiseAndSetIfChanged(ref _isDirty, value);
  }
}