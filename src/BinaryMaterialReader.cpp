#include "BinaryMaterialReader.h"

#include "IO/MaterialStream.h"

void BinaryMaterialReader::read(IStreamPtr&& stream) {
  MaterialStream m_stream(std::move(stream));

  std::array<char, 4> signature;
  m_stream.read_array(signature);
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
  file_->Read(m_stream);
}