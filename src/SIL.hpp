#pragma once

namespace SIL {
// versioned database for SKSE plugins
template <typename Type>
class Column {
 public:
  Column() = default;
  explicit Column(const Type& value) : value_(value), has_value_(true) {}
  bool has_value() const { return has_value_; }
  const Type& value() const { return value_; }
  void set_value(const Type& value) {
    value_ = value;
    has_value_ = true;
  }

 private:
  Type value_;
  bool has_value_ = false;
};

class Database {
  const char* get_database_filename() {
    return "";
  }
 public:
  static Database* GetSingleton() {
    static Database singleton;
    return &singleton;
  }
  void SetVersion(int version) { version_ = version; }
  int GetVersion() const { return version_; }

 private:
  Database() : version_(0) {}
  ~Database() = default;
  int version_;
};
}  // namespace SIL