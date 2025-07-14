using System.Collections.Generic;
using System.Collections.ObjectModel;
using ReactiveUI;

namespace MaterialSwapperFramework.Gui.ViewModels;

public class MainWindowViewModel : ViewModelBase
{
  private bool _isReady = false;
  public bool IsReady
  {
    get => _isReady;
    set => this.RaiseAndSetIfChanged(ref _isReady, value);
  }

  private IEnumerable<string> _allModFiles = [];
  public IEnumerable<string> AllModFiles
  {
    get => _allModFiles;
    set => this.RaiseAndSetIfChanged(ref _allModFiles, value);
  }

  private ObservableCollection<string> _selectedModFiles = [];
  public ObservableCollection<string> SelectedModFiles
  {
    get => _selectedModFiles;
    set => this.RaiseAndSetIfChanged(ref _selectedModFiles, value);
  }

  private string? _dataDir;
  public string? DataDir
  {
    get => _dataDir;
    set => this.RaiseAndSetIfChanged(ref _dataDir, value);
  }
}