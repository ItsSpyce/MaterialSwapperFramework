#pragma once

namespace RE {
class BSTextureSetClone {
 public:
  explicit BSTextureSetClone(NiPointer<BSTextureSet> original) {
    clone_ = BSShaderTextureSet::Create();
    if (original) {
      for (int i = 0; i < BSShaderTextureSet::Texture::kTotal; i++) {
        const char* path = original->GetTexturePath(
            static_cast<BSShaderTextureSet::Texture>(i));
        if (path && path[0] != '\0') {
          clone_->SetTexturePath(static_cast<BSShaderTextureSet::Texture>(i),
                                path);
        }
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

  void SetTexturePath(const BSShaderTextureSet::Texture type, const optional<string>& path) const {
    if (path.has_value() && !path->empty()) {
      clone_->SetTexturePath(type, path->c_str());
    }
  }

  NiPointer<BSTextureSet> Get() const { return NiPointer(clone_); }
  operator BSTextureSet*() const { return clone_; }

 private:
  BSTextureSet* clone_;
};
}  // namespace RE