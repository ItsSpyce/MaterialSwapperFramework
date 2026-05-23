#pragma once

template <typename T>
class Result {
  Result(const optional<T>& value, const optional<string>& error) : value_(value), error_(error) {}

public:

  T& expect(const string& error) const {
    if (error_.has_value()) {
      throw runtime_error(error);
    }
    return value_.value();
  }

  T* unwrap() const {
    if (value_.has_value()) {
      return &value_->value();
    }
    return nullptr;
  }

  string unwrap_error() const {
    if (error_.has_value()) {
      return *error_;
    }
    return string{};
  }

private:
  optional<T> value_;
  optional<string> error_;
};

template <typename T>
struct Ok {
  friend class Result<T>;
  Result<T> operator()(const T& value) {
    return Result(value, nullopt);
  }
}; 

template <typename T>
struct Err {
  friend class Result<T>;
  Result<T> operator()(const string& err) {
    return Result<T>(nullopt, err);
  }
};