#pragma once

#include <srell.hpp>

#include "StringHelpers.h"

namespace RE {
class BSTextureSetClone {
 public:
  explicit BSTextureSetClone(NiPointer<BSTextureSet> original) {
    clone_ = BSShaderTextureSet::Create();
    if (original) {
      for (int i = 0; i < BSShaderTextureSet::Texture::kTotal; i++) {
        auto texture = static_cast<BSShaderTextureSet::Texture>(i);
        auto texturePath = original->GetTexturePath(texture);
        if (!texturePath) continue;
        clone_->SetTexturePath(texture, texturePath);
      }
    }
    original.reset();
  }

  ~BSTextureSetClone() {
    if (clone_) {
      clone_->~BSTextureSet();
      free(clone_);
    }
  }

  void SetTexturePath(const BSShaderTextureSet::Texture type,
                      const optional<string>& path) const {
    if (path.has_value() && !path->empty()) {
      auto value = string(path.value());
      SanitizePath(value);
      clone_->SetTexturePath(type, value.c_str());
    }
  }

  void SetTexture(const RE::BSShaderTextureSet::Texture type,
                  NiSourceTexturePtr srcTexture) const {
    clone_->SetTexture(type, srcTexture);
  }

  NiPointer<BSTextureSet> Get() const { return NiPointer(clone_); }
  operator BSTextureSet*() const { return clone_; }

 private:
  BSTextureSet* clone_;

  static inline void SanitizePath(string& in) {
    auto result = StringHelpers::ToLower(in);
    result = srell::regex_replace(result, srell::regex("/+|\\\\+"), "\\");
    result = srell::regex_replace(result, srell::regex("^\\\\+"), "");
    result = srell::regex_replace(
        result,
        srell::regex(R"(.*?[^\s]textures\\|^textures\\)", srell::regex::icase),
        "");
  }
};
}  // namespace RE