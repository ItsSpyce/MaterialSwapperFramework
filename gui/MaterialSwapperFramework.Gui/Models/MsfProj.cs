using ReactiveUI;

namespace MaterialSwapperFramework.Gui.Models;

public class MsfProj : ReactiveObject
{
  public const string Extension = ".msfproj";

  private string _name = string.Empty;
  private string? _version;
  private string? _dataDir;

  public string Name
  {
    get => _name;
    set => this.RaiseAndSetIfChanged(ref _name, value);
  }

  public string? Version
  {
    get => _version;
    set => this.RaiseAndSetIfChanged(ref _version, value);
  }

  public string? DataDir
  {
    get => _dataDir;
    set => this.RaiseAndSetIfChanged(ref _dataDir, value);
  }
}