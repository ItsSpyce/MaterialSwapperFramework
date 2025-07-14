using CommunityToolkit.Mvvm.Input;
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
using System.Threading.Tasks;

namespace MaterialSwapperFramework.Gui.ViewModels;

public partial class MainViewModel : ViewModelBase
{
  public class PluginTreeNode(string label, object? value, PluginTreeNode? parent)
  {
    public string Label { get; set; } = label;
    public object? Value { get; set; } = value;
    public PluginTreeNode? Parent { get; set; } = parent;
    public IEnumerable<PluginTreeNode>? Nodes { get; set; }
  }

  private readonly IMaterialService _materialService = new MaterialService();
  private readonly IModService _modService = new ModService();

  private bool _isReady = false;
  public bool IsReady
  {
    get => _isReady;
    set => this.RaiseAndSetIfChanged(ref _isReady, value);
  }

  public string? DataDir { get; set; }

  private ObservableCollection<string> _recentFiles = [];
  public ObservableCollection<string> RecentFiles
  {
    get => _recentFiles;
    set => this.RaiseAndSetIfChanged(ref _recentFiles, value);
  }

  private MaterialRecord? _selectedMaterial;
  public MaterialRecord? SelectedMaterial
  {
    get => _selectedMaterial;
    set => this.RaiseAndSetIfChanged(ref _selectedMaterial, value);
  }
  
  private ObservableCollection<string> _modFiles = [];
  public ObservableCollection<string> ModFiles
  {
    get => _modFiles;
    set => this.RaiseAndSetIfChanged(ref _modFiles, value);
  }

  private ObservableAsPropertyHelper<IEnumerable<string>>? _allMaterials;
  public IEnumerable<string>? AllMaterials => _allMaterials?.Value;

  private ObservableAsPropertyHelper<IEnumerable<PluginTreeNode>>? _pluginNodes;
  public IEnumerable<PluginTreeNode>? PluginNodes => _pluginNodes?.Value;

  private ObservableAsPropertyHelper<IEnumerable<MsfConfigJson>>? _configJsons;
  public IEnumerable<MsfConfigJson>? ConfigJsons => _configJsons?.Value;

  private ModifyCurrentRecordViewModel? _modifyCurrentRecordView;
  public ModifyCurrentRecordViewModel? ModifyCurrentRecordView
  {
    get => _modifyCurrentRecordView;
    set => this.RaiseAndSetIfChanged(ref _modifyCurrentRecordView, value);
  }

  
  protected override void Configure()
  {
    this.SubscribePropertyChangingEvents();
    PropertyChanging += (_, args) =>
    {
      if (args.PropertyName == nameof(ModFiles))
      {
        foreach (var node in PluginNodes)
        {
          if (node.Value is ISkyrimModDisposableGetter dis)
          {
            dis.Dispose();
          }
        }
      }
    };

    _allMaterials = this
      .WhenAnyValue(x => x.ModFiles)
      .Throttle(TimeSpan.FromMilliseconds(200))
      .Select(x =>
      {
        if (x is null or []) return null;
        var dataDir = Path.GetDirectoryName(x.First());
        return _materialService.GetMaterialFiles(dataDir);
      })
      .DistinctUntilChanged()
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.AllMaterials);

