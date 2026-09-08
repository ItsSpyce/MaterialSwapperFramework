#pragma once

#include <RmlUi/Core.h>

#include "STL.h"

namespace UI {
using namespace Rml;

class Window;
static inline event<Window*> WindowShown;
static inline event<Window*> WindowHidden;
static inline event<Window*> WindowLayoutChanged;

enum WindowFlags : u8 {
  WindowFlags_None = 0,
  WindowFlags_NoResize = 1 << 0,
  WindowFlags_NoMove = 1 << 1,
  WindowFlags_AlwaysVisible = 1 << 2,
  WindowFlags_ShowTitlebar = 1 << 3,
  WindowFlags_CanMinimize = 1 << 4,
  WindowFlags_CanMaximize = 1 << 5,
  WindowFlags_ClickThrough = 1 << 6,

  WindowFlags_ALL = UINT8_MAX,
};

struct WindowOpts {
  Vector2f size, pos;
  WindowFlags flags;
};

class _NODISCARD Window {
 public:
  Window(const char* name, const Vector2f& size, const Vector2f& pos,
         const WindowFlags flags)
      : name_(name), size_(size), pos_(pos), flags_(flags) {}

  unsigned GetID() const { return id_; }

  void Show() {
    // don't check for window flag here
    if (!open_) {
      open_ = true;
      WindowShown(this);
    }
  }

  void Hide() {
    if (open_ && !HasFlag(WindowFlags_AlwaysVisible)) {
      open_ = false;
      WindowHidden(this);
    }
  }

  bool IsOpen() const {
    return (flags_ & WindowFlags_AlwaysVisible != 0) || open_;
  }

  void SetSize(const Vector2f& size) {
    if (!HasFlag(WindowFlags_NoResize)) {
      size_ = size;
      WindowLayoutChanged(this);
    }
  }

  void SetPosition(const Vector2f& pos) {
    if (!HasFlag(WindowFlags_NoMove)) {
      pos_ = pos;
      WindowLayoutChanged(this);
    }
  }

  const Vector2f& GetSize() const { return size_; }

  const Vector2f& GetPos() const { return pos_; }

  bool HasFlag(const WindowFlags flags) const { return (flags_ & flags) != 0; }

  bool operator==(Window* other) const { return id_ == other->id_; }
  bool operator==(const Window* other) const { return id_ == other->id_; }
  bool operator!=(Window* other) const { return id_ != other->id_; }
  bool operator!=(const Window* other) const { return id_ != other->id_; }

 private:
  static inline unsigned ID = 0;
  unsigned id_{++ID};
  const char* name_;
  Vector2f size_, pos_;
  WindowFlags flags_;
  bool open_{false};
};

void Install();
Window* NewWindow(const char* name, const char* url, const WindowOpts& opts);
}  // namespace UI