#pragma once

namespace Core {
// idk why, lowercase function names look nicer here. It's a vibe
template <typename T, size_t N>
class RollingList {
  using value_type = T;
  using ptr_type = T*;
  using ref_type = T&;
  using const_ref_type = const T&;
  using size_type = size_t;
  static_assert(N > 0, "capacity must be greater than 0");

  class iter {
  public:
    using category = std::random_access_iterator_tag;
    using dif_type = ptrdiff_t;

    iter() = default;
    iter(RollingList* list, size_type index) : list_(list), index_(index) {}

    ref_type operator*() const {
      return (*list_)[index_];
    }

    ptr_type operator->() const {
      return &(*list_)[index_];
    }

    iter& operator++() {
      ++index_;
      return *this;
    }

    iter operator++(int) {
      iter temp = *this;
      ++(*this);
      return temp;
    }

    iter& operator--() {
      --index_;
      return *this;
    }

    iter operator--(int) {
      iter temp = *this;
      --(*this);
      return temp;
    }

    iter& operator+=(dif_type offset) {
      index_ += offset;
      return *this;
    }

    iter& operator-=(dif_type offset) {
      index_ -= offset;
      return *this;
    }

    iter operator+(dif_type offset) const {
      iter temp = *this;
      temp += offset;
      return temp;
    }

    iter operator-(dif_type offset) const {
      iter temp = *this;
      temp -= offset;
      return temp;
    }

    dif_type operator-(const iter& other) const {
      return static_cast<dif_type>(index_) - static_cast<dif_type>(other.index_);
    }

    ref_type operator[](dif_type offset) const {
      return *(*this + offset);
    }

    bool operator==(const iter& other) const {
      return list_ == other.list_ && index_ == other.index_;
    }

    bool operator!=(const iter& other) const {
      return list_ != other.list_ || index_ != other.index_;
    }

    bool operator<(const iter& other) const {
      return index_ < other.index_;
    }

    bool operator>(const iter& other) const {
      return index_ > other.index_;
    }

    bool operator<=(const iter& other) const {
      return index_ <= other.index_;
    }

    bool operator>=(const iter& other) const {
      return index_ >= other.index_;
    }
    
  private:
    RollingList* list_{};
    size_type index_ = 0;
  };

  class const_iter {
  public:
    using category = std::random_access_iterator_tag;
    using dif_type = ptrdiff_t;

    const_iter() = default;
    const_iter(RollingList* list, size_type index) : list_(list), index_(index) {}
    const_iter(iter it) : list_(it.list_), index_(it.index_) {}

    ref_type operator*() const {
      return (*list_)[index_];
    }

    ptr_type operator->() const {
      return &(*list_)[index_];
    }

    const_iter& operator++() {
      ++index_;
      return *this;
    }

    const_iter operator++(int) {
      const_iter temp = *this;
      ++(*this);
      return temp;
    }

    const_iter& operator--() {
      --index_;
      return *this;
    }

    const_iter operator--(int) {
      const_iter temp = *this;
      --(*this);
      return temp;
    }

    const_iter& operator+=(dif_type offset) {
      index_ += offset;
      return *this;
    }

    const_iter& operator-=(dif_type offset) {
      index_ -= offset;
      return *this;
    }

    const_iter operator+(dif_type offset) const {
      const_iter temp = *this;
      temp += offset;
      return temp;
    }

    const_iter operator-(dif_type offset) const {
      const_iter temp = *this;
      temp -= offset;
      return temp;
    }

    dif_type operator-(const const_iter& other) const {
      return static_cast<dif_type>(index_) - static_cast<dif_type>(other.index_);
    }

    ref_type operator[](dif_type offset) const {
      return *(*this + offset);
    }

    bool operator==(const const_iter& other) const {
      return list_ == other.list_ && index_ == other.index_;
    }

    bool operator!=(const const_iter& other) const {
      return list_ != other.list_ || index_ != other.index_;
    }

    bool operator<(const const_iter& other) const {
      return index_ < other.index_;
    }

    bool operator>(const const_iter& other) const {
      return index_ > other.index_;
    }

    bool operator<=(const const_iter& other) const {
      return index_ <= other.index_;
    }

    bool operator>=(const const_iter& other) const {
      return index_ >= other.index_;
    }
    
  private:
    const RollingList* list_{};
    size_type index_ = 0;
  };

public:

  RollingList() = default;
  ~RollingList() {
    clear();
  }

  NODISCARD constexpr size_type size() const noexcept { return size_; }
  NODISCARD constexpr size_type capacity() const noexcept { return N; }
  NODISCARD constexpr bool empty() const noexcept { return size_ == 0; }

  void clear() noexcept {
    position_ = 0;
    size_ = 0;
  }

  ref_type operator[](size_type index) {
    return contents_[physical_index(index)];
  }

  const_ref_type operator[](size_type index) const {
    return contents_[physical_index(index)];
  }

  ref_type at(size_type index) {
    if (index >= size_) {
      throw std::out_of_range("index out of range");
    }
    return (*this)[index];
  }

  const_ref_type at(size_type index) const {
    if (index >= size_) {
      throw std::out_of_range("index out of range");
    }
    return (*this)[index];
  }

  ref_type front() {
    return at(0);
  }

  const_ref_type front() const {
    return at(0);
  }

  ref_type back() {
    return at(size_ - 1);
  }

  const_ref_type back() const {
    return at(size_ - 1);
  }

  void push_back(const T& value) {
    if (size_ == N) {
      contents_[position_] = value;
      position_ = (position_ + 1) % N;
    } else {
      contents_[physical_index(size_)] = value;
      ++size_;
    }
  }

  void push_back(T&& value) {
    if (size_ == N) {
      contents_[position_] = move(value);
      position_ = (position_ + 1) % N;
    } else {
      contents_[physical_index(size_)] = move(value);
      ++size_;
    }
  }

  template <typename... Args>
  ref_type emplace_back(Args&&... args) {
    if (size_ == N) {
      contents_[position_] = T(forward<Args>(args)...);
      auto i = position_;
      position_ = (position_ + 1) % N;
      return contents_[i];
    } else {
      auto i = physical_index(size_);
      contents_[i] = T(forward<Args>(args)...);
      ++size_;
      return contents_[i];
    }
  }

  iter begin() noexcept {
    return iter(this, 0);
  }

  iter end() noexcept {
    return iter(this, size_);
  }

  const_iter cbegin() const noexcept {
    return const_iter(this, 0);
  }

  const_iter cend() const noexcept {
    return const_iter(this, size_);
  }

private:
  std::array<T, N> contents_;
  size_type size_ = 0, position_ = 0;

  size_type physical_index(size_type index) const noexcept {
    return (position_ + index) % N;
  }
};
}