# Wand and Wistoria Unreal Project

This project is being converted from the First Person Blueprint template into a C++ backed action RPG prototype.

## Current Gameplay Foundation

- `AWistoriaCharacter`: player base with health, mana, courage, XP, coin collection, save/load hooks, and Blueprint events for attacks/spells/defeat.
- `ACoin`: collectible actor with mesh, trigger collision, score value, audio, Niagara VFX, and collector interface support.
- `AWistoriaEnemy`: scalable enemy base with health, damage, elemental identity, damage events, and defeat events.
- `ADungeonWaveSpawner`: trigger volume that starts enemy waves and scales enemy count/stats based on companion count.
- `AWistoriaGameMode`: default game mode, party-size difficulty scaling, and player XP reward helper.
- `UWistoriaSaveGame`: save object for level, XP, coins, weapon log, story flags, checkpoint respawn data, and New Game Plus count.
- `UWistoriaMeleeHitbox`: Blueprint-spawnable melee damage volume for animation-notify attack windows.
- `AWistoriaSpellProjectile`: configurable projectile actor for elemental spell damage, impact sound, and Niagara VFX.
- Spell projectiles can now use `FWistoriaSpellDataRow` status metadata: `StatusToApply`, `StatusDuration`, and `StatusChance`.
- `UWistoriaStatusComponent`: status-effect container with corruption miasma support.
- `AMiasmaCrystalNode`: interactable dungeon crystal that clears miasma and can drive companion/objective gameplay.
- `AWorldLootPickup`: world item/gold pickup actor with overlap collection, manual interaction, sound, and Niagara VFX hooks.
- `ATreasureChest`: interactable one-time reward chest for exploration loot, crafting materials, weapons, and gold; optional `ChestId` persistence uses save story flags.
- `ASavePoint`: interactable rest/checkpoint actor that restores resources, saves progression, and stores respawn location/rotation.
- Character customization on `AWistoriaCharacter` supports dynamic hair/robe colors, saves them, and exposes a `UWistoriaCharacterCreatorWidget` parent for UMG.
- `AWistoriaMountCharacter`: rideable summoned mount with fast movement, possession transfer, rider attachment, and dismount support.
- `AWistoriaRaidGameMode`: end-game raid referee for active wave, alive enemies, max waves, inter-wave delay, final boss wave, raid score, victory, and failure.
- `ARaidSpawnManager`: raid arena spawner that starts raid instances and spawns scaled enemy waves from local offsets or explicit world spawn locations.
- Raid and dungeon wave scaling now reads the player's actual active NPC companion count at runtime, increasing enemy count and enemy stat multipliers for larger AI squads.
- The project is single-player only; dungeon and raid party support comes from NPC companions rather than networked players.
- `ACompanionAIController`: single-player companion controller that follows the player and switches to enemy support behavior during dungeon/raid combat.
- `ACompanionPartySpawner`: dungeon/raid entrance actor that spawns a capped NPC party and lets companions auto-register with the player.
- `IWistoriaInteractable`: shared interface for crystals, NPCs, doors, loot, and future command targets.
- `UWistoriaInventoryComponent`: stack-based inventory for weapons, consumables, key items, and materials.
- `UWistoriaQuestComponent`: active/completed quest records with objective progress tracking.
- `AWistoriaCompanionCharacter`: AI companion base with follow, hold, attack-target, and interact-target commands.
- `AWistoriaCharacter` keeps an active companion roster and exposes party-wide follow, hold, and attack-locked-target command functions.
- `UWistoriaSkillComponent`: skill tree state, skill-point spending, prerequisite checks, and save/load support.
- Equipment slots and data-table rows in `WistoriaTypes.h` support large weapon/spell logs: 1,000 swords/wands and 500 spells through Data Tables.
- `FWistoriaCraftingRecipe` is also a Data Table row, so blacksmith upgrades can be authored in bulk instead of hardcoded.
- `UWistoriaItemDataAsset`, `UWistoriaQuestDataAsset`, and `UWistoriaStarterLoadoutDataAsset` support hand-authored items, quests, and starting kits from the Content Browser.
- `AFastTravelGate`: interactable teleport gate activation point for open-world traversal.
- `ASkyCycleManager`: day/night sun rotation with fog/rain storm toggles.
- `AWistoriaAudioManager`: dynamic music actor with `CombatIntensity` parameter support.
- `ADialogueNPC` and `AShopVendorNPC`: quest-giving dialogue and stocked buy/sell merchant economy hooks.
- `AWistoriaBossEnemy`: cinematic boss base with boss health change events for a large HUD bar.
- Player parry is implemented on `AWistoriaCharacter` through `StartParry`, `TryResolveParry`, and `OnParrySucceeded`.
- Player dodge is implemented on `AWistoriaCharacter` through `Dodge`, `DodgeStrength`, `DodgeCooldown`, `DodgeCourageCost`, and `OnDodgeStarted`.
- `AWistoriaEnemy::TryMeleeAttack` respects parry windows before applying damage.
- `AWistoriaEnemyAIController` gives enemies runtime chase/attack behavior with aggro radius, `MoveToActor`, melee cooldown, and Blueprint target-acquired/lost hooks.
- `AWistoriaEnemy` now has `EnemyId`, `XPReward`, and `LootDrops`; defeated enemies award XP, update matching quest objectives, and grant loot to the killing player.
- `UWistoriaQuestComponent::AdvanceAnyActiveObjectiveWithCompletions` supports kill-objective completion and quest reward XP.
- `ABossArenaTrigger` starts boss arena presentation logic and can create/open the boss HUD from Blueprint.
- `UWistoriaDataLogLibrary` provides Blueprint lookup helpers for weapon/spell Data Tables and converts weapon rows into inventory items.
- `UWistoriaHUDLibrary` creates a HUD snapshot for health, mana, courage, XP, coins, gold, targeting state, and primary quest text.
- `UWistoriaSaveLibrary` provides Blueprint menu helpers for save/load/delete/default slot and New Game Plus.
- `UWistoriaSaveLibrary` also exposes three title-screen save slots: `WistoriaSlot_1`, `WistoriaSlot_2`, and `WistoriaSlot_3`, with Blueprint-readable slot summaries for level, gold, NG+, raid clears, and academy rank.
- `UWistoriaGameInstance` stores active save-slot and pending New Game Plus state across map loads.
- `AWistoriaPlayerController` creates HUD and pause widgets and handles Escape pause toggling.
- `UWistoriaUIWidgets` provides C++ parent widgets for HUD, pause, inventory, quests, skills, boss health, raid HUD, dialogue, shop, fast travel, companion command, character creator, and save-slot manager screens.
- `AWistoriaCharacter` can spawn a Niagara Courage burst effect when Will-style combat spends `80` Courage through `CastActiveSpell`.
- `AWistoriaCharacter` supports Enhanced Input assets through `DefaultMappingContext`, `MoveAction`, `LookAction`, `JumpAction`, `AttackAction`, `CastSpellAction`, `InteractAction`, `LockOnAction`, `ParryAction`, and `DodgeAction`, while retaining legacy fallback mappings.
- Weapons with rolled affixes can be reforged through `ReforgeWeaponSlot`, spending gold to reroll one affix while preserving the item and rarity tier.
- `FWistoriaAcademyRank` tracks end-game academy titles, required raid clears, required raid score, cosmetic cape IDs, and optional reward cape skeletal meshes. Raid clears add academy rank score from raid score and increment total raid clears.
- `DataSeeds/DT_WeaponLog.csv` contains 1,000 generated swords/wands for import into a Weapon Data Table.
- `DataSeeds/DT_SpellBook.csv` contains 500 generated spells with status-effect metadata for import into a Spell Data Table.
- `DataSeeds/DT_CraftingRecipes.csv` contains 120 generated blacksmith recipes for import into a Crafting Recipe Data Table.
- `Tools/GenerateWistoriaDataSeeds.ps1` regenerates those CSV logs on demand.
- `Tools/CleanUnrealGeneratedFiles.ps1` safely removes generated Unreal build folders after closing Unreal/Visual Studio. Run with `-WhatIf` first.
- `MISSION_01_THE_FIRST_DESCENT.md` contains the first playable campaign mission, dialogue script, level layout, quest objectives, and test checklist.
- `NIAGARA_WILL_ULTIMATE_AURA_BLAST.md` contains the three-layer Courage/Wis ultimate Niagara setup for ribbon burst, ground crack radial expansion, and body aura mesh glow.
- `PACKAGING_AND_KEYBINDING.md` contains the final Windows packaging checklist and Enhanced Input keybinding menu plan.
- `WORLD_PARTITION_OPTIMIZATION.md` contains the cell sizing, HLOD, foliage, fast travel, and raid-map optimization pass.

