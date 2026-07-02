# Wand and Wistoria Beta Test Project

This folder is the loadable Unreal Engine project for the current beta build.

## Open The Project

1. Install Unreal Engine `5.8`.
2. Install Visual Studio with:
   - Desktop development with C++
   - MSVC C++ toolset
   - Windows SDK `10.0.19041.0` or newer
   - Visual Studio Tools for Unreal Engine
3. Open `MyProject.uproject`.
4. If Unreal asks to rebuild modules, choose rebuild.

## Current Default Map

The project currently opens to:

```text
/Game/Maps/LV1_TestArena
```

This is the current beta test arena for verifying the gameplay framework.

## Important Before Building

If Unreal Editor is already open and Live Coding is enabled, command-line builds will stop with:

```text
Unable to build while Live Coding is active.
```

Close Unreal Editor or press `Ctrl+Alt+F11` in the editor, then run:

```powershell
.\Tools\BuildWistoriaEditor.ps1
```

To verify the project folder and beta archive have the expected files:

```powershell
.\Tools\ValidateBetaProject.ps1
```

To verify the generated weapon, spell, and crafting CSV libraries:

```powershell
.\Tools\ValidateDataSeeds.ps1
```

To refresh the beta archive and run all safe validators:

```powershell
.\Tools\PrepareBetaRelease.ps1
```

To also compile the editor target after closing Unreal or disabling Live Coding:

```powershell
.\Tools\PrepareBetaRelease.ps1 -Build
```

After a successful build, run the core automation tests with:

```powershell
.\Tools\RunWistoriaAutomationTests.ps1
```

Or compile and run the automation tests in one beta-prep pass:

```powershell
.\Tools\PrepareBetaRelease.ps1 -Build -RunAutomationTests
```

The prepared source beta archive is generated at:

```text
C:\Users\AJTOU\OneDrive\Documents\WandAndWistoria_BetaProject.zip
```

## What Is Implemented In C++

- Player health, mana, Courage, XP, inventory, quests, skills, save/load, NG+
- Sword attacks, spell casting, parry, dodge, lock-on, Courage burst Niagara hook
- Coin pickups, world loot, treasure chests, save points
- Enemy AI, boss health HUD hooks, dungeon zones, wave spawners, raid mode
- NPC companions, command wheel functions, bond/ultimate systems
- Shop vendors, dialogue NPCs, fast travel gates
- Miasma crystals and miasma hazard volumes for Floor 4
- Title-screen save slot flow through `AWistoriaTitleGameMode` and `AWistoriaTitlePlayerController`
- Optional in-map smoke checks through `AWistoriaBetaTestProbe`
- Core automation tests for save slots, inventory, skills, and quests

## Beta Test Checklist

1. Open `LV1_TestArena`.
2. Press Play.
3. Confirm player movement, attack, cast, parry, dodge, interact, and mount inputs.
4. Spawn or place `BP_WistoriaEnemy` and confirm combat damage.
5. Place a companion and confirm companion command wheel orders.
6. Place `BP_MiasmaHazardVolume` and confirm miasma suppresses Courage and magic output.
7. Use `WBP_SaveSlotManager` from a title map to create/load slots.
8. Run a raid arena test with 0 companions, then 3 companions, and compare enemy count/health.
9. Place `BP_WistoriaBetaTestProbe` in the arena, assign imported Weapon and Spell Data Tables, and confirm the on-screen smoke checks pass.

## Known Beta Assembly Work

The main C++ systems have previously compiled, and the project archive/data validation passes. If this copy has newer C++ files than your last editor build, close Unreal or disable Live Coding and run `.\Tools\PrepareBetaRelease.ps1 -Build -RunAutomationTests` before gameplay testing. Several Blueprint assets still need designer setup:

- `BP_WistoriaTitlePlayerController`: assign `WBP_SaveSlotManager`.
- `WBP_SaveSlotManager`: create visual rows using `GetSaveSlots(3)`.
- `BP_MiasmaHazardVolume`: add fog/Niagara visuals.
- `BP_MiasmaCrystalNode`: link cleanse events to hazard volumes.
- `NS_Will_CourageAuraBlast`: build the Niagara system from the project doc.
- Title screen map: set `AWistoriaTitleGameMode` or a Blueprint child as override.
- `BP_WistoriaBetaTestProbe`: create a Blueprint child and assign the imported Weapon and Spell Data Tables for row-count checks.

## Packaging

After editor playtesting works:

```powershell
.\Tools\PackageWistoriaWindows.ps1 -OutputDirectory "C:\WistoriaRPG_Builds"
```
