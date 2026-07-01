# Packaging And Keybinding Setup

This is the final production pass after the C++ project compiles locally.

## Current Packaging Blocker

UnrealBuildTool currently stops before compiling because the Windows SDK is missing:

```text
Platform Win64 is not a valid platform to build. SDK validation failed:
  Sdk: not found. Required version 10.0.19041.0.
```

Install these in Visual Studio Installer before packaging:

- Desktop development with C++
- MSVC C++ toolset
- Windows 10/11 SDK `10.0.19041.0` or newer
- Visual Studio Tools for Unreal Engine

After installing, close Unreal and Visual Studio, then regenerate project files from `MyProject.uproject`.

## Packaging Settings

Open Unreal Engine:

1. Go to `Edit > Project Settings > Maps & Modes`.
2. Set `Editor Startup Map` to the title screen map once it exists.
3. Set `Game Default Map` to the same title screen map.
4. Go to `Project > Packaging`.
5. Set `Project Build Configuration` to `Shipping`.
6. Enable `For Distribution`.
7. Enable `Full Rebuild`.
8. Enable pak file/compression options if the project size becomes large.
9. Add every required map to `List of maps to include in a packaged build`.

Recommended maps:

- Title screen map with `WBP_SaveSlotManager`.
- Regarden Academy courtyard.
- Mission 01 dungeon map.
- Raid arena map.

## Package Command

From the editor:

1. Open `Platforms > Windows`.
2. Set binary configuration to `Shipping`.
3. Select `Package Project`.
4. Use a clean output directory such as `C:\WistoriaRPG_Builds`.

Command-line packaging can be added after the SDK is installed, but editor packaging is the best first pass because it shows missing assets and maps clearly.

## Save Slot Title Screen

Create `WBP_SaveSlotManager` from `UWistoriaSaveSlotManagerWidget`.

On Construct:

1. Call `GetSaveSlots(3)`.
2. Create one row per `FWistoriaSaveSlotInfo`.
3. Bind row text to `SlotLabel`.
4. If `bHasSave` is false, show `New Game`.
5. If `bHasSave` is true, show `Load Game`.

Button behavior:

- New Game: `StartNewGameInSlot(SlotIndex)`, then `OpenGameplayLevel`.
- Load Game: `LoadGameInSlot(SlotIndex)`, then `OpenGameplayLevel`.
- Delete: `DeleteSaveSlot(SlotIndex)`, then refresh the slot rows.

Set `GameplayLevelName` on the widget to the first playable map, not the title map.

## Keybinding Menu

Create `WBP_KeyRebindMenu`.

Recommended rows:

- Move Forward / Back
- Move Right / Left
- Jump
- Primary Attack
- Cast Spell
- Interact
- Lock On
- Parry
- Dodge
- Command Wheel
- Mount
- Pause

Enhanced Input asset setup:

1. Open each `IA_` action used by `BP_WistoriaCharacter`.
2. Enable player mappable settings.
3. Give each action a clear display name, for example `Primary Attack / Strike`.
4. Use display groups such as `Movement`, `Combat`, `Companion`, and `Menus`.

Widget row setup:

1. Add a text label for the action display name.
2. Add an `Input Key Selector` widget.
3. On `On Key Selected`, remove the old mapping from the player's mapping context and add the new key mapping.
4. Save the selected key to a player settings save object or Enhanced Input user settings, depending on the engine version available in your editor.

Blueprint node flow for UE versions with Enhanced Input User Settings:

```text
Input Key Selector: On Key Selected
-> Get Owning Local Player
-> Get Enhanced Input Local Player Subsystem
-> Get User Settings
-> Map Player Key
   - Input Action: IA_CastSpell, IA_Attack, IA_Parry, etc.
   - New Key: Selected Key
-> Apply Settings
-> Print String: Controls updated successfully!
```

The subsystem call starts from `Get Enhanced Input Local Player Subsystem`, then uses its `Get User Settings` object.
The mapping call should target the same player-mappable action configured on the `IA_` asset.
This lets players update controls and return to a raid encounter without restarting the level.

## Launch Validation Matrix

Use this order for final checks:

1. Shipping Cook Verification: package a build after the SDK/toolchain blocker is fixed.
2. Stand-Alone Executable Extraction: launch the packaged `.exe` outside Unreal Editor.
3. Input Mapping Context Overrides: rebind combat keys in `WBP_KeyRebindMenu`.
4. UserSettings Disk Commit: close and relaunch the packaged build, then confirm the custom key remains mapped.
5. Save Slot Persistence: create and load `WistoriaSlot_1`, `WistoriaSlot_2`, and `WistoriaSlot_3` from Windows AppData saves.
6. Live Encounter Rebind: open options during a boss or raid wave, change one key, return to combat, and confirm input updates instantly.

## Testing Keybinds

Before packaging:

1. Rebind Attack to a keyboard key.
2. Confirm the character still calls `ExecutePrimaryAttack`.
3. Rebind Cast Spell.
4. Build Courage to `80` and confirm the spell key triggers `CourageBurstEffect`.
5. Restart PIE or reload the map and confirm bindings persist.

Keep the existing `DefaultInput.ini` fallback bindings until the Enhanced Input rebinding UI has been tested in a packaged build.