## Required Local Tooling Before Compile

UnrealBuildTool currently stops before compiling because the Windows C++ SDK/toolchain is missing.

Install these through Visual Studio Installer:

- Desktop development with C++
- MSVC C++ toolset
- Windows 10/11 SDK, version 10.0.19041.0 or newer
- Visual Studio Tools for Unreal Engine

Then right-click `MyProject.uproject` and generate project files, or build `MyProjectEditor Win64 Development`.

If generated files become stale after installing the SDK, close Unreal and Visual Studio, then preview cleanup with:

```powershell
.\Tools\CleanUnrealGeneratedFiles.ps1 -WhatIf
```

To remove generated build folders:

```powershell
.\Tools\CleanUnrealGeneratedFiles.ps1
```

Use `-IncludeSaved` only if you intentionally want to delete saved logs/autosaves/config output. Use `-IncludeDerivedDataCache` only if shader/cache corruption is suspected.

## Editor Assembly Steps

The C++ side already contains the core classes from the original prototype roadmap: character combat, dungeon zones, wave spawners, spell projectiles, parry, lock-on, inventory, quests, companions, raids, loot rarity, and puzzle switches. The remaining work is editor asset assembly and local compiler setup.

1. Create `BP_WistoriaCharacter` from `AWistoriaCharacter`.
2. Assign `BP_WistoriaCharacter` as the default pawn in a Blueprint subclass of `AWistoriaGameMode`, or use the C++ game mode directly.
3. Create `BP_WistoriaPlayerController` from `AWistoriaPlayerController` and assign HUD/Pause widget classes, or set those classes on the C++ controller defaults after Blueprint assets exist.
4. Create `BP_Coin` from `ACoin`, assign a coin mesh, pickup sound, and Niagara effect.
5. Create `BP_WorldLootPickup` from `AWorldLootPickup`, `BP_TreasureChest` from `ATreasureChest`, and `BP_SavePoint` from `ASavePoint`; assign meshes, pickup/open/rest sounds, VFX, and reward data.
   Give persistent chests unique `ChestId` values such as `AcademyCourtyardChest01` so opened state survives save/load.
