#pragma once

#include <emhash/hash_table8.hpp>

#include "Save/Save.h"
#include "Types.h"

class ModState : public Singleton<ModState>, ISaveable {
 public:
  bool IsReady() const { return isReady_; }
  void SetReady(bool ready) { isReady_ = ready; }
  bool IsCSInstalled() const { return isCSInstalled_; }
  void SetCSInstalled(bool installed) { isCSInstalled_ = installed; }

  void ReadFromSave(Save::SaveData&) override {}
  void WriteToSave(Save::SaveData&) override {}

  void VisitMaterials(const UniqueID uid,
                      const Visitor<const std::string&>& visitor) const {
    FIND_IN(materials_, it, uid) {
      for (const auto& material : it->second) {
        if (visitor(material) == RE::BSVisit::BSVisitControl::kStop) return;
      }
    }
  }

  auto GetMaterials(const UniqueID uid) const {
    FIND_IN(materials_, it, uid) {
      return it->second;
    }
    return std::vector<std::string>{};
  }

  void SetMaterials(const UniqueID uid,
                    const std::vector<std::string>& materials) {
    materials_[uid] = materials;
  }

 private:
  bool isReady_{false};
  bool isCSInstalled_{false};
  emhash8::HashMap<UniqueID, std::vector<std::string>> materials_;
};