#pragma once

#include <RmlUi/Core.h>

#include "MaterialSwapper.h"
#include "UI/RmlHelpers.h"

namespace UI::Models {
using namespace Rml;

struct InventoryItem;

inline struct MenuModel {
  std::vector<InventoryItem> items;
} g_menuModel;

struct InventoryItem {
  using T = InventoryItem;
  RE::FormID formID;
  UniqueID uid;
  String originalName;
  std::vector<String> availableMaterials;
  std::vector<String> appliedMaterials;

  void ApplyMaterial(DataModelHandle& handle, Event& event,
                     const VariantList& variants) {
    const auto* target = RE::Console::GetSelectedRef()
                             ? RE::Console::GetSelectedRef().get()
                             : RE::PlayerCharacter::GetSingleton();
    if (const auto args = RmlHelpers::make_tuple_from_args<String>(variants)) {
      const auto [materialName] = args.value();
      auto* form = RE::TESForm::LookupByID(formID);
      if (auto* actor = target->As<RE::Actor>()) {
        if (const auto* armo = form->As<RE::TESObjectARMO>()) {
          MaterialSwapper::ApplyArmorMaterial(actor, RE::BipedObjectSlot::kBody,
                                              nullptr);
        }
      }
      appliedMaterials.emplace_back(materialName);
      handle.DirtyVariable("appliedMaterials");
    }
  }

  static constexpr void bind(StructHandle<InventoryItem>& handle) {
    handle.RegisterMember("formID", &T::formID);
    handle.RegisterMember("uid", &T::uid);
    handle.RegisterMember("originalName", &T::originalName);
    handle.RegisterMember("availableMaterials", &T::availableMaterials);
    handle.RegisterMember("appliedMaterials", &T::appliedMaterials);
  }
};

inline bool CreateBindings(Context* ctx, DataModelHandle& handle) {
  auto ctor = ctx->CreateDataModel("MSFMenu");
  if (!ctor) return false;
  ctor.RegisterArray<std::vector<String>>();
  if (auto modelHandle = ctor.RegisterStruct<InventoryItem>()) {
    InventoryItem::bind(modelHandle);
  }
  ctor.RegisterArray<std::vector<InventoryItem>>();
  ctor.Bind("items", &g_menuModel.items);
  handle = ctor.GetModelHandle();
  return true;
}
}  // namespace UI::Models