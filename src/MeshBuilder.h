#pragma once

struct MATR;
struct MATC;

namespace MeshBuilder {
bool ApplyMaterialToNode(RE::BSGeometry* geometry, const MATR& record);
bool ApplyMaterialToMesh(RE::NiNode* root, const MATC& matc);
bool ApplyMaterialToRefr(RE::TESObjectREFR* refr, const MATC& matc);
}  // namespace MeshBuilder