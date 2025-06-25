using System.Linq.Expressions;
using MaterialSwapperFramework.Gui.Services;
using Mutagen.Bethesda.Plugins;
using Mutagen.Bethesda.Skyrim;

namespace MaterialSwapperFramework.Gui;
public partial class MainWindow : Form
{
  private string _workingDirectory;
  private static readonly string[] _pluginFileExtensions = [".esp", ".esm", ".esl"];
  private readonly string[] _pluginFiles = [];
  private readonly StatusService _statusService = new();
  private Dictionary<string, List<MaterialDataConfig>> _editedMaterials = [];

  struct MaterialDataConfig
  {
    public FormKey FormKey;
    public string MaterialName;
    public string[] Shapes;
    public string[] Flags;
  }

  public MainWindow(string directory)
  {
    InitializeComponent();
    _workingDirectory = Path.Join(directory, "Data");
    _statusService.PropertyChanged += (_, args) =>
    {
      if (args.PropertyName == nameof(StatusService.StatusText))
      {
        statusLabel.Text = _statusService.StatusText;
      }
    };
    pluginTreeView.AfterSelect += (_, args) =>
    {
      if (args.Node is null or { Tag: null }) return;
      DisplayRecord(args.Node);
    };
  }

  private bool LoadPluginFiles()
  {
    _statusService.UpdateStatus("Loading plugins...");
    if (!Directory.Exists(_workingDirectory))
    {
      pluginTreeView.Nodes.Clear();
      pluginTreeView.Nodes.Add(new TreeNode("No plugins found"));
      _statusService.UpdateStatus("Data directory not found");
      return false;
    }

    var pluginFiles = Directory.GetFiles(_workingDirectory)
      .Where(filename => _pluginFileExtensions.Contains(
        Path.GetExtension(filename).ToLower()))
      .OrderByDescending(filename => File.GetCreationTime(Path.Join(filename)))
      .Select(filename => Path.GetRelativePath(_workingDirectory, filename));
    pluginTreeView.Nodes.Clear();
    foreach (var pluginFile in pluginFiles)
    {
      var node = new TreeNode(pluginFile)
      {
        Tag = pluginFile
      };
      var index = pluginTreeView.Nodes.Add(node);
      if (LoadRecordsInPlugin(pluginFile, index)) continue;
      node.ForeColor = Color.Red;
      node.Text += " (Failed to load)";
    }
    if (pluginTreeView.Nodes.Count == 0)
    {
      pluginTreeView.Nodes.Add(new TreeNode("No plugins found"));
    }
    _statusService.UpdateStatus("Plugins loaded successfully");
    return true;
  }


  private bool LoadRecordsInPlugin(string pluginFile, int treeIndex)
  {
    using var loadedMod = SkyrimMod.Create(SkyrimRelease.SkyrimSE)
      .FromPath(ModPath.FromPath(Path.Join(_workingDirectory, pluginFile)))
      .WithDataFolder(_workingDirectory)
      .Parallel()
      .Construct();
    var treeNode = pluginTreeView.Nodes[treeIndex];
    treeNode.Tag = loadedMod.ModKey;

    void AddSupportFor<T>(string displayName, Expression<Func<ISkyrimModDisposableGetter, ISkyrimGroupGetter<T>>> getter) where T : class, ISkyrimMajorRecordGetter
    {
      var field = getter.Compile().Invoke(loadedMod);
      if (!field.Any())
      {
        return;
      }
      var groupNode = new TreeNode(displayName)
      {
        Tag = field.Type,
      };
      foreach (var record in field)
      {
        var recordNode = new TreeNode($"{record.FormKey.IDString()}|{record.EditorID ?? string.Empty}")
        {
          Tag = record,
        };
        if (_editedMaterials.ContainsKey(record.FormKey.ToString()))
        {
          recordNode.BackColor = Color.LightGreen;
        }
        groupNode.Nodes.Add(recordNode);
      }
      treeNode.Nodes.Add(groupNode);
    }

    AddSupportFor("Activators", (mod) => mod.Activators);
    AddSupportFor("Addon Nodes", (mod) => mod.AddonNodes);
    AddSupportFor("Ammunitions", (mod) => mod.Ammunitions);
    AddSupportFor("Armors", (mod) => mod.Armors);
    AddSupportFor("Art Objects", (mod) => mod.ArtObjects);
    AddSupportFor("Body Parts", (mod) => mod.BodyParts);
    AddSupportFor("Books", (mod) => mod.Books);
    AddSupportFor("Climates", (mod) => mod.Climates);
    AddSupportFor("Containers", (mod) => mod.Containers);
    AddSupportFor("Doors", (mod) => mod.Doors);
    AddSupportFor("Effect Shaders", (mod) => mod.EffectShaders);
    AddSupportFor("Explosions", (mod) => mod.Explosions);
    AddSupportFor("Flora", (mod) => mod.Florae);
    AddSupportFor("Furniture", (mod) => mod.Furniture);
    AddSupportFor("Hazards", (mod) => mod.Hazards);
    AddSupportFor("Head Parts", (mod) => mod.HeadParts);
    AddSupportFor("Ingestibles", (mod) => mod.Ingestibles);
    AddSupportFor("Ingredients", (mod) => mod.Ingredients);
    AddSupportFor("Landscape Textures", (mod) => mod.LandscapeTextures);
    AddSupportFor("Misc Items", (mod) => mod.MiscItems);
    AddSupportFor("Moveable Statics", (mod) => mod.MoveableStatics);
    AddSupportFor("Projectiles", (mod) => mod.Projectiles);
    AddSupportFor("Scrolls", (mod) => mod.Scrolls);
    AddSupportFor("Soul Gems", (mod) => mod.SoulGems);
    AddSupportFor("Statics", (mod) => mod.Statics);
    AddSupportFor("Texture Sets", (mod) => mod.TextureSets);
    AddSupportFor("Trees", (mod) => mod.Trees);
    AddSupportFor("Waters", (mod) => mod.Waters);
    AddSupportFor("Weapons", (mod) => mod.Weapons);

    return true;
  }

  private void DisplayRecord(TreeNode node)
  {
    try
    {
      var form = node.Tag as ISkyrimMajorRecordGetter;
      if (form is null)
      {
        _statusService.UpdateStatus("Invalid record");
        return;
      }
      _statusService.UpdateStatus($"Loaded record {form.EditorID ?? form.FormKey.IDString()}");
      objectListView1.ContextMenuStrip = new()
      {
        Items =
        {
          new ToolStripMenuItem("New material", null, (_, _) => { }),
        }
      };
      if (!_editedMaterials.ContainsKey(form.FormKey.ToString()))
      {
        objectListView1.SetObjects(new string[] { "No materials found" });
        return;
      }
      
    }
    catch (Exception e)
    {
      _statusService.UpdateStatus("Invalid record");
    }
  }

  private void OnLoad(object sender, EventArgs e)
  {
    if (!LoadPluginFiles())
    {
      return;
    }
  }

  private void OnChangeDirectoryClicked(object sender, EventArgs e)
  {
    var folderBrowser = new FolderBrowserDialog
    {
      Description = "Select data directory",
      UseDescriptionForTitle = true,
      SelectedPath = _workingDirectory
    };
    if (folderBrowser.ShowDialog() != DialogResult.OK) return;
    _workingDirectory = folderBrowser.SelectedPath;
    statusLabel.Text = _workingDirectory;
    LoadPluginFiles();
  }
}