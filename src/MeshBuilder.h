#pragma once

struct MATC;

namespace MeshBuilder {
void ApplyMaterialToMesh(RE::NiNode* root, const MATC& matc);
}  // namespace MeshBuilder