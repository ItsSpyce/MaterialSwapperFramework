#pragma once

namespace RE {
// Used by Misc.h. The offsets were nabbed from PO3's commonlib.
namespace Offset {
namespace NiAVObject {
constexpr auto Demand = REL::ID(75782);
constexpr auto Clone = REL::ID(70187);
}  // namespace NiAVObject
namespace NiTexture {
constexpr auto GetTexture = REL::VariantID(98986, 105640, 0);
}
namespace BSShaderTextureSet {
constexpr auto SetTexture = REL::ID(21363);
constexpr auto SetTexturePath = REL::ID(21364);
}  // namespace BSShaderTextureSet
namespace Actor {
constexpr auto AttachArmor = REL::VariantID(15535, 15712, 0x001DB9E0);
constexpr auto CreateWeaponNodes = REL::VariantID(19342, 19769, 0);
}  // namespace Actor
namespace Main {
constexpr auto Update = REL::VariantID(35565, 36564, 0x005BAB10);
constexpr auto UpdateOffset = REL::VariantOffset(0x748, 0xC26, 0X7EE);
}  // namespace Main
}  // namespace Offset
}  // namespace RE