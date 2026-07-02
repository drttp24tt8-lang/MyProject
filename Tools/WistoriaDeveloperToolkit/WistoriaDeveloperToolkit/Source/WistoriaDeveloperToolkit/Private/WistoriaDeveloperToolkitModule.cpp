#include "WistoriaDeveloperToolkitModule.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Editor.h"
#include "EditorAssetLibrary.h"
#include "EditorLevelLibrary.h"
#include "Engine/DirectionalLight.h"
#include "Engine/Level.h"
#include "Engine/PlayerStart.h"
#include "Engine/StaticMeshActor.h"
#include "Factories/BlueprintFactory.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "LevelEditor.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "NavigationSystem.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "FWistoriaDeveloperToolkitModule"

IMPLEMENT_MODULE(FWistoriaDeveloperToolkitModule, WistoriaDeveloperToolkit)

static const FString ProjectModuleName = TEXT("MyProject");

void FWistoriaDeveloperToolkitModule::StartupModule()
{
    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FWistoriaDeveloperToolkitModule::RegisterMenus));
}

void FWistoriaDeveloperToolkitModule::ShutdownModule()
{
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
}

void FWistoriaDeveloperToolkitModule::RegisterMenus()
{
    FToolMenuOwnerScoped OwnerScoped(this);

    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
    FToolMenuSection& Section = Menu->AddSection("WistoriaToolkit", LOCTEXT("WistoriaToolkit", "Wistoria"));

    Section.AddMenuEntry(
        "WistoriaValidateProject",
        LOCTEXT("ValidateProject", "Validate Project"),
        LOCTEXT("ValidateProjectTooltip", "Checks required Wand and Wistoria project classes, folders, and core assets."),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FWistoriaDeveloperToolkitModule::ValidateProject)));

    Section.AddMenuEntry(
        "WistoriaCreateFolders",
        LOCTEXT("CreateFolders", "Create Folder Structure"),
        LOCTEXT("CreateFoldersTooltip", "Creates the standard Wand and Wistoria Content folders."),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FWistoriaDeveloperToolkitModule::CreateFolderStructure)));

    Section.AddMenuEntry(
        "WistoriaBuildCoreBlueprints",
        LOCTEXT("BuildCoreBlueprints", "Build Core Blueprints"),
        LOCTEXT("BuildCoreBlueprintsTooltip", "Creates Blueprint children for GameMode, PlayerController, Character, Enemy, Boss, Dungeon, Items, NPCs, and Companions."),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FWistoriaDeveloperToolkitModule::BuildCoreBlueprints)));

    Section.AddMenuEntry(
        "WistoriaGenerateCombatTestArena",
        LOCTEXT("GenerateCombatTestArena", "Generate Combat Test Arena"),
        LOCTEXT("GenerateCombatTestArenaTooltip", "Creates a simple local combat test arena with floor, lights, PlayerStart, enemy, chest, save point, and NavMeshBoundsVolume."),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FWistoriaDeveloperToolkitModule::GenerateCombatTestArena)));

    Section.AddMenuEntry(
        "WistoriaFixRedirectors",
        LOCTEXT("FixRedirectors", "Fix Redirectors"),
        LOCTEXT("FixRedirectorsTooltip", "Fixes redirectors under /Game."),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FWistoriaDeveloperToolkitModule::FixRedirectors)));
}

void FWistoriaDeveloperToolkitModule::Log(const FString& Message) const
{
    UE_LOG(LogTemp, Display, TEXT("[Wistoria Toolkit] %s"), *Message);
}

void FWistoriaDeveloperToolkitModule::Warn(const FString& Message) const
{
    UE_LOG(LogTemp, Warning, TEXT("[Wistoria Toolkit] %s"), *Message);
}

UClass* FWistoriaDeveloperToolkitModule::LoadWistoriaClass(const FString& ClassPath) const
{
    UClass* LoadedClass = LoadObject<UClass>(nullptr, *ClassPath);
    if (!LoadedClass)
    {
        Warn(FString::Printf(TEXT("Missing class: %s"), *ClassPath));
    }
    return LoadedClass;
}

