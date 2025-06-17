#pragma once

#include "ShaderMaterialFile.h"

class MaterialFileReader {
 public:
  void read(IStreamPtr&& stream);

  std::shared_ptr<ShaderMaterialFile> file() { return std::move(file_); }

 private:
  std::shared_ptr<ShaderMaterialFile> file_;
};