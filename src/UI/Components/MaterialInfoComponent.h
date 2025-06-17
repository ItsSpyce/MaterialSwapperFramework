#pragma once

namespace UI::Components {
struct MaterialInfoComponentProps {
  std::shared_ptr<ShaderMaterialFile> material;
  const char* name;
};

inline void MaterialInfoComponent(const MaterialInfoComponentProps& props) {
  if (!props.material) {
    ImGui::Text("No material selected.");
    return;
  }
  if (const auto effectMaterial = props.material->As<BGEMFile>()) {
    ImGui::Text("Material Name: %s", props.name);
    ImGui::Text("Selected Material: %s", effectMaterial->base_map.c_str());
    ImGui::Text("Base Map: %s", effectMaterial->base_map.c_str());
    ImGui::Text("Normal Map: %s", effectMaterial->normal_map.c_str());
    ImGui::Text("Grayscale Map: %s", effectMaterial->grayscale_map.c_str());
    ImGui::Text(
        "Emittance Color: (%f, %f, %f)", effectMaterial->emittance_color.x,
        effectMaterial->emittance_color.y, effectMaterial->emittance_color.z);
  } else if (const auto lightingMaterial = props.material->As<BGSMFile>()) {
    ImGui::Text("Material Name: %s", props.name);
    ImGui::Text("Selected Material: %s", lightingMaterial->diffuse_map.c_str());
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
  }
}
}  // namespace MaterialSwapperFramework::UI::Components