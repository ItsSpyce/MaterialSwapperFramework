using System.Collections.ObjectModel;
using ReactiveUI;

namespace MaterialSwapperFramework.Gui.ViewModels;

public class ViewModelBase : ReactiveObject
{
  public ViewModelBase()
  {
    Configure();
  }

  protected void SubscribeToCollectionUpdates<T>(ref ObservableCollection<T> collection, string name) where T : class
  {
    collection.CollectionChanged += (_, _) => this.RaisePropertyChanged(name);
  }

  protected virtual void Configure()
  {

  }
}
