#pragma once

#include "Models/MaterialFileBase.h"

class BinaryMaterialReader {
 public:
  void read(IStreamPtr&& stream);

  std::shared_ptr<MaterialFileBase> file() { return std::move(file_); }

 private:
  std::shared_ptr<MaterialFileBase> file_;
};