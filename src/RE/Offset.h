#pragma once

namespace RE {
// Used by Misc.h. The offsets were nabbed from PO3's commonlib.
namespace Offset {
namespace NiAVObject {
constexpr auto Demand = REL::ID(75782);
constexpr auto Clone = REL::ID(70187);
}  // namespace NiAVObject
namespace NiTexture {
constexpr auto GetTexture = RELOCATION_ID(98986, 105640);
}
namespace BSShaderTextureSet {
constexpr auto SetTexture = REL::ID(21363);
constexpr auto SetTexturePath = REL::ID(21364);
}  // namespace BSShaderTextureSet
namespace Actor {
constexpr auto AttachArmor = REL::VariantID(15535, 15712, 0x001DB9E0);
constexpr auto CreateWeaponNodes = RELOCATION_ID(19342, 19769);
constexpr auto CreateArmorNodes = RELOCATION_ID(15535, 15712);
}  // namespace Actor
namespace AIProcess {
constexpr auto UpdateEquipment = RELOCATION_ID(38404, 39395);
}
namespace PlayerCamera {
constexpr auto UpdatePOV = RELOCATION_ID(39401, 40476);
constexpr auto UpdatePOVOffset = REL::VariantOffset(0x2AF, 0x294, NULL);
}  // namespace PlayerCamera
namespace Main {
constexpr auto Update = REL::VariantID(35565, 36564, 0x005BAB10);
constexpr auto UpdateOffset = REL::VariantOffset(0x748, 0xC26, 0X7EE);
}  // namespace Main
namespace BSLightingShaderMaterial {
constexpr auto Ctor = RELOCATION_ID(100004, 106711);
constexpr auto ApplySkinToGeometry = RELOCATION_ID(NULL, 15739);
}
namespace BSLightingShaderMaterialEnvmap {
constexpr auto Ctor = RELOCATION_ID(100021, 106728);
}
namespace BSLightingShaderMaterialFacegen {
constexpr auto Ctor = RELOCATION_ID(100077, 106784);
}
namespace BSLightingShaderMaterialGlowmap {
constexpr auto Ctor = RELOCATION_ID(100045, 106752);
}
namespace BSShaderProperty {
constexpr auto SetMaterial = RELOCATION_ID(98897, 105544);
}
namespace TESObjectREFR {
constexpr auto WornHasKeyword = REL::VariantID(21190, 21647, 0x02EF0D0);
}
namespace BipedAnim {
constexpr auto ApplySkinnedObject = REL::ID(15711);
constexpr auto AttachSkinnedObject = REL::ID(15712);
constexpr auto AttachModsForBipedObject = REL::ID(15746);
}
}  // namespace Offset
}  // namespace RE