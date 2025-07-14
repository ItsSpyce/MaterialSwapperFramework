using CommunityToolkit.Mvvm.Input;
using MaterialSwapperFramework.Gui.Models;
using Mutagen.Bethesda.Plugins.Cache;
using Mutagen.Bethesda.Skyrim;
using NiflySharp;
using Noggog;
using ReactiveUI;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Reactive.Linq;
using System.Threading.Tasks;

namespace MaterialSwapperFramework.Gui.ViewModels;
public partial class AppliesEntry(
  string shapeName,
  string materialFile,
  bool isTemp,
  MaterialRecordViewModel source) : ViewModelBase
{
  private string _shapeName = shapeName;
  public string ShapeName
  {
    get => _shapeName;
    set => this.RaiseAndSetIfChanged(ref _shapeName, value);
  }

  private string _materialFile = materialFile;
  public string MaterialFile
  {
    get => _materialFile;
    set => this.RaiseAndSetIfChanged(ref _materialFile, value);
  }

  public bool IsTemp => isTemp;

  public MaterialRecordViewModel Source => source;

  [RelayCommand]
  public void RemoveFromSource()
  {
    Source.Applies.RemoveWhere(x => x.ShapeName == ShapeName);
  }

  [RelayCommand]
  public void AddShapeAndMaterial()
  {
    Source.Applies.Add(new(ShapeName, MaterialFile, false, source));
  }
}

