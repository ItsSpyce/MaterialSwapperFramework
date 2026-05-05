#pragma once

#include "SKEE64PluginInterface.h"

class SKEE64Instance : public Singleton<SKEE64Instance> {
 public:
  SKEE64Instance() = default;
  virtual ~SKEE64Instance() = default;

  bool Initialize() {
    SKEE64::InterfaceExchangeMessage interfaceExchangeMessage;
    SKSE::GetMessagingInterface()->Dispatch(
        SKEE64::InterfaceExchangeMessage::kMessage_ExchangeInterface,
        &interfaceExchangeMessage, 0, "SKEE");
    if (!interfaceExchangeMessage.interfaceMap) {
      return false;
    }
    interfaceMap_ = std::move(interfaceExchangeMessage.interfaceMap);
    return interfaceMap_ != nullptr;
  }

  template <typename SKEE64Interface>
  SKEE64Interface* Query() const {
#define CHECK_IFACE(_NAME, _TYPE)                                           \
  if constexpr (std::is_same_v<SKEE64Interface, SKEE64::I##_NAME##_TYPE>) { \
    return static_cast<SKEE64::I##_NAME##_TYPE*>(                           \
        interfaceMap_->QueryInterface(#_NAME));                             \
  }

    CHECK_IFACE(BodyMorph, Interface)
    CHECK_IFACE(NiTransform, Interface)
    CHECK_IFACE(Overlay, Interface)
    CHECK_IFACE(Attachment, Interface)
    CHECK_IFACE(ItemData, Interface)
    CHECK_IFACE(Command, Interface)
    CHECK_IFACE(ActorUpdate, Manager)
    CHECK_IFACE(Override, Interface)
    CHECK_IFACE(Preset, Interface)
#undef CHECK_IFACE
    return nullptr;
  }

 private:
  SKEE64::IInterfaceMap* interfaceMap_;
};