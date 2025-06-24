using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace MaterialSwapperFramework.Gui.Services;

public class StatusService : INotifyPropertyChanged
{
  public event PropertyChangedEventHandler? PropertyChanged;

  private string _statusText = string.Empty;
  public string StatusText
  {
    get => _statusText;
    set => SetField(ref _statusText, value);
  }

  protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
  {
    PropertyChanged?.Invoke(this, new(propertyName));
  }

  protected bool SetField<T>(ref T field, T value, [CallerMemberName] string? propertyName = null)
  {
    if (EqualityComparer<T>.Default.Equals(field, value)) return false;
    field = value;
    OnPropertyChanged(propertyName);
    return true;
  }

  public void UpdateStatus(string statusText)
  {
    StatusText = statusText;
  }

  public void DisplayStatusDuring(Action action, string statusText)
  {
    try
    {
      StatusText = statusText;
      action();
    }
    finally
    {
      StatusText = string.Empty;
    }
  }

  public async Task DisplayStatusDuringAsync(Func<Task> action, string statusText)
  {
    try
    {
      StatusText = statusText;
      await action();
    }
    finally
    {
      StatusText = string.Empty;
    }
  }
}