public partial class MaterialRecordViewModel(
  MaterialRecord material,
  string dataDir,
  ModifyCurrentRecordViewModel parent,
  IEnumerable<string> availableMaterialFiles,
  ISkyrimMajorRecordGetter record,
  ILinkCache<ISkyrimMod, ISkyrimModGetter> linkCache) : ViewModelBase
{
  private readonly ModifyCurrentRecordViewModel _parent = parent;

  private ISkyrimMajorRecordGetter _record = record;
  public ISkyrimMajorRecordGetter Record
  {
    get => _record;
    set => this.RaiseAndSetIfChanged(ref _record, value);
  }

  private string _name;
  public string Name
  {
    get => _name;
    set => this.RaiseAndSetIfChanged(ref _name, value);
  }

  private bool? _isHidden;
  public bool? IsHidden
  {
    get => _isHidden;
    set => this.RaiseAndSetIfChanged(ref _isHidden, value);
  }

  private bool? _modifyName;
  public bool? ModifyName
  {
    get => _modifyName;
    set => this.RaiseAndSetIfChanged(ref _modifyName, value);
  }

  private ObservableCollection<AppliesEntry> _applies;
  public ObservableCollection<AppliesEntry> Applies
  {
    get => _applies;
    set => this.RaiseAndSetIfChanged(ref _applies, value);
  }

  private ObservableAsPropertyHelper<IEnumerable<string>> _availableShapes;
  public IEnumerable<string> AvailableShapes => _availableShapes.Value;

  private ObservableAsPropertyHelper<IEnumerable<string>> _availableMaterialFiles;
  public IEnumerable<string> AvailableMaterialFiles => _availableMaterialFiles.Value;

  private ObservableAsPropertyHelper<bool> _isActivator;
  public bool IsActivator => _isActivator.Value;

  private ObservableAsPropertyHelper<bool> _isAddonNode;
  public bool IsAddonNode => _isAddonNode.Value;

  private ObservableAsPropertyHelper<bool> _isAmmunition;
  public bool IsAmmunition => _isAmmunition.Value;

  private ObservableAsPropertyHelper<bool> _isArmor;
  public bool IsArmor => _isArmor.Value;

  private MaterialRecord.MaterialRecordGender? _gender = material.Gender;

  public MaterialRecord.MaterialRecordGender? Gender
  {
    get => _gender;
    set => this.RaiseAndSetIfChanged(ref _gender, value);
  }

  private ObservableAsPropertyHelper<bool> _isArtObject;
  public bool IsArtObject => _isArtObject.Value;

  private ObservableAsPropertyHelper<bool> _isBodyPart;
  public bool IsBodyPart => _isBodyPart.Value;

  private ObservableAsPropertyHelper<bool> _isBook;
  public bool IsBook => _isBook.Value;

  private ObservableAsPropertyHelper<bool> _isClimate;
  public bool IsClimate => _isClimate.Value;

  private ObservableAsPropertyHelper<bool> _isContainer;
  public bool IsContainer => _isContainer.Value;

  private ObservableAsPropertyHelper<bool> _isDoor;
  public bool IsDoor => _isDoor.Value;

  private ObservableAsPropertyHelper<bool> _isEffectShader;
  public bool IsEffectShader => _isEffectShader.Value;

  private ObservableAsPropertyHelper<bool> _isExplosion;
  public bool IsExplosion => _isExplosion.Value;

  private ObservableAsPropertyHelper<bool> _isFlora;
  public bool IsFlora => _isFlora.Value;

  private ObservableAsPropertyHelper<bool> _isFurniture;
  public bool IsFurniture => _isFurniture.Value;

  private ObservableAsPropertyHelper<bool> _isHazard;
  public bool IsHazard => _isHazard.Value;

  private ObservableAsPropertyHelper<bool> _isHeadPart;
  public bool IsHeadPart => _isHeadPart.Value;

  private ObservableAsPropertyHelper<bool> _isIngestible;
  public bool IsIngestible => _isIngestible.Value;

  private ObservableAsPropertyHelper<bool> _isIngredient;
  public bool IsIngredient => _isIngredient.Value;

  private ObservableAsPropertyHelper<bool> _isLandscapeTexture;
  public bool IsLandscapeTexture => _isLandscapeTexture.Value;

  private ObservableAsPropertyHelper<bool> _isMiscItem;
  public bool IsMiscItem => _isMiscItem.Value;

  private ObservableAsPropertyHelper<bool> _isMoveableStatic;
  public bool IsMoveableStatic => _isMoveableStatic.Value;

  private ObservableAsPropertyHelper<bool> _isProjectile;
  public bool IsProjectile => _isProjectile.Value;

  private ObservableAsPropertyHelper<bool> _isScroll;
  public bool IsScroll => _isScroll.Value;

  private ObservableAsPropertyHelper<bool> _isSoulGem;
  public bool IsSoulGem => _isSoulGem.Value;

  private ObservableAsPropertyHelper<bool> _isStatic;
  public bool IsStatic => _isStatic.Value;

  private ObservableAsPropertyHelper<bool> _isTextureSet;
  public bool IsTextureSet => _isTextureSet.Value;

  private ObservableAsPropertyHelper<bool> _isTree;
  public bool IsTree => _isTree.Value;

  private ObservableAsPropertyHelper<bool> _isWater;
  public bool IsWater => _isWater.Value;

  private ObservableAsPropertyHelper<bool> _isWeapon;
  public bool IsWeapon => _isWeapon.Value;

  protected override void Configure()
  {
    Name = material.Name;
    IsHidden = material.IsHidden;
    ModifyName = material.ModifyName;
    Applies = new(Enumerable.AsEnumerable(material.Applies).Select(x => new AppliesEntry(x.Key, x.Value, false, this)))
    {
      new(string.Empty, string.Empty, true, this)
    };

    _availableMaterialFiles = this
      .WhenAnyValue(x => x.Applies)
      .Select(x => availableMaterialFiles.Where(mat =>
        !x.Any(e => !string.IsNullOrEmpty(e.MaterialFile) && e.MaterialFile.Equals(mat, StringComparison.CurrentCultureIgnoreCase))))
      .Select(x => x.Select(filename => filename.Replace('\\', '/')))
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.AvailableMaterialFiles);
    _availableShapes = this
      .WhenAnyValue(x => x.Applies, x => x.Gender)
      .SelectMany(args => GetShapesForRecord(args.Item1))
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.AvailableShapes);

    _isActivator = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IActivatorGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsActivator);

    _isAddonNode = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IAddonNodeGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsAddonNode);

    _isAmmunition = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IAmmunitionGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsAmmunition);

    _isArmor = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IArmorGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsArmor);

    _isArtObject = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IArtObjectGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsArtObject);

    _isBodyPart = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IBodyPartDataGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsBodyPart);

    _isBook = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IBookGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsBook);

    _isClimate = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IClimateGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsClimate);

    _isContainer = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IContainerGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsContainer);

    _isDoor = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IDoorGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsDoor);

    _isEffectShader = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IEffectShaderGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsEffectShader);

    _isExplosion = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IExplosionGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsExplosion);

    _isFlora = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IFloraGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsFlora);

    _isFurniture = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IFurnitureGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsFurniture);

    _isHazard = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IHazardGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsHazard);

    _isHeadPart = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IHeadPartGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsHeadPart);

    _isIngestible = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IIngestibleGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsIngestible);

    _isIngredient = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IIngredientGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsIngredient);

    _isLandscapeTexture = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is ILandscapeTextureGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsLandscapeTexture);

    _isMiscItem = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IMiscItemGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsMiscItem);

    _isMoveableStatic = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IMoveableStaticGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsMoveableStatic);

    _isProjectile = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IProjectileGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsProjectile);

    _isScroll = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IScrollGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsScroll);

    _isSoulGem = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is ISoulGemGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsSoulGem);

    _isStatic = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IStaticGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsStatic);

    _isTextureSet = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is ITextureSetGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsTextureSet);

    _isTree = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is ITreeGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsTree);

    _isWater = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IWaterGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsWater);

    _isWeapon = this
      .WhenAnyValue(x => x.Record)
      .Select(x => x is IWeaponGetter)
      .ObserveOn(RxApp.MainThreadScheduler)
      .ToProperty(this, x => x.IsWeapon);
  }

  [RelayCommand]
  public async Task SaveMaterialRecord()
  {

  }

  [RelayCommand]
  public async Task RevertMaterialRecord()
  {
    Configure();
  }

  
  private async Task<IEnumerable<string>> GetShapesForRecord(IEnumerable<AppliesEntry> applies)
  {
    IEnumerable<string> availableShapes = [];
    if (_record is IArmorGetter armor)
    {
      var armorAddonLink = armor.Armature.FirstOrDefault();
      var addon = armorAddonLink?.TryResolve(linkCache);
      if (addon is not null)
      {
        var mesh = Gender switch
        {
          MaterialRecord.MaterialRecordGender.Male => addon.WorldModel?.Male,
          MaterialRecord.MaterialRecordGender.Female => addon.WorldModel?.Female,
          _ => addon.WorldModel?.FirstOrDefault(x => x is not null),
        };
        if (mesh is not null)
        {
          var nifPath = Path.Join(dataDir, mesh.File.DataRelativePath.Path);
          if (File.Exists(nifPath))
          {
            await using var fs = File.OpenRead(nifPath);
            var nifFile = new NifFile(fs, new() { IsTerrainFile = false });
            availableShapes = nifFile.GetShapes().Select(x => x.Name.String);
          }
        }
      }
    }

    return availableShapes.Except(applies.Select(a => a.ShapeName));
  }
}