void FWistoriaDeveloperToolkitModule::CreateFolderStructure()
{
    const TArray<FString> Folders = {
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
        "/Game/Companions"
    };

    for (const FString& Folder : Folders)
    {
        if (!UEditorAssetLibrary::DoesDirectoryExist(Folder))
        {
            UEditorAssetLibrary::MakeDirectory(Folder);
            Log(FString::Printf(TEXT("Created folder: %s"), *Folder));
        }
    }

    UEditorAssetLibrary::SaveDirectory("/Game", true, true);
    FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("FoldersCreated", "Wistoria folder structure created/verified."));
}

UObject* FWistoriaDeveloperToolkitModule::CreateBlueprintAsset(const FString& AssetName, const FString& PackagePath, const FString& ParentClassPath) const
{
    const FString FullAssetPath = PackagePath / AssetName;
    if (UEditorAssetLibrary::DoesAssetExist(FullAssetPath))
    {
        Log(FString::Printf(TEXT("Exists: %s"), *FullAssetPath));
        return UEditorAssetLibrary::LoadAsset(FullAssetPath);
    }

    UClass* ParentClass = LoadWistoriaClass(ParentClassPath);
    if (!ParentClass)
    {
        return nullptr;
    }

    UBlueprintFactory* Factory = NewObject<UBlueprintFactory>();
    Factory->ParentClass = ParentClass;

    FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
    UObject* CreatedAsset = AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, UBlueprint::StaticClass(), Factory);

    if (CreatedAsset)
    {
        Log(FString::Printf(TEXT("Created Blueprint: %s"), *FullAssetPath));
        UEditorAssetLibrary::SaveLoadedAsset(CreatedAsset, false);
    }
    else
    {
        Warn(FString::Printf(TEXT("Failed to create Blueprint: %s"), *FullAssetPath));
    }

    return CreatedAsset;
}

void FWistoriaDeveloperToolkitModule::BuildCoreBlueprints()
{
    CreateFolderStructure();

    struct FBlueprintSpec
    {
        FString Name;
        FString Path;
        FString Parent;
    };

    const TArray<FBlueprintSpec> Specs = {
        {"BP_WistoriaGameMode", "/Game/Blueprints", "/Script/MyProject.WistoriaGameMode"},
        {"BP_WistoriaPlayerController", "/Game/Blueprints", "/Script/MyProject.WistoriaPlayerController"},
        {"BP_WistoriaCharacter", "/Game/Characters", "/Script/MyProject.WistoriaCharacter"},
        {"BP_WistoriaEnemy", "/Game/Enemies", "/Script/MyProject.WistoriaEnemy"},
        {"BP_DungeonBoss", "/Game/Bosses", "/Script/MyProject.WistoriaBossEnemy"},
        {"BP_DungeonZone", "/Game/Dungeons", "/Script/MyProject.DungeonZone"},
        {"BP_DungeonWaveSpawner", "/Game/Dungeons", "/Script/MyProject.DungeonWaveSpawner"},
        {"BP_SavePoint", "/Game/Blueprints", "/Script/MyProject.SavePoint"},
        {"BP_TreasureChest", "/Game/Items", "/Script/MyProject.TreasureChest"},
        {"BP_WorldLootPickup", "/Game/Items", "/Script/MyProject.WorldLootPickup"},
        {"BP_CompanionMage", "/Game/Companions", "/Script/MyProject.WistoriaCompanionCharacter"},
        {"BP_NPCProfessor", "/Game/Blueprints", "/Script/MyProject.DialogueNPC"},
        {"BP_FastTravelGate", "/Game/Blueprints", "/Script/MyProject.FastTravelGate"}
    };

    for (const FBlueprintSpec& Spec : Specs)
    {
        CreateBlueprintAsset(Spec.Name, Spec.Path, Spec.Parent);
    }

    UEditorAssetLibrary::SaveDirectory("/Game", true, true);
    FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("BlueprintsCreated", "Core Wistoria Blueprints created/verified."));
}

