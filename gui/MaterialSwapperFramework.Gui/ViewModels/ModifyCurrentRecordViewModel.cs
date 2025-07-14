using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reactive.Linq;
using MaterialSwapperFramework.Gui.Models;
using Mutagen.Bethesda.Plugins.Cache;
using Mutagen.Bethesda.Skyrim;
using NiflySharp;
using ReactiveUI;

namespace MaterialSwapperFramework.Gui.ViewModels;

public partial class ModifyCurrentRecordViewModel(
  ISkyrimMajorRecordGetter record,
  ISkyrimModDisposableGetter mod,
  string dataDir,
  IEnumerable<MaterialRecord>? materials,
  IEnumerable<string> availableMaterials) : ViewModelBase
{
  public ILinkCache<ISkyrimMod, ISkyrimModGetter> LinkCache { get; private set; }

  private ISkyrimMajorRecordGetter _record = record;
  public ISkyrimMajorRecordGetter Record
  {
    get => _record;
    set => this.RaiseAndSetIfChanged(ref _record, value);
  }

  private IEnumerable<MaterialRecord>? _materials = materials;
  public IEnumerable<MaterialRecord>? Materials
  {
    get => _materials;
    set => this.RaiseAndSetIfChanged(ref _materials, value);
  }

  private MaterialRecordViewModel? _currentlyEditing;
  public MaterialRecordViewModel? CurrentlyEditing
  {
    get => _currentlyEditing;
    set => this.RaiseAndSetIfChanged(ref _currentlyEditing, value);
  }

  private ObservableAsPropertyHelper<bool> _showNifEdit;
  public bool ShowNifEdit => _showNifEdit.Value;

  protected override void Configure()
  {
    LinkCache = mod.ToImmutableLinkCache();
    _showNifEdit = this
      .WhenAnyValue(x => x.CurrentlyEditing)
      .Select(x => x is not null)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.ShowNifEdit);
  }

  public void SetCurrentlyEditing(MaterialRecord material)
  {
    CurrentlyEditing = new(material, dataDir, this, availableMaterials, Record, LinkCache);
  }
}