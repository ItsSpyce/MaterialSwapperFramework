#pragma once

namespace RE {
// Used by Misc.h. The offsets were nabbed from PO3's commonlib.
namespace Offset {
namespace NiAVObject {
constexpr auto Demand = REL::ID(75782);
}
namespace NiAVObject {
constexpr auto Clone = REL::ID(70187);
}
namespace NiTexture {
constexpr auto GetTexture = REL::ID(105640);
}
namespace BSShaderTextureSet {
constexpr auto SetTexture = REL::ID(21363);
constexpr auto SetTexturePath = REL::ID(21364);
}
}  // namespace Offset
}  // namespace RE