6. Create `BP_MagicalMount` from `AWistoriaMountCharacter`, assign a mount mesh, add a `RiderSeat` socket, and tune `MountedWalkSpeed`.
7. Assign `BP_MagicalMount` to `BP_WistoriaCharacter`'s `MountClass`; the fallback mount key is `H`.
8. Create at least one `BP_WistoriaEnemy` from `AWistoriaEnemy` and one `BP_DungeonBoss` from `AWistoriaBossEnemy`.
9. Set each enemy Blueprint's `EnemyId`, `XPReward`, and `LootDrops`, then place `ADungeonWaveSpawner` in the level and add enemy Blueprint classes to `EnemyClasses`.
   To test procedural weapons, enable `bRollProceduralWeaponDrop`, add base weapon templates to `ProceduralWeaponDropPool`, and leave the default rarity rates at Legendary `0.10`, Epic `0.35`, Rare `0.30`, Standard remaining `0.25`.
   Use `ADungeonZone` for open-world floor/sector entrances that should spawn a one-time monster pool around the entering player; use `ADungeonWaveSpawner` for combat rooms that need tracked wave counts and completion events.
10. Add a Nav Mesh Bounds Volume covering combat floors and press `P` in the editor to confirm green navigation surfaces for `AWistoriaEnemyAIController`.
11. Create Enhanced Input assets: `IMC_Wistoria`, `IA_Move`, `IA_Look`, `IA_Jump`, `IA_Attack`, `IA_CastSpell`, `IA_Interact`, `IA_LockOn`, `IA_Parry`, and `IA_Dodge`.
12. Assign those input assets to `BP_WistoriaCharacter`'s Enhanced Input properties.
    Map `IA_Attack` to Left Mouse Button, `IA_CastSpell` to Right Mouse Button or `E`, `IA_Parry` to Left Shift, `IA_Dodge` to Left Alt, `IA_LockOn` to Middle Mouse Button, and `IA_Interact` to `E` if spell casting uses Right Mouse Button.
    The legacy `DefaultInput.ini` mappings remain as a fallback.
    The legacy command wheel action is mapped to `C` and is handled by `AWistoriaPlayerController`.
    The legacy mount action is mapped to `H`.
    For quick combat testing before HUD widgets exist, enable `bShowDebugResourceOverlay` on `BP_WistoriaCharacter`; it displays style, health, mana, courage, level, and XP on screen while playing.
