#pragma once

#include "ShaderFactory.h"

namespace Graphics {
class ShaderResource : public ShaderFile {
 public:
  ShaderResource(const char* filename, const char* entryPoint)
      : filename_(filename), entryPoint_(entryPoint) {}

 private:
  string entryPoint_;
  string filename_;
  vector<char> buffer_;
};
}  // namespace Graphics