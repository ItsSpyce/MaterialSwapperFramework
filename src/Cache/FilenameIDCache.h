#pragma once

#include <emhash/hash_table5.hpp>
#include <emhash/hash_table8.hpp>

#include "Macros.h"
#include "Result.h"
#include "Types.h"

class FilenameIDCache {
public:
  /// <summary>
  ///   Saves the path into the cache.
  /// </summary>
  /// <param name="filename"></param>
  /// <returns>TextureID</returns>
  _NODISCARD static fn GetFilenameID(const std::string& filename) -> FileID {
    // but spyce, why would we immediately increment! 0 is valid!
    // no it's not. 0 = NULL.

    // TODO: persist the filename to Filesystem:FILE_ID_BIN
    const auto expected = lastID_ + 1;
    const auto result = filenames_.set_get(filename, expected);
    if (result == expected) {
      fileIDs_.emplace(result, filename);
      lastID_ = expected;
    }
    return result;
  }

  _NODISCARD static fn GetPathForID(const FileID id) -> result<std::string> {
    if (id == NULL) return Err<std::string>("Received an empty form ID");
    SCOPE_GUARD(lock_);
    FIND_IN(fileIDs_, it, id) {
      return Ok(it->second);
    }
    return Err<std::string>("Texture lookup not found for ID {}", id);
  }

  static fn Clear() {
    SCOPE_GUARD(lock_);
    fileIDs_.clear();
    filenames_.clear();
    lastID_ = 0;
  }
private:
  static inline FileID lastID_ = 0;
  static inline std::mutex lock_;
  static inline emhash5::HashMap<FileID, std::string> fileIDs_;
  static inline emhash8::HashMap<std::string, FileID> filenames_;
};