13. Bind animation events in the character Blueprint to `ActivateMeleeWindow`, `DeactivateMeleeWindow`, `CastSpellSlotOne`, `TryInteract`, `ToggleLockOn`, `StartParry`, and `Dodge` as needed.
14. Assign a Blueprint subclass of `AWistoriaSpellProjectile` to `SpellProjectileClass`.
15. Use animation notifies to call `ActivateMeleeWindow` at the start of sword contact frames and `DeactivateMeleeWindow` when the swing ends.
16. Place `AMiasmaCrystalNode` actors in dungeon rooms and use `UWistoriaStatusComponent::ApplyStatus(Miasma)` from hazard volumes.
17. Create Weapon, Spell, and Crafting Recipe Data Tables from `FWistoriaWeaponDataRow`, `FWistoriaSpellDataRow`, and `FWistoriaCraftingRecipe`; import CSV rows for the large equipment, spell, and blacksmith libraries.
    Spell rows can control status effects through `StatusToApply`, `StatusDuration`, and `StatusChance`.
    Use Data Tables for the large 1,000 weapon / 500 spell catalogs, and Data Assets for special authored items, professor quests, story rewards, and player starter loadouts.
18. Place `AFastTravelGate`, `ASkyCycleManager`, and `AWistoriaAudioManager` actors in the world.
19. Place `ABossArenaTrigger` at boss arena entrances and bind `OnBossArenaEntered` to create the cinematic boss HUD and increase music intensity.
20. For single-player dungeon and raid parties, place `ACompanionPartySpawner` near the entrance and assign 1-3 companion Blueprint classes.
21. For end-game raids, create a raid level or arena using `AWistoriaRaidGameMode`, place `ARaidSpawnManager`, assign enemy classes, optional `FinalBossClass`, local/world spawn locations, `SecondsBetweenWaves`, raid clear rewards, and tune `MaxWaves`, `BaseEnemiesPerWave`, and per-wave scaling.
    Raid victory grants `50` affinity to every active companion with a valid `CompanionId`.
22. Use `UWistoriaHUDLibrary::MakeHUDSnapshot` for HUD binding and `UWistoriaSaveLibrary` for save-slot menu buttons.
23. Bind UI widgets to `OnHealthChanged`, `OnManaChanged`, `OnCourageChanged`, `OnCoinsChanged`, boss health events, raid state events, skill changes, inventory changes, parry success, customization changes, and status-change events.
24. For kill quests, set objective IDs to match enemy `EnemyId` values such as `DungeonBeast`.
25. Make sure interactable meshes block the `Visibility` channel so `AWistoriaCharacter::TryInteract` can trace them.
26. Create Widget Blueprints from these C++ parents:
    - `UWistoriaHUDWidget` -> `WBP_WistoriaHUD`
    - `UWistoriaPauseMenuWidget` -> `WBP_PauseMenu`
    - `UWistoriaInventoryWidget` -> `WBP_Inventory`
    - `UWistoriaQuestJournalWidget` -> `WBP_QuestJournal`
    - `UWistoriaSkillTreeWidget` -> `WBP_SkillTree`
    - `UWistoriaBossHUDWidget` -> `WBP_BossHUD`
    - `UWistoriaRaidHUDWidget` -> `WBP_RaidHUD`
    - `UWistoriaDialogueWidget` -> `WBP_Dialogue`
    - `UWistoriaShopWidget` -> `WBP_Shop`
    - `UWistoriaFastTravelWidget` -> `WBP_FastTravel`
    - `UWistoriaCompanionCommandWidget` -> `WBP_CompanionCommandWheel`
    - `UWistoriaCharacterCreatorWidget` -> `WBP_CharacterCreator`
    - `UWistoriaSaveSlotManagerWidget` -> `WBP_SaveSlotManager`
27. Assign `WBP_WistoriaHUD`, `WBP_PauseMenu`, and `WBP_CompanionCommandWheel` to `AWistoriaPlayerController`; open the remaining menus from interaction, pause, raid start, boss-arena, character-creator, or fast-travel Blueprint events.
28. For companion puzzle rooms, place one `AWistoriaDungeonPuzzleGate` and two or more `AWistoriaPuzzleSwitch` actors.
    Give each switch a unique `SwitchId`, set the same `LinkedGate`, and set the gate's `RequiredActiveSwitches` to the number of simultaneous activations required.
    Companions can activate switches through the existing `InteractWithTarget` command, while the player uses the normal interact input.
29. Create optional starter Data Assets:
    - `UWistoriaItemDataAsset` for special swords, robes, potions, dungeon tokens, or quest materials.
    - `UWistoriaQuestDataAsset` for professor/NPC quest templates.
    - `UWistoriaStarterLoadoutDataAsset` for first-play equipment, gold, unlocked weapons, and starter quests.
    Assign the loadout asset to `BP_WistoriaCharacter`'s `StarterLoadout` property.
