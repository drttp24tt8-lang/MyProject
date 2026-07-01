# Mission 01: The First Descent

## Purpose

This first mission introduces the core single-player loop:

1. Accept a professor quest at Regarden Academy.
2. Enter a dungeon sector through `ADungeonZone`.
3. Fight a small monster wave.
4. Recruit or spawn an AI companion.
5. Solve a two-switch companion puzzle.
6. Fight an early elite boss.
7. Collect a Glacial Core quest item.
8. Return for XP, gold, affinity, and academy rank progress.

## Required Actors

- `BP_WistoriaCharacter`
- `BP_WistoriaPlayerController`
- `BP_NPCProfessor`
- `BP_CompanionMage`
- `BP_DungeonZone`
- `BP_WistoriaEnemy`
- `BP_DungeonBoss`
- `BP_WistoriaPuzzleSwitch` x2
- `BP_WistoriaDungeonPuzzleGate`
- `BP_TreasureChest` or `BP_WorldLootPickup` for the Glacial Core
- `BP_SavePoint`
- `BP_WistoriaHUD`
- `WBP_Dialogue`
- `WBP_RaidHUD` or a small dungeon objective widget

## Quest Data

Create a `UWistoriaQuestDataAsset` named `DA_Quest_FirstDescent`.

Recommended fields:

- `QuestId`: `Quest_FirstDescent`
- `QuestName`: `The First Descent`
- `State`: `Active`
- Objective 1:
  - `ObjectiveId`: `EnterFloor3`
  - `Description`: `Enter Dungeon Sub-Floor 3`
  - `RequiredCount`: `1`
- Objective 2:
  - `ObjectiveId`: `DungeonBeast`
  - `Description`: `Defeat dungeon beasts`
  - `RequiredCount`: `3`
- Objective 3:
  - `ObjectiveId`: `FrostSentinel`
  - `Description`: `Defeat the Frost Sentinel`
  - `RequiredCount`: `1`
- Objective 4:
  - `ObjectiveId`: `GlacialCore`
  - `Description`: `Retrieve the Glacial Core`
  - `RequiredCount`: `1`
- `RewardXP`: `250`

## Level Flow

### 1. Regarden Academy Courtyard

Place `BP_NPCProfessor` near the player start. Assign `DA_Quest_FirstDescent` as the offered quest.

On interaction:

- Open `WBP_Dialogue`.
- Call `StartQuest`.
- Set the HUD tracker to `The First Descent`.

### 2. Dungeon Entrance

Place `ACompanionPartySpawner` near the dungeon door.

Settings:

- `MaxCompanionsToSpawn`: `1`
- `CompanionClasses`: `BP_CompanionMage`

Set the companion's `CompanionId` to `Elfi_Companion` so raid and mission affinity can persist.

### 3. Sub-Floor 3 Zone

Place `BP_DungeonZone` at the first dungeon room.

Settings:

- `DungeonFloorLevel`: `3`
- `MonstersToSpawn`: `3`
- `MonsterPool`: `BP_WistoriaEnemy`
- `bTriggerOnce`: `true`

Set the enemy `EnemyId` to `DungeonBeast` so quest kill tracking advances.

### 4. Dual Lever Gate

Place two `BP_WistoriaPuzzleSwitch` actors on opposite sides of the room.

Recommended setup:

- Switch A `SwitchId`: `Floor3LeverA`
- Switch B `SwitchId`: `Floor3LeverB`
- Both `LinkedGate`: the same `BP_WistoriaDungeonPuzzleGate`
- Gate `RequiredActiveSwitches`: `2`
- Switch `ActiveDuration`: `8`
- Gate `bStayOpenOnceSolved`: `true`

Player action:

- Use the command wheel Activate Object button on Switch A.
- Companion runs to Switch A and interacts.
- Player activates Switch B.
- Gate opens.

### 5. Frost Sentinel Arena

Place `BP_DungeonBoss` behind the puzzle gate.

Recommended boss values:

- `EnemyId`: `FrostSentinel`
- `MaxHealth`: `650`
- `BaseDamage`: `25`
- Element: `Ice`
- Enable procedural weapon drops:
  - `bRollProceduralWeaponDrop`: `true`
  - `ProceduralWeaponDropChance`: `1.0`
  - `LegendaryDropRate`: `0.10`
  - `EpicDropRate`: `0.35`

Combat goals:

- Player parries heavy attacks to build Courage.
- Companion uses freeze/support spells through Blueprint events.
- Player uses `CastActiveSpell` when Courage reaches `80`.

### 6. Glacial Core Reward

After the boss dies, spawn or reveal a `BP_WorldLootPickup`.

Item:

