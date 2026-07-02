import unreal

MAP_PATH = "/Game/Maps/Lvl_CombatTest"
PROJECT_NAME = "MyProject"

ROOT_FOLDERS = [
    "/Game/Maps",
    "/Game/Blueprints",
    "/Game/Characters",
    "/Game/Enemies",
    "/Game/Bosses",
    "/Game/UI",
    "/Game/FX",
    "/Game/Audio",
    "/Game/Materials",
    "/Game/Items",
    "/Game/Quests",
    "/Game/DataAssets",
    "/Game/Dungeons",
    "/Game/Companions",
]

BLUEPRINTS = [
    ("BP_WistoriaGameMode", "/Game/Blueprints", f"/Script/{PROJECT_NAME}.WistoriaGameMode"),
    ("BP_WistoriaPlayerController", "/Game/Blueprints", f"/Script/{PROJECT_NAME}.WistoriaPlayerController"),
    ("BP_WistoriaCharacter", "/Game/Characters", f"/Script/{PROJECT_NAME}.WistoriaCharacter"),
    ("BP_WistoriaEnemy", "/Game/Enemies", f"/Script/{PROJECT_NAME}.WistoriaEnemy"),
    ("BP_DungeonBoss", "/Game/Bosses", f"/Script/{PROJECT_NAME}.WistoriaBossEnemy"),
    ("BP_DungeonZone", "/Game/Dungeons", f"/Script/{PROJECT_NAME}.DungeonZone"),
    ("BP_DungeonWaveSpawner", "/Game/Dungeons", f"/Script/{PROJECT_NAME}.DungeonWaveSpawner"),
    ("BP_SavePoint", "/Game/Blueprints", f"/Script/{PROJECT_NAME}.SavePoint"),
    ("BP_TreasureChest", "/Game/Items", f"/Script/{PROJECT_NAME}.TreasureChest"),
    ("BP_WorldLootPickup", "/Game/Items", f"/Script/{PROJECT_NAME}.WorldLootPickup"),
    ("BP_CompanionMage", "/Game/Companions", f"/Script/{PROJECT_NAME}.WistoriaCompanionCharacter"),
    ("BP_NPCProfessor", "/Game/Blueprints", f"/Script/{PROJECT_NAME}.DialogueNPC"),
    ("BP_FastTravelGate", "/Game/Blueprints", f"/Script/{PROJECT_NAME}.FastTravelGate"),
]

def log(msg):
    unreal.log(f"[Wistoria Toolkit] {msg}")

def ensure_folders():
    for folder in ROOT_FOLDERS:
        if not unreal.EditorAssetLibrary.does_directory_exist(folder):
            unreal.EditorAssetLibrary.make_directory(folder)
            log(f"Created folder: {folder}")

def load_class(path):
    cls = unreal.load_class(None, path)
    if not cls:
        log(f"Missing class: {path}")
    return cls

def create_blueprint(asset_name, path, parent_class_path):
    asset_path = f"{path}/{asset_name}"

    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        log(f"Exists: {asset_path}")
        return unreal.load_asset(asset_path)

    parent_class = load_class(parent_class_path)
    if not parent_class:
        return None

    factory = unreal.BlueprintFactory()
    factory.set_editor_property("ParentClass", parent_class)

    asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name,
        path,
        unreal.Blueprint,
        factory
    )

    if asset:
        log(f"Created Blueprint: {asset_path}")

    return asset

def build_core_blueprints():
    for name, path, parent in BLUEPRINTS:
        create_blueprint(name, path, parent)

def load_bp_class(asset_path):
    asset = unreal.load_asset(asset_path)
    if not asset:
        log(f"Missing asset: {asset_path}")
        return None

    generated_class = asset.generated_class()
    if not generated_class:
        log(f"No generated class for: {asset_path}")
        return None

    return generated_class

def create_combat_test_level():
    world = unreal.EditorLevelLibrary.new_level(MAP_PATH)

    if not world:
        log("Failed to create/open combat test level.")
        return

    log(f"Created/opened level: {MAP_PATH}")

    player_start_class = unreal.load_class(None, "/Script/Engine.PlayerStart")
    navmesh_class = unreal.load_class(None, "/Script/NavigationSystem.NavMeshBoundsVolume")

    enemy_class = load_bp_class("/Game/Enemies/BP_WistoriaEnemy")
    save_point_class = load_bp_class("/Game/Blueprints/BP_SavePoint")
    chest_class = load_bp_class("/Game/Items/BP_TreasureChest")

    unreal.EditorLevelLibrary.spawn_actor_from_class(
        player_start_class,
        unreal.Vector(0, 0, 120),
        unreal.Rotator(0, 0, 0)
    )

    if enemy_class:
        unreal.EditorLevelLibrary.spawn_actor_from_class(
            enemy_class,
            unreal.Vector(600, 0, 120),
            unreal.Rotator(0, 180, 0)
        )

    if save_point_class:
        unreal.EditorLevelLibrary.spawn_actor_from_class(
            save_point_class,
            unreal.Vector(-350, 250, 100),
            unreal.Rotator(0, 0, 0)
        )

    if chest_class:
        unreal.EditorLevelLibrary.spawn_actor_from_class(
            chest_class,
            unreal.Vector(350, 250, 100),
            unreal.Rotator(0, 0, 0)
        )

    nav = unreal.EditorLevelLibrary.spawn_actor_from_class(
        navmesh_class,
        unreal.Vector(0, 0, 0),
        unreal.Rotator(0, 0, 0)
    )

    if nav:
        nav.set_actor_scale3d(unreal.Vector(20, 20, 3))

    unreal.EditorLevelLibrary.save_current_level()
    log("Combat test level built and saved.")

def save_all():
    unreal.EditorAssetLibrary.save_directory("/Game", only_if_is_dirty=True, recursive=True)

def run():
    log("Starting Wistoria Developer Toolkit...")
    ensure_folders()
    build_core_blueprints()
    save_all()
    create_combat_test_level()
    save_all()
    log("Toolkit complete.")

run()