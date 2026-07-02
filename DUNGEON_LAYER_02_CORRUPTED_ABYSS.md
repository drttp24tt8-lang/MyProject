# Dungeon Layer 02: The Corrupted Abyss

This layer follows Mission 01 and shifts the dungeon from ice trials into corruption control. The player is still single-player, but AI companions become important because they can be ordered to cleanse miasma crystals while the player holds off enemies.

## Core Theme

- Environment: obsidian caverns, violet crystal veins, deep floor rifts, low purple fog.
- Primary hazard: Corruption Miasma.
- Player pressure: miasma suppresses magic output and stops Will-style Courage gain.
- Companion role: mage companions cleanse miasma crystal nodes through the existing command wheel `InteractWithTarget` order.

## Required Actors

- `BP_WistoriaCharacter`
- `BP_CompanionMage`
- `BP_MiasmaHazardVolume` from `AMiasmaHazardVolume`
- `BP_MiasmaCrystalNode` from `AMiasmaCrystalNode`
- `BP_DungeonZone`
- `BP_DungeonWaveSpawner`
- `BP_WistoriaEnemy` variants using Miasma or Dark visual effects
- `BP_WistoriaPuzzleSwitch`
- `BP_WistoriaDungeonPuzzleGate`

## Miasma Rules

`AMiasmaHazardVolume` applies `EWistoriaElement::Miasma` to actors with `UWistoriaStatusComponent`.

On the player:

- sword attacks cannot build Courage while miasma is active
- mana spell costs increase through existing combat logic
- magic output is reduced to `70%`

Recommended hazard settings:

- small room pockets: `TimedMiasmaDuration = 4`, `bClearMiasmaOnExit = false`
- persistent fog rooms: `TimedMiasmaDuration = 0`, `bClearMiasmaOnExit = true`
- crystal-controlled rooms: set hazard enabled at start, then call `SetMiasmaEnabled(false)` from the crystal cleanse event

## Crystal Cleanse Flow

1. Place `BP_MiasmaCrystalNode` near the center of the corrupted room.
2. Place one or more `BP_MiasmaHazardVolume` actors covering the fog pockets.
3. In the crystal Blueprint, bind `OnNodeCleansed`.
4. On cleanse, call `SetMiasmaEnabled(false)` on the linked hazard volumes.
5. Use the companion command wheel to target the crystal and call `CommandInteractWithTarget`.

## Encounter Loop

1. Player enters a miasma chamber.
2. Courage gain and magic output are suppressed.
3. A wave spawner starts enemies.
4. Player commands Elfi or another mage companion to cleanse the crystal.
5. Companion reaches the node and interacts.
6. Fog shuts down, resources recover, and the player can burst down the remaining enemies.

## Visual Direction

- Fog: Exponential Height Fog with purple tint.
- Crystals: emissive violet material with slow pulse.
- Hazards: Niagara ground mist with low alpha and drifting sparks.
- Cleansing: white-blue vertical beam, then violet fog collapse inward.

## Test Checklist

- Entering the hazard applies the miasma status.
- Will-style primary attacks do not build Courage while miasma is active.
- Mage spells deal reduced projectile damage while miasma is active.
- Leaving a clear-on-exit hazard removes miasma.
- Cleansing the crystal disables linked hazard volumes.
- Companion `InteractWithTarget` can cleanse the crystal without the player touching it.
- Dungeon waves remain balanced with 0, 1, and 3 companions.