void FWistoriaDeveloperToolkitModule::ValidateProject()
{
    const TArray<FString> RequiredClasses = {
        "/Script/MyProject.WistoriaGameMode",
        "/Script/MyProject.WistoriaPlayerController",
        "/Script/MyProject.WistoriaCharacter",
        "/Script/MyProject.WistoriaEnemy",
        "/Script/MyProject.WistoriaBossEnemy",
        "/Script/MyProject.DungeonZone",
        "/Script/MyProject.DungeonWaveSpawner",
        "/Script/MyProject.SavePoint",
        "/Script/MyProject.TreasureChest",
        "/Script/MyProject.WistoriaCompanionCharacter"
    };

    int32 Missing = 0;
    for (const FString& RequiredClass : RequiredClasses)
    {
        if (!LoadWistoriaClass(RequiredClass))
        {
            Missing++;
        }
    }

    const FString Message = Missing == 0
        ? TEXT("Project validation passed. All required Wistoria C++ classes are loaded.")
        : FString::Printf(TEXT("Project validation found %d missing classes. Check Output Log."), Missing);

    FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Message));
}

static TSubclassOf<AActor> LoadBlueprintGeneratedClass(const FString& AssetPath)
{
    UObject* Asset = UEditorAssetLibrary::LoadAsset(AssetPath);
    UBlueprint* Blueprint = Cast<UBlueprint>(Asset);
    return Blueprint ? Blueprint->GeneratedClass : nullptr;
}

void FWistoriaDeveloperToolkitModule::GenerateCombatTestArena()
{
    BuildCoreBlueprints();

    UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
    if (!World)
    {
        Warn(TEXT("No editor world available."));
        return;
    }

    // Basic floor
    AStaticMeshActor* Floor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
    if (Floor)
    {
        Floor->SetActorLabel(TEXT("Wistoria_TestArena_Floor"));
        Floor->SetActorLocation(FVector(0.f, 0.f, -50.f));
        Floor->SetActorScale3D(FVector(50.f, 50.f, 1.f));
        UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
        Floor->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
        Floor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
        Floor->GetStaticMeshComponent()->SetCanEverAffectNavigation(true);
    }

    World->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), FVector(0.f, 0.f, 150.f), FRotator::ZeroRotator);
    World->SpawnActor<ADirectionalLight>(ADirectionalLight::StaticClass(), FVector(0.f, 0.f, 500.f), FRotator(-45.f, 0.f, 0.f));

    if (UClass* EnemyClass = LoadBlueprintGeneratedClass("/Game/Enemies/BP_WistoriaEnemy"))
    {
        World->SpawnActor<AActor>(EnemyClass, FVector(600.f, 0.f, 120.f), FRotator(0.f, 180.f, 0.f));
    }

    if (UClass* SavePointClass = LoadBlueprintGeneratedClass("/Game/Blueprints/BP_SavePoint"))
    {
        World->SpawnActor<AActor>(SavePointClass, FVector(-350.f, 250.f, 100.f), FRotator::ZeroRotator);
    }

    if (UClass* ChestClass = LoadBlueprintGeneratedClass("/Game/Items/BP_TreasureChest"))
    {
        World->SpawnActor<AActor>(ChestClass, FVector(350.f, 250.f, 100.f), FRotator::ZeroRotator);
    }

    UClass* NavVolumeClass = LoadObject<UClass>(nullptr, TEXT("/Script/NavigationSystem.NavMeshBoundsVolume"));
    if (NavVolumeClass)
    {
        AActor* NavVolume = World->SpawnActor<AActor>(NavVolumeClass, FVector(0.f, 0.f, 0.f), FRotator::ZeroRotator);
        if (NavVolume)
        {
            NavVolume->SetActorLabel(TEXT("Wistoria_TestArena_NavMeshBounds"));
            NavVolume->SetActorScale3D(FVector(30.f, 30.f, 5.f));
        }
    }

    UEditorAssetLibrary::SaveDirectory("/Game", true, true);
    FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("CombatArenaCreated", "Combat test arena actors created in the current level. Build Paths if navigation needs rebuilding."));
}

void FWistoriaDeveloperToolkitModule::FixRedirectors()
{
    TArray<UObject*> Redirectors;
    const TArray<FString> Paths = { "/Game" };
    UEditorAssetLibrary::FixupRedirectors(Paths);
    FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("RedirectorsFixed", "Redirectors fixed under /Game."));
}

#undef LOCTEXT_NAMESPACE
