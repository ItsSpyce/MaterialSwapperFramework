#pragma once

namespace Save::Types {
using UniqueID = u64;

enum class UniqueItemLocation : u8 {
  kInventory = 0,
  kWorld = 1,
  kContainer = 2,
};

struct UniqueIDRowV1 {
  u16 uid;
  RE::FormID ownerID;
  string editorID;
  mutable u32 savesSinceLastAccess;
  u16 index;  // useless right now, but might be useful later
  UniqueItemLocation location;
};

struct UniqueIDRowV2 {
  UniqueID uniqueID;
  RE::VMHandle ownerID;
  mutable u32 savesSinceLastAccess;
  UniqueItemLocation location;
};

struct SaveDataHeaderV1 {
  static constexpr u32 IDENTIFIER = 'HEAD';
};

struct ArmorRecordEntryV1 {
  static constexpr u32 IDENTIFIER = 'ARMO';
  u16 uid;
  vector<string> appliedMaterials;
};

struct ArmorRecordEntryV2 {
  static constexpr u32 IDENTIFIER = 'ARMO';
  UniqueID uniqueID;
  vector<string> appliedMaterials;
};

struct UniqueIDHistoryV1 {
  static constexpr u32 IDENTIFIER = 'UIDT';
  vector<UniqueIDRowV1> rows{};
  vector<u16> freedUIDs{};
  u16 nextUID{1};
};

struct UniqueIDHistoryV2 {
  static constexpr u32 IDENTIFIER = 'UIDT';
  vector<UniqueIDRowV2> rows{};
  vector<UniqueID> freedUIDs{};
};

struct NPCRecordEntryV2 {
  static constexpr u32 IDENTIFIER = 'NPCD';
  RE::FormID formID;
  vector<string> appliedMaterials;
};

// should we consider just making this type an abstract for all other types
struct WeaponRecordEntryV2 {
  static constexpr u32 IDENTIFIER = 'WEAP';
  UniqueID uniqueID;
  vector<string> appliedMaterials;
};
}  // namespace Save::Types