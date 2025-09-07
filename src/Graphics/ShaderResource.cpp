#include "ShaderResource.h"

Graphics::ShaderResource::ShaderResource(const char* filename,
                                         const char* entryPoint)
    : entryPoint_(entryPoint), filename_(filename) {
  if (filename_.empty() || entryPoint_.empty()) {
    _ERROR("Invalid filename or entry point");
    return;
  }
  std::ifstream file(filename_, std::ios::binary);
  if (!file) {
    _ERROR("Failed to open shader file: {}", filename_);
    return;
  }
  file.seekg(0, std::ios::end);
  auto size = file.tellg();
  if (size <= 0) {
    _ERROR("Shader file is empty: {}", filename_);
    return;
  }
  file.seekg(0, std::ios::beg);
  buffer_.resize(static_cast<size_t>(size));
  file.read(buffer_.data(), size);
  if (!file) {
    _ERROR("Failed to read shader file: {}", filename_);
    buffer_.clear();
  }
}

void* Graphics::ShaderResource::GetBuffer() const {
  return buffer_.empty() ? nullptr : (void*)buffer_.at(0);
}

size_t Graphics::ShaderResource::GetBufferSize() const {
  return buffer_.size();
}

const char* Graphics::ShaderResource::GetSourceName() const {
  return filename_.c_str();
}

const char* Graphics::ShaderResource::GetEntryPoint() const {
  return entryPoint_.c_str();
}