- `ItemId`: `GlacialCore`
- `DisplayName`: `Glacial Core`
- `ItemType`: `KeyItem`
- `Quantity`: `1`

When picked up, advance quest objective `GlacialCore`.

## Dialogue Script

### Scene 1: Courtyard Quest Offer

**Professor Edward**

Serfort. Look at your ledger. Zero magical credits. A magicless boy standing among the chosen scholars of the Magia Vander. You are an embarrassment to this academy's crest.

**Will**

I can fight, Professor. Send me into the lower dungeon grids. I'll prove my sword can keep pace with any wand.

**Professor Edward**

Then prove it on the third sub-floor. A rare Glacial Beast has nested near the barrier gates. Retrieve its core, or pack your bags and leave Regarden by sunset.

Quest accepted: `The First Descent`

### Scene 2: Dungeon Entrance

**Elfi**

You were going to descend alone?

**Will**

I did not want anyone else losing credits because of me.

**Elfi**

Then do not fall behind. I will handle the frost seals. You handle whatever tries to eat us.

Companion joined: `Elfi_Companion`

### Scene 3: Puzzle Gate

**Elfi**

Floating staff visual, looking at the vault door.

Will, look at the runic seals on this door. It's a dual-lock system. You won't be able to force it open with physical muscle alone.

**Will**

We pull them at the exact same time. Elfi, take the left switch. I'll sprint to the right.

Player action: Open command wheel, choose Activate Object, target Switch A.

AI action: Elfi breaks off, moves to Switch A, interacts, and flips its active flag.

Player action: Move to Switch B and press Interact.

Gate event: Both switch flags evaluate true, the vault gate opens, and the boss arena trigger arms.

**Elfi**

It worked! Wait... Will, get back! The temperature in the arena is plummeting!

### Scene 4: Boss Reveal

Cinematic trigger: Camera shifts lock-on toward the cavern roof. A massive ice beast crashes into the arena.

Boss health popup: `Frost Sentinel - Floor 10 Trespasser`

Audio layer crossfade: Battle music `CombatIntensity` ramps to `1.0`.

**Elfi**

A Floor 10 monster up here?! My ice spells won't pierce its thick shell, but I can cast a Glacial Freeze to slow its tracking speed!

**Will**

Perfect! Keep it slowed! I'll stand right in its pathing vector, parry its ground slam, and use the Courage burst to shatter its core!

**Elfi**

Its armor is ice-bound. Break its rhythm. Parry the heavy strikes, then use that impossible speed of yours.

Boss started: `FrostSentinel`

Combat tutorial prompt: Parry heavy attacks to build Courage, then cast with `80` Courage to trigger Will's ultimate aura burst.

### Scene 5: Mission Clear

**Professor Edward**

You returned.

**Will**

With the core.

**Professor Edward**

Hmph. Brute force wearing discipline's clothing. Still, a result is a result. Your credits will be updated.

Mission complete: grant XP, gold, affinity, and academy rank score.

## Cinematic Event Hooks

- Gate opened: call the boss arena trigger, start a camera look-at or Sequencer shot, and set combat music intensity to `1.0`.
- Boss spawn: create or reveal `BP_DungeonBoss`, call `SetBoss` on `WBP_BossHUD`, and show the boss intro title.
- Elfi support: command companion support Blueprint logic to apply Freeze or slow effects to `FrostSentinel`.
- Will finisher: assign `NS_Will_CourageAuraBlast` to `BP_WistoriaCharacter.CourageBurstEffect`; `CastActiveSpell` will spawn it when the sword style spends `80` Courage.


## Blueprint Hooks

- Professor interaction: `StartQuest(DA_Quest_FirstDescent.MakeQuestRecord())`
- Dungeon zone entered: advance `EnterFloor3`
- Enemy defeated: `NotifyEnemyKilled(DungeonBeast)`
- Boss defeated: `NotifyEnemyKilled(FrostSentinel)`
- Glacial Core pickup: `AdvanceQuestObjective(Quest_FirstDescent, GlacialCore, 1)`
- Mission clear:
  - `AddXP(250)`
  - inventory `AddGold(150)`
  - `AwardAffinityToParty(25)`
  - `AddAcademyRankScore(100)`

## Test Checklist

- Player can attack and cast with fallback inputs before Enhanced Input assets are assigned.
- `bShowDebugResourceOverlay` displays resource changes.
- Dungeon zone spawns three enemies around the entering player.
- Quest kill count advances from `DungeonBeast`.
- Companion command wheel can send the companion to a lever.
- Gate opens when both switches are active.
- Boss death advances `FrostSentinel`.
- Glacial Core pickup completes the final objective.
- Returning to Professor Edward gives rewards.
