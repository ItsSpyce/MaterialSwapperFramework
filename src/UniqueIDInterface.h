#pragma once

#include "Singleton.h"

class UniqueIDInterface : public Singleton<UniqueIDInterface> {
  class ItemAttributeData;
  struct ModifiedItem {
    RE::TESForm* form;
    RE::BaseExtraList* extraData;
    bool isWorn;

    ModifiedItem() : form(nullptr), extraData(nullptr), isWorn(false) {}

    operator bool() const { return form && extraData; }

    std::shared_ptr<ItemAttributeData> GetAttributeData(RE::TESObjectREFR* refr,
                                                        uint32_t* out = NULL) {
      RE::ExtraRank* rank = nullptr;
    }
  };

  class ItemAttributeData {
   public:
    void Save(SKSE::SerializationInterface* serialization) const;
    void Load(SKSE::SerializationInterface* serialization);
  };

 public:
  int32_t GetUID(RE::Actor* actor, RE::FormID formID) {
    auto armo = actor->GetWornArmor(formID);
    if (!armo) {
      return 0;
    }
    if (auto ref = armo->AsReference()) {
      auto &extraList = ref->extraList;
      if (extraList.HasType(RE::ExtraDataType::kRank)) {
        return 0;
      }
      // TODO:
      auto rank = new RE::ExtraRank(1);
      extraList.Add(rank);
      return rank->rank;
    }
    return 0;
  }
};