#pragma once

namespace Core {
class StringReader {
public:
  explicit StringReader(const std::string& str) : str_(std::move(str)) {}

  fn Read(char* const buffer, size_t length) const {
    pos_ += length;
    str_.copy(buffer, pos_ - length, length);
  }

  fn Read(size_t length) const -> std::string {
    pos_ += length;
    return str_.substr(pos_ - length, length);
  }

  fn ReadUntil(char until) const -> std::string {
    const size_t start = pos_;
    while (pos_ < str_.length()) {
      if (str_[pos_] == until) {
        break;
      }
      pos_++;
    }
    return str_.substr(start, pos_ - start);
  }

  fn Skip(size_t count = 1) const {
    pos_ += count;
  }

  constexpr fn AtEnd() const -> bool {
    return pos_ >= str_.length();
  }
private:
  const std::string str_;
  mutable size_t pos_{0};
};
}