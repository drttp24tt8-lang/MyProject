# Niagara: Will Ultimate Aura Blast

This effect is designed for `AWistoriaCharacter::CourageBurstEffect`. Assign the finished Niagara System to `BP_WistoriaCharacter` and set `CourageBurstAttachSocketName` to a torso socket such as `spine_03`. If that socket does not exist, the C++ fallback spawns the effect at the actor location.

## System Asset

Create a Niagara System named `NS_Will_CourageAuraBlast`.

Recommended user parameters:

- `User.AuraColor`: Linear Color, default electric cyan-white.
- `User.BurstIntensity`: Float, default `1.0`.
- `User.Duration`: Float, default `1.25`.
- `User.Radius`: Float, default `180.0`.

## Emitter Stack

```text
Ultimate Active
├── Emitter 1: Core Sword Burst Ribbon (Initial Strike)
├── Emitter 2: Ground Crack Particle Grid (Radial Expansion)
└── Emitter 3: Character Aura Glow Mesh (Skeletal Attachment)
```

## Emitter 1: Core Sword Burst Ribbon

Purpose: The first high-speed sword/body ignition streak when Will enters Wis Acquisition.

- Template: Ribbon or CPU Ribbon.
- Spawn: Burst Instantaneous, `1` particle for the hero streak, or `3` if you want afterimages.
- Lifetime: random `0.18` to `0.35`.
- Add Velocity: use the character forward axis, scaled to `4000`.
- Ribbon Width: starts `26`, shrinks to `0`.
- Color: neon cyan/white; use HDR intensity around `30` to drive strong bloom.
- Add Curl Noise Force: strength `300` to `500`, frequency `6`.
- Renderer Material: additive ribbon material, no depth fade or a very soft depth fade.

## Emitter 2: Ground Crack Particle Grid

Purpose: A flat radial shockwave that looks like cracks and lightning sprites bursting along the dungeon floor.

- Template: Fountain or standard Sprite emitter.
- Spawn: Burst Instantaneous, `150` particles.
- Initialize Particle: lifetime random `0.2` to `0.4`.
- Shape Location: Sphere Location, radius `50`.
- Flattening: keep Z distribution at `0` so particles stay pinned to the floor plane.
- Velocity: Velocity from Point centered on the emitter root, speed `1500`.
- Color: cyan-white core with a faint electric-blue edge.
- Scale Sprite Size: starts `10` to `26`, fades/shrinks quickly.

## Emitter 3: Character Aura Glow Mesh

Purpose: A persistent body glow that wraps the character mesh in raw physical energy while Will runs and attacks.

- Template: Mesh Reproduction System or Skeletal Mesh Location based emitter.
- Skeletal Mesh Source: the player's active body mesh.
- Spawn Rate: `1000` particles per second for the active burst window.
- Lifetime: `0.15` to `0.3`, continuously replenished.
- Material: additive particle material with scrolling noise in emissive.
- Color: `User.AuraColor`, with intensity driven by `User.BurstIntensity`.
- Attachment: the C++ effect is spawned attached to `CourageBurstAttachSocketName` when that socket exists.

## Optional Emitter 4: Ground Shock Ring

Purpose: A readable circular impact wave for boss arenas.

- Template: Sprite or flat Mesh Ring.
- Spawn: Burst Instantaneous, `1` to `3` rings.
- Lifetime: `0.6`.
- Initial Size: `40`.
- Scale Over Life: expands to `700` to `1000`.
- Color: pale blue-white, alpha fades to `0`.

## Blueprint Hookup

1. Create `NS_Will_CourageAuraBlast`.
2. Open `BP_WistoriaCharacter`.
3. Assign `NS_Will_CourageAuraBlast` to `CourageBurstEffect`.
4. Set `CourageBurstAttachSocketName` to a socket on the torso or root mesh.
5. In Play mode, set combat style to `Will Serfort Style`, build Courage to `80`, then trigger `CastActiveSpell`.

Expected result: Courage is reduced by `80`, the Niagara aura burst spawns, `OnSpellCast` fires with Lightning, and the normal spell projectile still launches for gameplay damage.

## Polish Pass

- Add a short camera shake in Blueprint from `OnSpellCast` when Element is Lightning and combat style is sword.
- Add a bass hit or sword-ring sound on the first frame.
- Fade battle music up through `AWistoriaAudioManager` if this happens during the Frost Sentinel fight.
- Use this effect as the visual cue for the Mission 1 line: "use the Courage burst to shatter its core."
