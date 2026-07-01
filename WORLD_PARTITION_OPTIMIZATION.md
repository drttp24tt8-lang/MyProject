# World Partition Optimization

Use this pass after Mission 01 runs correctly in-editor. The goal is faster open-world loading on lower-end hardware without breaking dungeon or raid flow.

## Cell Size Pass

Start with restrained streaming cells:

- Outdoor academy fields: larger cells, around `128m` to `256m`.
- Dense academy streets or forests: medium cells, around `64m` to `128m`.
- Dungeon interiors and raid arenas: prefer separate maps or small streaming regions instead of one enormous cell.

If the player sees loading hitches while sprinting or riding a mount, increase loading range before shrinking cell size.

## Streaming Sources

Use the player as the primary streaming source.

For fast travel gates:

1. Fade screen to black.
2. Teleport the player.
3. Wait a short delay or use Blueprint streaming readiness checks.
4. Fade back in after the destination area is visible.

This hides World Partition cell activation from the player.

## HLOD Setup

Generate HLODs for:

- distant academy buildings
- forest clusters
- cliff walls
- large dungeon entrances

Avoid HLOD for:

- interactable NPCs
- loot pickups
- puzzle switches
- boss arena moving pieces
- companion actors

## Foliage

Use foliage density scaling:

- Trees: lower density in far cells.
- Grass: cull aggressively at distance.
- Magical flowers or readable plants: keep near traversal paths only.

Prefer Hierarchical Instanced Static Mesh foliage over loose placed mesh actors.

## Dungeon And Raid Maps

Keep raid arenas predictable:

- Use a dedicated raid map or isolated World Partition region.
- Keep Nav Mesh Bounds tight around combat space.
- Spawn companions and enemies after the arena cells are loaded.
- Disable unneeded outdoor foliage/landscape streaming during raid instances if the arena is underground.

## Low-End Hardware Checklist

- Test from a packaged Development build, not only PIE.
- Ride the mount at max speed across the largest open area.
- Fast travel between the two farthest gates.
- Enter Mission 01 dungeon from the academy map.
- Trigger the Frost Sentinel boss reveal and Courage burst Niagara effect.
- Watch for shader stalls, missing HLODs, Nav Mesh gaps, and late-spawning puzzle actors.

## Practical Defaults For This Prototype

- Keep the title screen and Mission 01 dungeon small and separate.
- Use World Partition first for the academy/open field map.
- Build HLODs after static art placement is stable.
- Treat the raid arena as a controlled performance benchmark: if the raid is smooth, then expand the open world around it.