30. For blacksmith reforging, build a widget from `UWistoriaInventoryWidget`, show `GetInventoryItems`, and call `ReforgeWeaponSlot(ItemIndex, AffixIndex, CostInGold)` from the selected weapon row.
31. For academy title progression, populate `BP_WistoriaCharacter`'s `AcademyRanks` array with rank IDs, display titles, required levels, required raid clears, required raid scores, cosmetic cape IDs, and optional reward cape skeletal meshes.
    Bind `OnAcademyRankUnlocked` in Blueprint to show a title-unlocked toast. The C++ `CosmeticCapeComponent` will apply `RewardCapeMesh` automatically when present.
32. In `WBP_CompanionCommandWheel`, add an Activate Object button. Trace under the crosshair in Blueprint, then call `CommandInteractWithTarget(TargetActor)` so companions can run to puzzle switches/levers.
33. Build the first playable story pass from `MISSION_01_THE_FIRST_DESCENT.md`.
34. Build `WBP_SaveSlotManager` from `UWistoriaSaveSlotManagerWidget`.
    On widget construct, call `GetSaveSlots(3)` and create three rows.
    For an empty row, show New Game and call `StartNewGameInSlot(SlotIndex)`, then `OpenGameplayLevel`.
    For a filled row, show Load Game and call `LoadGameInSlot(SlotIndex)`, then `OpenGameplayLevel`.
    Optional delete buttons can call `DeleteSaveSlot(SlotIndex)` and refresh the rows.
35. Create `NS_Will_CourageAuraBlast` using `NIAGARA_WILL_ULTIMATE_AURA_BLAST.md`, assign it to `BP_WistoriaCharacter.CourageBurstEffect`, and test by spending `80` Courage.
36. Build the title screen map with `WBP_SaveSlotManager`, then follow `PACKAGING_AND_KEYBINDING.md` for Shipping packaging and the custom keybinding menu.
37. Use `WORLD_PARTITION_OPTIMIZATION.md` for the open-world streaming/HLOD pass after Mission 01 and the raid arena are playable.

## Next Gameplay Milestones

1. Create the Enhanced Input assets in the editor and assign `IA_Dodge` to the character's `DodgeAction` property.
2. Create Blueprint children for player, enemy, boss, projectile, coin, loot pickup, treasure chest, save point, magical mount, NPC companion, NPC, vendor, gate, and miasma crystal with real meshes/VFX.
   Also create optional Blueprint children for `ADungeonZone` and `ADungeonWaveSpawner` so designers can tune floor level, monster pools, wave counts, and spawn offsets in the Details panel.
3. Build the visual layout for `WBP_CompanionCommandWheel`; its C++ parent already calls Follow, Hold, Focus Fire, Defend Player, and Team Ultimate functions.
4. Add the visual layout for `WBP_SaveSlotManager`, using the C++ save-slot functions now available for three title-screen slots.
   Bind defeat UI to `RespawnAtCheckpoint` after the player has activated at least one save point.
5. Optionally replace `AWistoriaEnemyAIController` with Behavior Trees once the first loop is working.
6. Build visual layouts for the C++ backed inventory, quest journal, skill tree, shop, dialogue, fast-travel, and boss HUD widgets.
7. Import `DataSeeds/DT_WeaponLog.csv`, `DataSeeds/DT_SpellBook.csv`, and `DataSeeds/DT_CraftingRecipes.csv` into Weapon, Spell, and Crafting Recipe Data Tables.
   Populate vendor `MerchantInventory` arrays with `FWistoriaShopStockItem` entries so shop UIs can use buy/sell prices and limited stock.
8. Add NG+, HUD, skill tree, and boss HUD UMG widgets.
9. Set `HUDWidgetClass` and `PauseMenuWidgetClass` on `AWistoriaPlayerController` or a Blueprint subclass.
10. Add parry VFX/audio, boss intro widgets, and combat camera polish in Blueprints.
11. Build the visual layout for `WBP_CharacterCreator` with hair and robe color controls bound to `SetHairColor` and `SetRobeColor`.
12. Build `WBP_RaidHUD` from `UWistoriaRaidHUDWidget` and bind it to `RefreshRaidSnapshot`.
13. Create Blueprint children for `AWistoriaDungeonPuzzleGate` and `AWistoriaPuzzleSwitch`, add rune/lever meshes, and test a two-switch room with one player activation and one companion activation.
14. Build a blacksmith/reforge UI and test reforging an Epic or Legendary weapon affix after earning gold.
15. Add Academy Rank rows to the player Blueprint and confirm raid clears unlock title/cape IDs.
16. Build `NS_Will_CourageAuraBlast` and assign it to the player Courage burst effect slot.
17. Build `WBP_KeyRebindMenu` after core Enhanced Input assets are assigned.
18. Add World Partition/HLOD settings after the first playable dungeon loop is working.
19. Package a Windows Shipping build after installing the required Windows SDK.