    _configJsons = this
      .WhenAnyValue(x => x.ModFiles)
      .Throttle(TimeSpan.FromMilliseconds(200))
      .SelectMany(GetConfigJsons)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.ConfigJsons);

    _pluginNodes = this
      .WhenAnyValue(x => x.ModFiles)
      .Select(BuildPluginTree)
      .DistinctUntilChanged()
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.PluginNodes);
  }

  [RelayCommand]
  public async Task PluginMenuClick(PluginTreeNode node)
  {
    await Task.CompletedTask;
    if (node.Value is not ISkyrimMajorRecordGetter record)
    {
      return;
    }

    var mod = node.Parent!.Value as ISkyrimModDisposableGetter;

    ModifyCurrentRecordView = new(record, mod, DataDir, GetMaterialsForRecord(record), AllMaterials);
  }

  private IEnumerable<PluginTreeNode> BuildPluginTree(IEnumerable<string>? modFiles)
  {
    if (modFiles is null || !modFiles.Any())
    {
      yield return new("No mods loaded", null, null);
      yield break;
    }
    foreach (var modFile in modFiles)
    {
      var mod = _modService.LoadMod(modFile);
      var node = new PluginTreeNode(mod.ModKey.FileName, mod, null);
      var categories =
        new[]
        {
          AddSupportFor("Activators", mod, node, x => x.Activators),
          AddSupportFor("Addon Nodes", mod, node, x => x.AddonNodes),
          AddSupportFor("Ammunitions", mod, node, x => x.Ammunitions),
          AddSupportFor("Armors", mod, node, x => x.Armors),
          AddSupportFor("Art Objects", mod, node, x => x.ArtObjects),
          AddSupportFor("Body Parts", mod, node, x => x.BodyParts),
          AddSupportFor("Books", mod, node, x => x.Books),
          AddSupportFor("Climates", mod, node, x => x.Climates),
          AddSupportFor("Containers", mod, node, x => x.Containers),
          AddSupportFor("Doors", mod, node, x => x.Doors),
          AddSupportFor("Effect Shaders", mod, node, x => x.EffectShaders),
          AddSupportFor("Explosions", mod, node, x => x.Explosions),
          AddSupportFor("Flora", mod, node, x => x.Florae),
          AddSupportFor("Furniture", mod, node, x => x.Furniture),
          AddSupportFor("Hazards", mod, node, x => x.Hazards),
          AddSupportFor("Head Parts", mod, node, x => x.HeadParts),
          AddSupportFor("Ingestibles", mod, node, x => x.Ingestibles),
          AddSupportFor("Ingredients", mod, node, x => x.Ingredients),
          AddSupportFor("Landscape Textures", mod, node, x => x.LandscapeTextures),
          AddSupportFor("Misc Items", mod, node, x => x.MiscItems),
          AddSupportFor("Moveable Statics", mod, node, x => x.MoveableStatics),
          AddSupportFor("Projectiles", mod, node, x => x.Projectiles),
          AddSupportFor("Scrolls", mod, node, x => x.Scrolls),
          AddSupportFor("Soul Gems", mod, node, x => x.SoulGems),
          AddSupportFor("Statics", mod, node, x => x.Statics),
          AddSupportFor("Texture Sets", mod, node, x => x.TextureSets),
          AddSupportFor("Trees", mod, node, x => x.Trees),
          AddSupportFor("Waters", mod, node, x => x.Waters),
          AddSupportFor("Weapons", mod, node, x => x.Weapons),
        }.Where(x => x is not null);
      node.Nodes = categories;
      yield return node;
    }

    yield break;

    PluginTreeNode? AddSupportFor<T>(string displayName,
      ISkyrimModDisposableGetter mod,
      PluginTreeNode parent,
      Expression<Func<ISkyrimModDisposableGetter, ISkyrimGroupGetter<T>>> getter)
      where T : class, ISkyrimMajorRecordGetter
    {
      var field = getter.Compile().Invoke(mod);
      if (!field.Any())
      {
        return null;
      }

      var groupNode = new PluginTreeNode(displayName, parent.Value, parent);
      var forms = field
        .Select(form =>
          new PluginTreeNode($"{form.FormKey.IDString()} | {form.EditorID ?? string.Empty}", form, groupNode));
      groupNode.Nodes = forms;
      return groupNode;
    }
  }

  private async Task<IEnumerable<MsfConfigJson>> GetConfigJsons(IEnumerable<string>? modFiles)
  {
    if (modFiles is null || !modFiles.Any()) return [new("None", string.Empty, [])];
    return await _materialService?.LoadConfigJsonsAsync(DataDir) ?? [];
  }

  private IEnumerable<MaterialRecord>? GetMaterialsForRecord(ISkyrimMajorRecordGetter? record)
  {
    if (record is null || ConfigJsons is null) return null;
    var configJsonsForRecord = ConfigJsons.Where(c =>
      c.PluginName.Equals(record.FormKey.ModKey.FileName.String, StringComparison.CurrentCultureIgnoreCase) &&
      c.FormID.Equals(record.FormKey.ID.ToString("X")));
    var jsonsForRecord = configJsonsForRecord as MsfConfigJson[] ?? configJsonsForRecord.ToArray();
    return jsonsForRecord
      .SelectMany(c => c.Records);
  }
}
