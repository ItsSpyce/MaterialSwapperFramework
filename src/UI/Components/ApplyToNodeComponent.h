#pragma once

#include "MaterialLoader.h"
#include "StringHelpers.h"
#include "Models/MaterialFileBase.h"
#include "UI/ImGui_Sugar.h"

namespace UI::Components {
struct ApplyToNodeProps {
  RE::Actor* actor;
  const std::string* material;
};

inline void ApplyToNodeComponent(const ApplyToNodeProps& props) {
  auto &biped = props.actor->GetCurrentBiped();
  for (const auto slot : Helpers::VALID_SLOTS) {
    if (!biped->objects[slot].item) {
      continue;
    }
    if (auto armor = biped->objects[slot].item->As<RE::TESObjectARMO>()) {
      ImGui::Text("Armor Slot: %s", StringHelpers::GetSlotName(slot));
    }
  }
  
  if (props.material) {
    /*const auto loadedMaterial =
        MaterialLoader::LoadMaterial(*props.material);
    if (const auto effectMaterial = loadedMaterial->As<BGEMFile>()) {
      ImGui::Text("Material Name: %s", props.material->c_str());
      ImGui::Text("Selected Material: %s", effectMaterial->base_map.c_str());
      ImGui::Text("Base Map: %s", effectMaterial->base_map.c_str());
      ImGui::Text("Normal Map: %s", effectMaterial->normal_map.c_str());
      ImGui::Text("Grayscale Map: %s", effectMaterial->grayscale_map.c_str());
      ImGui::Text(
          "Emittance Color: (%f, %f, %f)", effectMaterial->emittance_color.x,
          effectMaterial->emittance_color.y, effectMaterial->emittance_color.z);
    } else if (const auto lightingMaterial = loadedMaterial->As<BGSMFile>()) {
      ImGui::Text("Material Name: %s", props.material->c_str());
      ImGui::Text("Selected Material: %s",
                  lightingMaterial->diffuse_map.c_str());
      ImGui::Text("Diffuse Map: %s", lightingMaterial->diffuse_map.c_str());
      ImGui::Text("Normal Map: %s", lightingMaterial->normal_map.c_str());
      ImGui::Text("Smooth Spec Map: %s",
                  lightingMaterial->smooth_spec_map.c_str());
      ImGui::Text("Specular Map: %s", lightingMaterial->specular_map.c_str());
      ImGui::Text("Emittance Color: (%f, %f, %f)",
                  lightingMaterial->emittance_color.x,
                  lightingMaterial->emittance_color.y,
                  lightingMaterial->emittance_color.z);
    } else {
      ImGui::Text("Unknown material type.");
    }*/
  }
}
}  // namespace MaterialSwapperFramework::UI::Components