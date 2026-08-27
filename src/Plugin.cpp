#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <spdlog/sinks/basic_file_sink.h>

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

SKEE64::IInterfaceMap* g_skee64InterfaceMap;

static void InitializeLogging() {
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true;
  auto path = logger::log_directory();
  if (!path) {
    stl::report_and_fail("Failed to retrieve log directory"sv);
  }
  *path /= "MaterialSwapperFramework.log";
  if (std::filesystem::exists(*path)) {
    std::ofstream ofs(path->string(), std::ios::trunc);
    if (!ofs) {
      stl::report_and_fail("Failed to clear log file"sv);
    }
    ofs.close();
  }
  std::shared_ptr<spdlog::sinks::sink> sink;
  if (IsDebuggerPresent()) {
    sink = std::make_shared<spdlog::sinks::msvc_sink_st>();
  } else {
    sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string());
  }
  auto logger =
      std::make_shared<spdlog::logger>("MaterialSwapperFramework", sink);
  logger->set_level(Options::GetSingleton()->GetLogLevel());
  logger->flush_on(Options::GetSingleton()->GetLogLevel());
  spdlog::set_default_logger(std::move(logger));
  spdlog::set_pattern("%^[%T] %l: %v%$");
}

static void HandleMessage(SKSE::MessagingInterface::Message* msg) {
  if (msg->type == SKSE::MessagingInterface::kDataLoaded) {
    // CoroutineManager::GetSingleton()->Initialize();
    _INFO("Querying RaceMenu...");
    if (!SKEE64Instance::GetSingleton()->Initialize()) {
      stl::report_and_fail(
          "Failed to query RaceMenu. Ensure it's installed before continuing.");
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
  InitializeLogging();
  Options::GetSingleton()->ReadFromDisk();

  SKSE::GetMessagingInterface()->RegisterListener(HandleMessage);
  _INFO("Registering UI hooks...");
  UI::Install();
  _INFO("Installing hooks...");
  Hooks::Install();
  _INFO("Registering Papyrus functions...");
  SKSE::GetPapyrusInterface()->Register(MaterialPapyrus::RegisterFunctions);
  _INFO("Registering save hooks...");
  Save::Install();

  return true;
}
