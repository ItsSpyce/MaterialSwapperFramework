using System.Threading.Tasks;
using MaterialSwapperFramework.Gui.Models;

namespace MaterialSwapperFramework.Gui.Services;

public interface IProjectService
{
  Task<MsfProj> LoadProjectAsync(string filename);
  Task SaveProjectAsync(string filename, MsfProj proj);
}

public class ProjectService : IProjectService
{
  public Task<MsfProj> LoadProjectAsync(string filename)
  {
    throw new System.NotImplementedException();
  }

  public Task SaveProjectAsync(string filename, MsfProj proj)
  {
    throw new System.NotImplementedException();
  }
}