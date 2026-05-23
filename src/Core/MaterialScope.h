#pragma once

#include "Models/MaterialConfig.h"
#include "Models/MaterialRecord.h"

class MaterialScope {
public:
  explicit MaterialScope(const MaterialRecord* record) : config_(nullptr), record_(record) {
    scope_ = this;
  }
  explicit MaterialScope(const MaterialConfig* config) : config_(config), record_(nullptr) {
    scope_ = this;
  }
  explicit MaterialScope(const MaterialConfig* config, const MaterialRecord* record) : config_(config), record_(record) {
    scope_ = this;
  }
  ~MaterialScope() {
    scope_ = nullptr;
  }

  NOMOVE(MaterialScope);
  NOCOPY(MaterialScope);

  static const MaterialConfig* GetCurrentConfig() {
    return scope_ ? scope_->config_ : nullptr;
  }

  static const MaterialRecord* GetCurrentRecord() {
    return scope_ ? scope_->record_ : nullptr;
  }

private:
  static inline MaterialScope* scope_{};

  const MaterialConfig* config_;
  const MaterialRecord* record_;
};