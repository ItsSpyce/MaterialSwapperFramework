#pragma once

struct MATR;
struct MATC;

namespace MaterialManager {
bool ResetMaterial(RE::BSGeometry* geometry);
bool ResetMaterials(RE::TESObjectREFR* refr);
bool ApplyMaterialToNode(RE::BSGeometry* geometry, const MATR& record);
bool ApplyMaterialToRefr(RE::TESObjectREFR* refr, const MATC& config);
}