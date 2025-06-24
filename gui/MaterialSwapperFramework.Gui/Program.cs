namespace MaterialSwapperFramework.Gui
{
  internal static class Program
  {
    /// <summary>
    ///  The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main(params string[] args)
    {
      // To customize application configuration such as set high DPI settings or default font,
      // see https://aka.ms/applicationconfiguration.
      ApplicationConfiguration.Initialize();
      string workingDirectory;
      if (args.Length == 0 || string.IsNullOrEmpty(args[0]))
      {
        workingDirectory = AppDomain.CurrentDomain.BaseDirectory;
      }
      else
      {
        workingDirectory = args[0];
      }
      Application.Run(new MainWindow(workingDirectory));
    }
  }
}