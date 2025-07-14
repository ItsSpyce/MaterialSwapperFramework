using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.Primitives;
using Avalonia.Interactivity;
using Avalonia.Platform.Storage;
using DynamicData.Binding;
using MaterialSwapperFramework.Gui.Models;
using MaterialSwapperFramework.Gui.Services;
using MaterialSwapperFramework.Gui.ViewModels;
using Noggog;

namespace MaterialSwapperFramework.Gui.Views;

public partial class MainView : UserControl
{
  private IEnumerable<string> _espFiles = [];
  public IEnumerable<string> EspFiles
  {
    get => Model.ModFiles;
    set => Model.ModFiles = new(value);
  }
  public MainViewModel Model => (MainViewModel)DataContext;

  public MainView()
  {
    DataContext = new MainViewModel();
    InitializeComponent();
  }

  private void CurrentMaterialSelectionChanged(object? sender, SelectionChangedEventArgs e)
  {
    if (e.AddedItems.Any())
    {
      if (e.AddedItems[0] is MaterialRecord item) Model.ModifyCurrentRecordView.SetCurrentlyEditing(item);
    }
  }

  private void AddButtonClick(object? sender, RoutedEventArgs e)
  {
    if (e.Source is not Button btn) return;
    StyledElement? dataGridCellPresenter = btn.Parent;
    while (dataGridCellPresenter is not null)
    {
      if (dataGridCellPresenter is DataGridCellsPresenter) break;
      dataGridCellPresenter = dataGridCellPresenter.Parent;
    }

    if (dataGridCellPresenter is not DataGridCellsPresenter presenter) return;
    var shapeName = ((ComboBox)((DataGridCell)presenter.Children[0]).Content).SelectedValue as string;
    var material = ((ComboBox)((DataGridCell)presenter.Children[1]).Content).SelectedValue as string;
    if (string.IsNullOrEmpty(shapeName) || string.IsNullOrEmpty(material)) return;
    Model.ModifyCurrentRecordView.CurrentlyEditing.Applies =
    [
      ..Model.ModifyCurrentRecordView.CurrentlyEditing.Applies.TakeWhile(x => !string.IsNullOrEmpty(x.ShapeName)),
      new(shapeName, material, false, Model.ModifyCurrentRecordView.CurrentlyEditing),
      new(string.Empty, string.Empty, true, Model.ModifyCurrentRecordView.CurrentlyEditing),
    ];
  }
}
