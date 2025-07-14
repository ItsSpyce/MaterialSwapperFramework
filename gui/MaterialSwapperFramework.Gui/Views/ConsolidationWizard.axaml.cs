using System.Collections.Generic;
using Avalonia.Controls;

namespace MaterialSwapperFramework.Gui.Views;

public partial class ConsolidationWizard : Window
{
  public IEnumerable<string> SelectedModFiles { get; set; }

  public ConsolidationWizard()
  {
    InitializeComponent();
  }
}