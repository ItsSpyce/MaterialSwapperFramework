#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

#include "MaterialLoader.h"
#include "UI.h"
#include "Hooks.h"
#include "Events.h"
#include "MaterialPapyrus.h"

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
    logger::info("Reading materials from disk...");
    MaterialLoader::ReadMaterialsFromDisk(true);
  }
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
  SKSE::Init(a_skse);
  InitializeLogging();

  SKSE::GetMessagingInterface()->RegisterListener(HandleMessage);
  UI::RegisterHooks();
  Hooks::Install();
  SKSE::GetPapyrusInterface()->Register(MaterialPapyrus::RegisterFunctions);

  return true;
}
