#pragma once

#include <emhash/hash_table5.hpp>
#include <emhash/hash_table8.hpp>

#include "Macros.h"
#include "Types.h"

class FilenameIDCache {
  static inline std::mutex lock_;
  static inline emhash5::HashMap<FileID, std::string> fileIDs_;
  static inline emhash8::HashMap<std::string, FileID> filenames_;

 public:
  /// <summary>
  ///   Saves the path into the cache.
  /// </summary>
  /// <param name="filename"></param>
  /// <returns>TextureID</returns>
  _NODISCARD static FileID GetFilenameID(const std::string& filename) {
    // but spyce, why would we immediately increment! 0 is valid!
    // no it's not. 0 = NULL.
    auto [fileID, didEmplace] =
        filenames_.try_emplace(filename, filenames_.size() + 1);
    return fileID->second;
  }

  _NODISCARD static result<std::string> GetPathForID(const FileID id) {
    if (id == NULL) return Err{"Received NULL for file ID"};
    SCOPE_GUARD(lock_);
    FIND_IN(fileIDs_, it, id) { return Ok{it->second}; }
    return Err{"Failed to find file ID {}", id};
  }

  static void Clear() {
    SCOPE_GUARD(lock_);
    fileIDs_.clear();
    filenames_.clear();
  }
};