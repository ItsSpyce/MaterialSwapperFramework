#pragma once

#include "Models/MaterialFileBase.h"

namespace IO {
class MaterialReaderBase {
 public:
  virtual ~MaterialReaderBase() = default;
  virtual void Read(const std::string& filename) = 0;
  std::shared_ptr<MaterialFileBase> File() { return std::move(file_); }

 protected:
  std::shared_ptr<MaterialFileBase> file_;
};
}  // namespace IO