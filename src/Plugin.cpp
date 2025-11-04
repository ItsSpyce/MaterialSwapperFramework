#include "Events.h"
#include "Hooks.h"
#include "IO/MaterialLoader.h"
#include "MaterialPapyrus.h"
#include "ModState.h"
#include "TaskManager.h"
#include "ThreadPool.h"
#include "Translations.h"
#include "UI.h"

EventSource<FrameEvent> g_frameEventSource;
EventSource<ArmorAttachEvent> g_armorAttachSource;
EventSource<PlayerCellChangeEvent> g_cellChangeSource;

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
  logger->set_level(spdlog::level::trace);
  logger->flush_on(spdlog::level::trace);
  spdlog::set_default_logger(std::move(logger));
  spdlog::set_pattern("%^[%T] %l: %v%$");
}

static void HandleMessage(SKSE::MessagingInterface::Message* msg) {
  if (msg->type == SKSE::MessagingInterface::kPostLoad) {
    UI::Initialize();
    Events::Configure();
  }
  if (msg->type == SKSE::MessagingInterface::kDataLoaded) {
    _INFO("Reading form editor IDs from plugins...");
    EditorIDCache::HydrateEditorIDCache();
    _INFO("Reading materials from disk...");
    MaterialLoader::ReadMaterialsFromDisk(true);
    ModState::GetSingleton()->SetReady(true);
  }
  if (msg->type == SKSE::MessagingInterface::kPostLoadGame) {
    TaskManager::GetSingleton()->Initialize();
  }
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
  SKSE::Init(a_skse);
  InitializeLogging();
  TranslationEX::UsePluginName("MSF");

  SKSE::GetMessagingInterface()->RegisterListener(HandleMessage);
  _INFO("Registering UI hooks...");
  UI::Hooks::Install();
  _INFO("Installing hooks...");
  Hooks::Install();
  auto* taskManager = TaskManager::GetSingleton();
  g_armorAttachSource.AddListener(taskManager);
  g_frameEventSource.AddListener(taskManager);
  g_cellChangeSource.AddListener(taskManager);
  _INFO("Registering Papyrus functions...");
  SKSE::GetPapyrusInterface()->Register(MaterialPapyrus::RegisterFunctions);
  _INFO("Registering save hooks...");
  Save::Install();

  return true;
}
