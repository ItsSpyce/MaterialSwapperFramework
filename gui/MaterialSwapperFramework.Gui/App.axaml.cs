using Avalonia;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Markup.Xaml;
using MaterialSwapperFramework.Gui.ViewModels;
using MaterialSwapperFramework.Gui.Views;

namespace MaterialSwapperFramework.Gui;

public partial class App : Application
{
  public override void Initialize()
  {
    AvaloniaXamlLoader.Load(this);
  }

  public override void OnFrameworkInitializationCompleted()
  {
    switch (ApplicationLifetime)
    {
      case IClassicDesktopStyleApplicationLifetime desktop:
        desktop.MainWindow = new MainWindow
        {
          DataContext = new MainWindowViewModel(),
        };
        break;
      case ISingleViewApplicationLifetime singleViewPlatform:
        singleViewPlatform.MainView = new MainView
        {
          DataContext = new MainViewModel(),
        };
        break;
    }

    base.OnFrameworkInitializationCompleted();
  }
}
