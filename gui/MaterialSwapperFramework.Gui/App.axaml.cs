using Avalonia;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Markup.Xaml;
using MaterialSwapperFramework.Gui.Services;
using MaterialSwapperFramework.Gui.ViewModels;
using MaterialSwapperFramework.Gui.Views;
using Microsoft.Extensions.DependencyInjection;

namespace MaterialSwapperFramework.Gui;

public partial class App : Application
{
  public override void Initialize()
  {
    AvaloniaXamlLoader.Load(this);
  }

  public override void OnFrameworkInitializationCompleted()
  {
    var services = new ServiceCollection();
    ConfigureServices(services);
    var serviceProvider = services.BuildServiceProvider();

    switch (ApplicationLifetime)
    {
      case IClassicDesktopStyleApplicationLifetime desktop:
        desktop.MainWindow = new MainWindow
        {
          DataContext = serviceProvider.GetRequiredService<MainViewModel>()
        };
        break;
      case ISingleViewApplicationLifetime singleViewPlatform:
        singleViewPlatform.MainView = serviceProvider.GetRequiredService<MainView>();
        break;
    }

    base.OnFrameworkInitializationCompleted();
  }

  void ConfigureServices(IServiceCollection services)
  {
    services
      .AddSingleton<MainViewModel>();

    services
      .AddScoped<IModService, ModService>()
      .AddScoped<IProjectService, ProjectService>()
      .AddScoped<IMaterialService, MaterialService>();
  }
}
