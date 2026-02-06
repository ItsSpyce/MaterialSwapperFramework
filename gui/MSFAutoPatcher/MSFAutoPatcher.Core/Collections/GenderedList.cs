using Mutagen.Bethesda.Plugins.Records;

namespace MSFAutoPatcher.Core.Collections;
public class GenderedList<T>
{
  private List<T> _maleItems = [];
  private List<T> _femaleItems = [];

  public int Count => _maleItems.Count + _femaleItems.Count;

  public IEnumerable<T> GetMaleEnumerable() => _maleItems;
  public IEnumerable<T> GetFemaleEnumerable() => _femaleItems;
  public IEnumerable<T> GetEnumerable(MaleFemaleGender gender) => gender switch
  {
    MaleFemaleGender.Female => GetFemaleEnumerable(),
    MaleFemaleGender.Male => GetMaleEnumerable(),
    _ => throw new NotImplementedException(),
  };

  public void AddMaleItem(T item) => _maleItems.Add(item);
  public void AddFemaleItem(T item) => _femaleItems.Add(item);

  public void AddItem(MaleFemaleGender gender, T item)
  {
    switch (gender)
    {
      case MaleFemaleGender.Male:
        _maleItems.Add(item);
        break;
      case MaleFemaleGender.Female:
        _femaleItems.Add(item);
        break;
      default:
        throw new ArgumentOutOfRangeException(nameof(gender), gender, null);
    }
  }

  public bool HasMaleItems() => _maleItems.Count > 0;
  public bool HasFemaleItems() => _femaleItems.Count > 0;

  public int MaleItemCount() => _maleItems.Count;
  public int FemaleItemCount() => _femaleItems.Count();

  public void Clear()
  {
    _maleItems.Clear();
    _femaleItems.Clear();
  }

  public void RemoveMaleItem(T item) => _maleItems.Remove(item);
  public void RemoveFemaleItem(T item) => _femaleItems.Remove(item);

  public void RemoveItem(T item)
  {
    _maleItems.Remove(item);
    _femaleItems.Remove(item);
  }
}
