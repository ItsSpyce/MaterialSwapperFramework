#pragma once

#include "NifHelpers.h"

namespace UI::Pages {
struct DebuggerPageProps {
  // Add any properties needed for the debugger page
  RE::Actor* actor = nullptr;
};

inline void DebuggerPage(const DebuggerPageProps& props) {
  // This function will render the debugger page
  ImGui::Text("Debugger Page");
  ImGui::Separator();
  ImGui_Child("NifInspection") {
    ImGui::Text("NIF Inspection");
    ImGui::Text("Actor: %s", props.actor ? props.actor->GetName() : "None");
    auto* nif = props.actor->Get3D();
    if (nif) {
      NifHelpers::VisitNiObject(nif, [&](RE::NiAVObject* child) {
        ImGui_TreeNodeEx(child->name.c_str(), ImGuiTreeNodeFlags_Leaf, "Node: %s", child->name.c_str()) {
          if (auto* triShape = child->AsTriShape()) {
            if (auto* shaderProperty =
                    NifHelpers::GetShaderProperty(triShape)) {
              ImGui_TreeNodeEx("BSLightingShaderProperty") {
                if (auto* shaderMaterial =
                        skyrim_cast<RE::BSLightingShaderMaterial*>(
                            shaderProperty->material)) {
                  ImGui_TreeNodeEx("Textures") {
                    ImGui::Text(
                        "Diffuse: %s",
                        shaderMaterial->diffuseTexture
                            ? shaderMaterial->diffuseTexture->name.c_str()
                            : "None");
                    ImGui::Text(
                        "Normal: %s",
                        shaderMaterial->normalTexture
                            ? shaderMaterial->normalTexture->name.c_str()
                            : "None");
                    if (auto* envmapMaterial =
                            shaderMaterial
                                ->As<RE::BSLightingShaderMaterialEnvmap>()) {
                      ImGui::Text("EnvMap: %s",
                                  envmapMaterial->envTexture
                                      ? envmapMaterial->envTexture->name.c_str()
                                      : "None");
                      ImGui::Text(
                          "EnvMask: %s",
                          envmapMaterial->envMaskTexture
                              ? envmapMaterial->envMaskTexture->name.c_str()
                              : "None");
                    }
                    if (auto* glowmapMaterial =
                            shaderMaterial
                                ->As<RE::BSLightingShaderMaterialGlowmap>()) {
                      ImGui::Text(
                          "GlowMap: %s",
                          glowmapMaterial->glowTexture
                              ? glowmapMaterial->glowTexture->name.c_str()
                              : "None");
                    }
                  }
                }
              }
            }
          }
        }
      });
    }
  }
  // Add more debugging features here as needed
};
}  // namespace UI::Pages