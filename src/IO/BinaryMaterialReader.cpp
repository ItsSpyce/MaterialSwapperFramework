#include "BinaryMaterialReader.h"

#include "MaterialStream.h"

void IO::BinaryMaterialReader::Read(const std::string& filename) {
  auto stream = std::make_unique<std::ifstream>(filename, std::ios::binary);
  if (!stream->is_open()) {
    throw std::runtime_error(fmt::format("Failed to open file: {}", filename));
  }
  stream->exceptions(std::ios::badbit);
  MaterialStream mStream(std::move(stream));

  std::array<char, 4> signature;
  mStream.ReadArray(signature);
  std::string shader_type(signature.data(), 4);
  if (shader_type == "BGEM") {
    file_ = std::make_shared<BGEMFile>();
    file_->shaderType = ShaderType::kEffect;
  } else if (shader_type == "BGSM") {
    file_ = std::make_shared<BGSMFile>();
    file_->shaderType = ShaderType::kLighting;
  } else {
    throw std::runtime_error("Invalid material file");
  }
  file_->Read(mStream);
}