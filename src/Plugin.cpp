#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Cache/Cache.h"

#include "CommunityShaders/CS.h"
#include "Hooks.h"
#include "MaterialPapyrus.h"
#include "ModState.h"
#include "Options.h"
#include "SKEE64.h"
#include "Save/Save.h"
#include "Translations.h"
#include "UI.h"
#include "spdlog/sinks/basic_file_sink.h"

SKEE64::IInterfaceMap* g_skee64InterfaceMap;

static void HandleMessage(SKSE::MessagingInterface::Message* msg) {
  if (msg->type == SKSE::MessagingInterface::kPostLoad) {
    UI::Initialize();
    UI::GetCurrentUI()->SetShowKey(Options::GetSingleton()->GetOpenWindowKey());
  }
  if (msg->type == SKSE::MessagingInterface::kDataLoaded) {
    // CoroutineManager::GetSingleton()->Initialize();
    _INFO("Querying RaceMenu...");
    if (!SKEE64Instance::GetSingleton()->Initialize()) {
      stl::report_and_fail("Failed to query RaceMenu. Ensure it's installed before continuing.");
    }
    _INFO("Reading form editor IDs from plugins...");
    EditorIDCache::HydrateEditorIDCache();
    _INFO("Reading materials from disk...");
    MaterialSwapper::ReadMaterialConfigurations();
    _INFO("Checking for ENB and CS...");
    if (CommunityShaders::IsInstalled()) {
      _INFO("CommunityShaders detected, enabling compatibility mode");
      ModState::GetSingleton()->SetCSInstalled(true);
    }
    ModState::GetSingleton()->SetReady(true);
  }
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
  SKSE::Init(a_skse);
  Options::GetSingleton()->ReadFromDisk();
  TranslationEX::UsePluginName("MSF");

  SKSE::GetMessagingInterface()->RegisterListener(HandleMessage);
  _INFO("Registering UI hooks...");
  UI::Hooks::Install();
  _INFO("Installing hooks...");
  Hooks::Install();
  _INFO("Registering Papyrus functions...");
  SKSE::GetPapyrusInterface()->Register(MaterialPapyrus::RegisterFunctions);
  _INFO("Registering save hooks...");
  Save::Install();

  return true;
}
