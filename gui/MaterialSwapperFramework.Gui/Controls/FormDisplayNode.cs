using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Mutagen.Bethesda.Skyrim;
using IContainer = System.ComponentModel.IContainer;

namespace MaterialSwapperFramework.Gui.Controls;

public partial class FormDisplayNode : Component
{
  // components

  private readonly Label _label = new();

  // fields

  private ISkyrimMajorRecordGetter? _recordGetter;
  private string _text = "No Record";

  public ISkyrimMajorRecordGetter? RecordGetter
  {
    get => _recordGetter;
    set
    {
      _recordGetter = value;
      if (value != null)
      {
        Text = value.EditorID ?? "No Editor ID";
      }
      else
      {
        Text = "No Record";
      }
    }
  }

  public string Text
  {
    get => _text;
    set
    {
      _text = value;
      _label.Text = !string.IsNullOrEmpty(value) ? value : "No Record";
    }
  }

  public FormDisplayNode()
  {
    InitializeComponent();
    components = new System.ComponentModel.Container();
    
    components.Add(_label);
  }

  public FormDisplayNode(IContainer container) : this()
  {
    container.Add(this);
  }
}
