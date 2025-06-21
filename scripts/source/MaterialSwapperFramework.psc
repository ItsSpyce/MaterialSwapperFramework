scriptname MaterialSwapperFramework Hidden

int Function GetVersion() global
  return 1
EndFunction

; Returns a list of all available materials.
string[] Function GetMaterials() native global
string[] Function GetMaterialsForArmor(Armor armo) native global
string[] Function GetMaterialsForSlot(ObjectReference ref, int slot) native global
string[] Function GetMaterialsForActor(Actor actor) native global

; Gets the current material for a specific armor or slot.
string Function GetCurrentMaterialForArmor(ObjectReference ref, Armor armo) native global
string Function GetCurrentMaterialForSlot(ObjectReference ref, int slot) native global
string Function GetCurrentMaterialForActor(Actor actor) native global

; Applies a material
bool Function ApplyMaterialToArmor(ObjectReference ref, Armor armo, string materialName) native global
bool Function ApplyMaterialToSlot(ObjectReference ref, int slot, string materialName) native global
bool Function ApplyMaterialToActor(Actor actor, string materialName) native global

; Resets materials
bool Function ResetMaterialForArmor(ObjectReference ref, Armor armo) native global
bool Function ResetMaterialForSlot(ObjectReference ref, int slot) native global
bool Function ResetMaterialForActor(Actor actor) native global