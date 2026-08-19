#pragma once

#include <glaze/glaze.hpp>

#include "Filesystem.h"

class Options : public Singleton<Options> {
  using level = spdlog::level::level_enum;
  struct OptionsInternal {
    std::string logLevel = "info";
    i32 applyMaterialTickDelay = 1;
    u32 openWindowKey = 68;
    u32 maxCoroutineOpsPerFrame = 500;
    bool enableCaching = false;
  };

 public:
  Options() = default;

  void ReadFromDisk() {
    auto path = fs::path(Filesystem::SKSE_PLUGINS_DIR) /
                "MaterialSwapperFramework.json";
    OptionsInternal options;
    if (auto err = glz::read_file_json<glz::opts{.comments = true}>(
            options, path.string(), std::string{})) {
      auto cleanedErr = glz::format_error(err);
      stl::report_and_fail(cleanedErr);
    }
    if (options.logLevel == "trace") {
      logLevel_ = level::trace;
    } else if (options.logLevel == "debug") {
      logLevel_ = level::debug;
    } else if (options.logLevel == "info") {
      logLevel_ = level::info;
    } else if (options.logLevel == "warn") {
      logLevel_ = level::warn;
    } else if (options.logLevel == "error") {
      logLevel_ = level::err;
    } else if (options.logLevel == "fatal") {
      logLevel_ = level::critical;
    } else if (options.logLevel == "off") {
      logLevel_ = level::off;
    } else {
      stl::report_and_fail("Invalid log level in options: " + options.logLevel);
    }
    applyMaterialTickDelay_ = options.applyMaterialTickDelay;
    openWindowKey_ = options.openWindowKey;
    maxCoroutineOpsPerFrame_ = std::max(10u, options.maxCoroutineOpsPerFrame);
    enableCaching_ = options.enableCaching;
  }

  NODISCARD level GetLogLevel() const { return logLevel_; }
  NODISCARD i32 GetApplyMaterialTickDelay() const {
    if (applyMaterialTickDelay_ == 0) {
      stl::report_and_fail("ApplyMaterialTickDelay must be greater than 0");
    }
    return applyMaterialTickDelay_;
  }
  NODISCARD u32 GetOpenWindowKey() const {
    if (openWindowKey_ == 0) {
      stl::report_and_fail("OpenWindowKey not set in ini");
    }
    return openWindowKey_;
  }
  NODISCARD u32 GetMaxCoroutineOpsPerFrame() const {
    return maxCoroutineOpsPerFrame_;
  }
  NODISCARD bool EnableCaching() const {
    return enableCaching_;
  }

 private:
  level logLevel_;
  i32 applyMaterialTickDelay_;
  u32 openWindowKey_;
  u32 maxCoroutineOpsPerFrame_;
  bool enableCaching_;
};