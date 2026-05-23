# TODO

- need to link an actor to what unique IDs they own so that I can override keyword reading
- add "reload materials from disk" button in menu
- cache materials by armor ID instead of actor ref ID
- instead of modifying the existing nif, lets clone it on first mount and then process it. On re-equipping, we'll need to swap to that one instead
- revisit UID shit. Racemenu is good for it but it's not something that could work universally
  - might be useful to also look into transactional type systems for storing so that the whole save isn't loaded at once?