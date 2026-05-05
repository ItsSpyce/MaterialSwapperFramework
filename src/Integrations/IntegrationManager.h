#pragma once

namespace Integrations {
class Integration;

class IntegrationManager : public Singleton<IntegrationManager> {
public:
  IntegrationManager() = default;

  NOMOVE(IntegrationManager);
  NOCOPY(IntegrationManager);

  void Configure();
};
}