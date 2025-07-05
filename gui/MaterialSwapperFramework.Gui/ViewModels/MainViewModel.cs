using CommunityToolkit.Mvvm.Input;
using DynamicData;
using MaterialSwapperFramework.Gui.Models;
using MaterialSwapperFramework.Gui.Services;
using Mutagen.Bethesda.Skyrim;
using ReactiveUI;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Linq.Expressions;
using System.Reactive.Linq;
using System.Threading;
using System.Threading.Tasks;
using Mutagen.Bethesda.Plugins.Records;

namespace MaterialSwapperFramework.Gui.ViewModels;

public partial class MainViewModel(IModService? modService, IProjectService? projectService, IMaterialService? materialService) : ViewModelBase
{
  public MainViewModel() : this(null, null, null)
  {
  }

  public class PluginTreeNode
  {
    public required string Label { get; set; }
    public required object? Value { get; set; }
    public ObservableCollection<PluginTreeNode>? Nodes { get; set; }
  }

  private ObservableCollection<string> _recentFiles = [];
  public ObservableCollection<string> RecentFiles
  {
    get => _recentFiles;
    set => this.RaiseAndSetIfChanged(ref _recentFiles, value);
  }
  
  private ISkyrimMajorRecordGetter? _loadedRecord;
  public ISkyrimMajorRecordGetter? LoadedRecord
  {
    get => _loadedRecord;
    set => this.RaiseAndSetIfChanged(ref _loadedRecord, value);
  }

  private ObservableCollection<string>? _loadedModFiles;
  public ObservableCollection<string>? LoadedModFiles
  {
    get => _loadedModFiles;
    set => this.RaiseAndSetIfChanged(ref _loadedModFiles, value);
  }
  
  private MaterialRecord? _selectedMaterial;
  public MaterialRecord? SelectedMaterial
  {
    get => _selectedMaterial;
    set => this.RaiseAndSetIfChanged(ref _selectedMaterial, value);
  }

  private MsfProj? _project;
  public MsfProj? Project
  {
    get => _project;
    set => this.RaiseAndSetIfChanged(ref _project, value);
  }

  private ISkyrimMajorRecordGetter? _selectedRecord;

  public ISkyrimMajorRecordGetter? SelectedRecord
  {
    get => _selectedRecord;
    set => this.RaiseAndSetIfChanged(ref _selectedRecord, value);
  }

  private ObservableAsPropertyHelper<IEnumerable<MaterialRecord>>? _loadedMaterials;
  public IEnumerable<MaterialRecord>? LoadedMaterials => _loadedMaterials?.Value;

  private ObservableAsPropertyHelper<IEnumerable<PluginTreeNode>>? _pluginNodes;
  public IEnumerable<PluginTreeNode>? PluginNodes => _pluginNodes?.Value;

  private ObservableAsPropertyHelper<IEnumerable<MaterialRecord>>? _materialsForRecord;
  public IEnumerable<MaterialRecord>? MaterialsForRecord => _materialsForRecord?.Value;

  private ObservableAsPropertyHelper<IEnumerable<MsfConfigJson>>? _configJsons;
  public IEnumerable<MsfConfigJson>? ConfigJsons => _configJsons?.Value;

  protected override void Configure()
  {
    this.SubscribePropertyChangingEvents();
    this.PropertyChanging += (_, args) =>
    {
      if (args.PropertyName == nameof(LoadedModFiles))
      {
        SelectedRecord = null;
        foreach (var node in PluginNodes)
        {
          if (node.Value is ISkyrimModDisposableGetter dis)
          {
            dis.Dispose();
          }
        }
      }
    };

    _loadedMaterials = this
      .WhenAnyValue(x => x.LoadedModFiles)
      .Throttle(TimeSpan.FromMilliseconds(200))
      .Select(x =>
      {
        if (x?.Count == null) return null;
        var dataDir = Path.GetDirectoryName(x[0]);
        return dataDir;
      })
      .DistinctUntilChanged()
      .SelectMany(LoadMaterialRecordsAsync)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.LoadedMaterials);

