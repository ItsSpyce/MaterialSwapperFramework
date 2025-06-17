#pragma once

class ShaderMaterialFile;

class MaterialCache {
 public:
  static void Initialize();
  static std::shared_ptr<ShaderMaterialFile> GetMaterial(int32_t uid);
  static void RemoveFromCache(int32_t uid) { cacheMap_.erase(uid); }
  static void AddToCache(int32_t uid, int32_t materialHash) {
    cacheMap_[uid] = materialHash;
  }

 private:
  struct MaterialCacheEntry {
    int32_t uid;
    int32_t materialHash;
  };
  static void SaveCallback(SKSE::SerializationInterface* serialization);
  static void LoadCallback(SKSE::SerializationInterface* serialization);

  static inline std::unordered_map<int32_t, int32_t> cacheMap_{};
};