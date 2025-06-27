#pragma once

#include "MaterialReaderBase.h"

namespace IO {
class BinaryMaterialReader final : public MaterialReaderBase {
  void Read(const std::string& filename) override;
};
}  // namespace IO