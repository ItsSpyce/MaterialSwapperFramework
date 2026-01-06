#pragma once

struct MaterialRecord;
struct MaterialConfig;

namespace MaterialManager {
bool ApplyMaterialToNode(RE::BSGeometry* geometry, const MaterialRecord* record);
bool ApplyMaterialToRefr(RE::TESObjectREFR* refr, const MaterialConfig* config);
}