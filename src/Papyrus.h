#pragma once

inline bool GetStaticFunction(
    const char* script, const char* func,
    RE::BSTSmartPointer<RE::BSScript::ObjectTypeInfo>& typeInfo,
    RE::BSTSmartPointer<RE::BSScript::IFunction>& iFunction) {
  using getStaticFunction_t =
      bool (*)(RE::BSScript::Internal::VirtualMachine*, const char*,
               const char*, RE::BSTSmartPointer<RE::BSScript::ObjectTypeInfo>&,
               RE::BSTSmartPointer<RE::BSScript::IFunction>&);
  REL::Relocation<getStaticFunction_t> Functor(RELOCATION_ID(98123, 104846));
  return Functor(RE::BSScript::Internal::VirtualMachine::GetSingleton(), script,
                 func, typeInfo, iFunction);
}

template <class func_t>
func_t GetNativeFunction(const char* script, const char* func) {
  RE::BSTSmartPointer<RE::BSScript::ObjectTypeInfo> typeInfo;
  RE::BSTSmartPointer<RE::BSScript::IFunction> iFunction;
  if (!GetStaticFunction(script, func, typeInfo, iFunction)) return nullptr;
  return reinterpret_cast<func_t>((uint64_t)iFunction.get() + 0x50);
}
