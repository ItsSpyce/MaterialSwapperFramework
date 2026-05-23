#pragma once

struct MaterialRecord;
struct MaterialConfig;

namespace MaterialManager {
RE::BSGeometry* ApplyMaterialToNode(RE::BSGeometry* geometry, const MaterialRecord* record, const char* key);
std::vector<RE::BSGeometry*> ApplyMaterialToRefr(RE::TESObjectREFR* refr, const MaterialConfig* config);
}