#pragma once

template <typename Type = bool>
class Result {
  Result() = default;
 public:

  static Result<Type> Ok(Type value) {
    Result result;
    result.value_ = std::make_shared<Type>(std::move(value));
    return result;
  }

  template <class ...Args>
  static Result<Type> Err(fmt::format_string<Args...> fmt, Args&&... args) {
    Result result;
    result.error_ = fmt::format(fmt, std::forward<Args>(args)...);
    return result;
  }

  explicit operator bool() const { return error_.empty(); }

  Type* operator->() { return value_.get(); }
  const Type* operator->() const { return value_.get(); }
  Type& operator*() { return *value_; }
  const Type& operator*() const { return *value_; }

  bool HasError() const { return !error_.empty(); }

  Type& Unwrap() {
    if (HasError()) {
      throw std::runtime_error("Called Unwrap on an Err result: " + error_);
    }
    return *value_;
  }

  std::string& UnwrapError() { return error_; }

  template <typename NewType = void>
  Result<NewType> Forward() {
    if (HasError()) {
      return Result<NewType>::Err(error_);
    }
    return Result<NewType>::Ok(NewType(*value_));
  }

 private:
  std::shared_ptr<Type> value_;
  std::string error_;
};

inline Result<> Ok() { return Result<>::Ok(true); }

template <typename Type = bool>
Result<Type> Ok(Type value) {
  return Result<Type>::Ok(value);
}

template <typename... Args>
Result<> Err(fmt::format_string<Args...> fmt, Args&&... args) {
  return Result<>::Err(fmt, std::forward<Args>(args)...);
}

template <typename Type, typename... Args>
Result<Type> Err(fmt::format_string<Args...> fmt, Args&&... args) {
  return Result<Type>::Err(fmt, std::forward<Args>(args)...);
}