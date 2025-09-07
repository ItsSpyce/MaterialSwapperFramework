#pragma once
#include "Save/Save.h"

using namespace Save::V1;

class UniqueIDTable : public Singleton<UniqueIDTable>, public ISaveable {
  static constexpr u32 MAX_SAVE_LIFE = 500;  // seems reasonable enough?
  template <typename T, typename Container = std::deque<T> >
  class iterable_queue : public std::queue<T, Container> {
   public:
    typedef typename Container::const_iterator const_iterator;

    const_iterator begin() const { return this->c.begin(); }
    const_iterator end() const { return this->c.end(); }
  };

 public:
  void ReadFromSave(Save::SaveData& data) override {
    rows_.clear();
    for (const auto& row : data.uniqueIDHistory.rows) {
      row.savesSinceLastAccess++;
      if (row.savesSinceLastAccess > MAX_SAVE_LIFE) {
        _TRACE("Removing stale UID: {} for owner: {}, form: {}", row.uid,
               row.ownerID, row.editorID);
        freeList_.emplace(row.uid);
        continue;
      }
      rows_[row.uid] = row;
    }
    nextUID_ = data.uniqueIDHistory.nextUID;
  }

  void WriteToSave(Save::SaveData& data) const override {
    auto values = rows_ | views::values;
    data.uniqueIDHistory.rows = vector(values.begin(), values.end());
    data.uniqueIDHistory.nextUID = nextUID_;
    data.uniqueIDHistory.freedUIDs = vector(freeList_.begin(), freeList_.end());
  }

  size_t GetUID(RE::TESObjectREFR* refr, string& editorID, bool init) {
    if (!refr) {
      return 0;
    }
    if (editorID.empty()) {
      _ERROR("Empty editor ID provided for reference: {}", refr->GetFormID());
      return 0;
    }
    auto ownerID = refr->GetFormID();
    for (const auto& [uid, row] : rows_) {
      if (row.ownerID == ownerID && row.editorID == editorID) {
        row.savesSinceLastAccess = 0;
        return uid;
      }
    }
    if (!init) {
      return 0;
    }
    size_t uid;
    if (!freeList_.empty()) {
      uid = freeList_.front();
      freeList_.pop();
      _TRACE("Reusing freed UID: {} for owner: {}, form: {}", uid, ownerID,
             editorID);
    } else {
      uid = nextUID_++;
      _TRACE("Assigning new UID: {} for owner: {}, form: {}", uid, ownerID,
             editorID);
    }
    if (uid == 0) {
      _ERROR("UID wrapped around to 0, UID list exhausted!");
      RE::DebugMessageBox(
          "Material Swapper Framework: UID list exhausted! Report this to the "
          "author.");
      return 0;
    }
    rows_.emplace(uid, UniqueIDRow{.uid = uid,
                                   .ownerID = ownerID,
                                   .editorID = string(editorID),
                                   .savesSinceLastAccess = 0});
    string rowsJson;
    if (auto err = glz::write_json(rows_, rowsJson)) {
      auto cleanedError = glz::format_error(err);
      _ERROR("Failed to serialize UID table to JSON: {}", cleanedError);
    } else {
      _DEBUG("Current UID table: {}", rowsJson);
    }
    return uid;
  }

  const string& GetEditorID(u32 uid) {
    static constexpr string empty = "";
    if (uid == 0) {
      return empty;
    }
    if (auto it = rows_.find(uid); it != rows_.end()) {
      return it->second.editorID;
    }
    return empty;
  }

  RE::FormID GetOwnerID(u32 uid) {
    if (uid == 0) {
      return 0;
    }
    if (auto it = rows_.find(uid); it != rows_.end()) {
      return it->second.ownerID;
    }
    return 0;
  }

 private:
  unordered_map<size_t, UniqueIDRow> rows_;
  iterable_queue<size_t> freeList_;
  size_t nextUID_ = 1;
};