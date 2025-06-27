#include "JsonMaterialReader.h"

#include <glaze/glaze.hpp>

void IO::JsonMaterialReader::Read(const std::string& filename) {
  if (filename.empty() || !std::filesystem::exists(filename)) {
    logger::error("Material file does not exist: {}", filename);
    return;
  }
  logger::debug("Reading JSON material file: {}", filename);
  glz::json_t json;
  std::string buffer{};
  // this is setting json to null and idk why
  if (auto err = glz::read_file_json(json, filename, buffer)) {
    auto cleanedError = glz::format_error(err);
    throw std::runtime_error(
        fmt::format("Failed to read JSON material file: {}", cleanedError));
  }
  logger::debug("JSON: {}", buffer.c_str());
  if (json["shaderType"].get_number() == 0) {
    BGSMFile file;
    file.version = json["version"].get_number();
    file.diffuseMap = json["diffuseMap"].get_string();
    file.normalMap = json["normalMap"].get_string();
    file.smoothSpecMap = json["smoothSpecMap"].get_string();
    file.grayscaleMap = json["grayscaleMap"].get_string();
    file.glowMap = json["glowMap"].get_string();
    file.wrinkleMap = json["wrinkleMap"].get_string();
    file.specularMap = json["specularMap"].get_string();
    file.lightingMap = json["lightingMap"].get_string();
    file.envMap = json["envMap"].get_string();
    file_ = std::make_shared<BGSMFile>(std::move(file));
  } else {
    BGEMFile file;
    file.version = json["version"].get_number();
    // TODO:
    file_ = std::make_shared<BGEMFile>(std::move(file));
  }
}