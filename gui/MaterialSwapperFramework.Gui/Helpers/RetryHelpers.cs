using System;
using System.Threading;
using System.Threading.Tasks;

namespace MaterialSwapperFramework.Gui.Helpers;

public static class RetryHelpers
{
  public static void Retry(Action action, int times, int waitTime = 1000)
  {
    var count = 0;
    while (count < times)
    {
      try
      {
        action();
        return; // Exit if action succeeds
      }
      catch (Exception ex)
      {
        count++;
        if (count >= times)
        {
          throw new($"Action failed after {times} attempts.", ex);
        }
        Thread.Sleep(waitTime); // Wait before retrying
      }
    }
  }

  public static async Task RetryAsync(Func<Task> action, int times, int waitTime = 1000)
  {
    var count = 0;
    while (count < times)
    {
      try
      {
        await action();
        return; // Exit if action succeeds
      }
      catch (Exception ex)
      {
        count++;
        if (count >= times)
        {
          throw new($"Action failed after {times} attempts.", ex);
        }

        await Task.Delay(waitTime); // Wait before retrying
      }
    }
  }
}