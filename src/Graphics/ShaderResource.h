#pragma once

#include "ShaderFactory.h"

namespace Graphics {
class ShaderResource : public ShaderFile {
 public:
  ShaderResource(const char* filename, const char* entryPoint);

  virtual void* GetBuffer() const;
	virtual size_t GetBufferSize() const;
	virtual const char* GetSourceName() const;
	virtual const char* GetEntryPoint() const;

 private:
  string entryPoint_;
  string filename_;
  vector<char> buffer_;
};
}  // namespace Graphics