    _configJsons = this
      .WhenAnyValue(x => x.LoadedModFiles)
      .Throttle(TimeSpan.FromMilliseconds(200))
      .SelectMany(GetConfigJsons)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.ConfigJsons);

    _pluginNodes = this
      .WhenAnyValue(x => x.LoadedModFiles)
      .Select(BuildPluginTree)
      .DistinctUntilChanged()
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.PluginNodes);

    _materialsForRecord = this
      .WhenAnyValue(x => x.SelectedRecord)
      .Select(GetMaterialsForRecord)
      .DistinctUntilChanged()
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.MaterialsForRecord);
  }

  [RelayCommand]
  public async Task PluginMenuClick(object? value)
  {
    await Task.CompletedTask;
    if (value is not ISkyrimMajorRecordGetter record)
    {
      return;
    }

    SelectedRecord = record;
  }

  [RelayCommand]
  public async Task SelectMaterialEntry(MaterialRecord material)
  {
    SelectedMaterial = material;
  }

  private async Task<IEnumerable<MaterialRecord>> LoadMaterialRecordsAsync(string? dataDir, CancellationToken _)
  {
    if (string.IsNullOrEmpty(dataDir))
    {
      return [];
    }
    var materialJsons = await materialService.LoadConfigJsonsAsync(dataDir);
    return materialJsons.SelectMany(x => x.Records);
  }

  private IEnumerable<PluginTreeNode> BuildPluginTree(IEnumerable<string>? mods)
  {
    if (mods is null || !mods.Any()) return [new() { Label = "No mods loaded", Nodes = [], Value = null }];
    return from filename in mods
      let mod = modService.LoadMod(filename)
      let label = Path.GetFileName(filename)
      let categories =
        new[]
        {
          AddSupportFor("Activators", mod, x => x.Activators),
          AddSupportFor("Addon Nodes", mod, x => x.AddonNodes),
          AddSupportFor("Ammunitions", mod, x => x.Ammunitions),
          AddSupportFor("Armors", mod, x => x.Armors),
          AddSupportFor("Art Objects", mod, x => x.ArtObjects),
          AddSupportFor("Body Parts", mod, x => x.BodyParts),
          AddSupportFor("Books", mod, x => x.Books),
          AddSupportFor("Climates", mod, x => x.Climates),
          AddSupportFor("Containers", mod, x => x.Containers),
          AddSupportFor("Doors", mod, x => x.Doors),
          AddSupportFor("Effect Shaders", mod, x => x.EffectShaders),
          AddSupportFor("Explosions", mod, x => x.Explosions),
          AddSupportFor("Flora", mod, x => x.Florae),
          AddSupportFor("Furniture", mod, x => x.Furniture),
          AddSupportFor("Hazards", mod, x => x.Hazards),
          AddSupportFor("Head Parts", mod, x => x.HeadParts),
          AddSupportFor("Ingestibles", mod, x => x.Ingestibles),
          AddSupportFor("Ingredients", mod, x => x.Ingredients),
          AddSupportFor("Landscape Textures", mod, x => x.LandscapeTextures),
          AddSupportFor("Misc Items", mod, x => x.MiscItems),
          AddSupportFor("Moveable Statics", mod, x => x.MoveableStatics),
          AddSupportFor("Projectiles", mod, x => x.Projectiles),
          AddSupportFor("Scrolls", mod, x => x.Scrolls),
          AddSupportFor("Soul Gems", mod, x => x.SoulGems),
          AddSupportFor("Statics", mod, x => x.Statics),
          AddSupportFor("Texture Sets", mod, x => x.TextureSets),
          AddSupportFor("Trees", mod, x => x.Trees),
          AddSupportFor("Waters", mod, x => x.Waters),
          AddSupportFor("Weapons", mod, x => x.Weapons),
        }.Where(x => x is not null)
      select new PluginTreeNode { Label = label, Nodes = new(categories), Value = mod };
    
    PluginTreeNode? AddSupportFor<T>(string displayName,
      ISkyrimModDisposableGetter mod,
      Expression<Func<ISkyrimModDisposableGetter, ISkyrimGroupGetter<T>>> getter)
      where T : class, ISkyrimMajorRecordGetter
    {
      var field = getter.Compile().Invoke(mod);
      if (!field.Any())
      {
        return null;
      }

      var forms = field
        .Select(form => new PluginTreeNode { Label = $"{form.FormKey.IDString()} | {form.EditorID ?? string.Empty}", Value = form });
      return new() { Label = displayName, Value = null, Nodes = new(forms) };
    }
  }

  private async Task<IEnumerable<MsfConfigJson>> GetConfigJsons(IEnumerable<string>? modFiles)
  {
    if (modFiles is null || !modFiles.Any()) return [new("None", string.Empty, [])];
    foreach (var file in modFiles ?? [])
    {
      var dataDir = Path.GetDirectoryName(file);
      if (string.IsNullOrEmpty(dataDir)) continue;
      return (await materialService?.LoadConfigJsonsAsync(dataDir)) ?? [];
    }
    return [];
  }

  private IEnumerable<MaterialRecord> GetMaterialsForRecord(ISkyrimMajorRecordGetter? record)
  {
    if (record is null || ConfigJsons is null) return [new() { Name = "No materials loaded" }];
    var configJsonsForRecord = ConfigJsons.Where(c =>
      c.PluginName.Equals(record.FormKey.ModKey.FileName.String, StringComparison.CurrentCultureIgnoreCase) &&
      c.FormID.Equals(record.FormKey.ID.ToString("X")));
    var jsonsForRecord = configJsonsForRecord as MsfConfigJson[] ?? configJsonsForRecord.ToArray();
    return jsonsForRecord
      .SelectMany(c => c.Records);
  }
}
