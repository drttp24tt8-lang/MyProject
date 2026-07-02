#include "WistoriaDeveloperToolkit.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Factories/BlueprintFactory.h"
#include "FileHelpers.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GameFramework/PlayerStart.h"
#include "HAL/FileManager.h"
#include "IAssetTools.h"
#include "LevelEditor.h"
#include "Misc/MessageDialog.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "NavigationSystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "ToolMenus.h"
#include "Engine/DirectionalLight.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/SkyLight.h"
#include "UObject/ConstructorHelpers.h"

#define LOCTEXT_NAMESPACE "FWistoriaDeveloperToolkitModule"

void FWistoriaDeveloperToolkitModule::StartupModule()
{
    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FWistoriaDeveloperToolkitModule::RegisterMenus));
}

void FWistoriaDeveloperToolkitModule::ShutdownModule()
{
    if (UToolMenus::IsToolMenuUIEnabled())
    {
        UnregisterMenus();
    }

    UToolMenus::UnRegisterStartupCallback(this);
}

void FWistoriaDeveloperToolkitModule::RegisterMenus()
{
    UToolMenu* ToolsMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
    if (!ToolsMenu)
    {
        return;
    }

    FToolMenuSection& Section = ToolsMenu->AddSection("WistoriaDeveloperToolkit", LOCTEXT("WistoriaSection", "Wistoria"));

    Section.AddMenuEntry(
        "WistoriaValidateProject",
        LOCTEXT("ValidateProject", "Wistoria: Validate Project"),
        LOCTEXT("ValidateProjectTooltip", "Check the project for key Wand and Wistoria folders and classes."),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FWistoriaDeveloperToolkitModule::ValidateProject))
    );

    Section.AddMenuEntry(
        "WistoriaCreateFolders",
        LOCTEXT("CreateFolders", "Wistoria: Create Folder Structure"),
        LOCTEXT("CreateFoldersTooltip", "Create the core /Game folders for Wand and Wistoria."),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FWistoriaDeveloperToolkitModule::CreateFolderStructure))
    );

    Section.AddMenuEntry(
        "WistoriaBuildBlueprints",
        LOCTEXT("BuildCoreBlueprints", "Wistoria: Build Core Blueprints"),
        LOCTEXT("BuildCoreBlueprintsTooltip", "Create Blueprint children from the core C++ gameplay classes."),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FWistoriaDeveloperToolkitModule::BuildCoreBlueprints))
    );

    Section.AddMenuEntry(
        "WistoriaGenerateArena",
        LOCTEXT("GenerateArena", "Wistoria: Generate Combat Test Arena"),
        LOCTEXT("GenerateArenaTooltip", "Populate the current level with a basic arena, lighting, navigation volume, PlayerStart, and enemy."),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FWistoriaDeveloperToolkitModule::GenerateCombatTestArena))
    );

    Section.AddMenuEntry(
        "WistoriaFixRedirectors",
        LOCTEXT("FixRedirectors", "Wistoria: Fix Redirectors"),
        LOCTEXT("FixRedirectorsTooltip", "Save all assets and ask the Content Browser to clean redirectors manually if required."),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FWistoriaDeveloperToolkitModule::FixRedirectors))
    );
}

void FWistoriaDeveloperToolkitModule::UnregisterMenus()
{
    UToolMenus::UnregisterOwner(this);
}

void FWistoriaDeveloperToolkitModule::ShowMessage(const FText& Title, const FText& Message) const
{
    FMessageDialog::Open(EAppMsgType::Ok, Message, Title);
}

void FWistoriaDeveloperToolkitModule::ValidateProject()
{
    const TArray<FString> RequiredFolders = {
        TEXT("/Game/Maps"),
        TEXT("/Game/Blueprints"),
        TEXT("/Game/Characters"),
        TEXT("/Game/Enemies"),
        TEXT("/Game/Bosses"),
        TEXT("/Game/UI"),
        TEXT("/Game/Items"),
        TEXT("/Game/Quests"),
        TEXT("/Game/DataAssets"),
        TEXT("/Game/Dungeons"),
        TEXT("/Game/Companions")
    };

    FString Report;
    int32 MissingCount = 0;

    for (const FString& Folder : RequiredFolders)
    {
        if (FPackageName::DoesPackageExist(Folder))
        {
            Report += FString::Printf(TEXT("OK: %s\n"), *Folder);
        }
        else
        {
            MissingCount++;
            Report += FString::Printf(TEXT("Missing: %s\n"), *Folder);
        }
    }

    const bool bHasCharacterClass = FindObject<UClass>(ANY_PACKAGE, TEXT("WistoriaCharacter")) != nullptr || LoadObject<UClass>(nullptr, TEXT("/Script/MyProject.WistoriaCharacter")) != nullptr;
    Report += bHasCharacterClass ? TEXT("OK: WistoriaCharacter C++ class\n") : TEXT("Missing: WistoriaCharacter C++ class\n");

    const FText Title = LOCTEXT("ValidationTitle", "Wistoria Project Validation");
    const FText Message = FText::FromString(FString::Printf(TEXT("Missing folders: %d\n\n%s"), MissingCount, *Report));
    ShowMessage(Title, Message);
}

void FWistoriaDeveloperToolkitModule::CreateFolderStructure()
{
    const TArray<FString> Folders = {
        TEXT("Maps"), TEXT("Blueprints"), TEXT("Characters"), TEXT("Enemies"), TEXT("Bosses"),
        TEXT("UI"), TEXT("FX"), TEXT("Audio"), TEXT("Materials"), TEXT("Items"),
        TEXT("Quests"), TEXT("DataAssets"), TEXT("Dungeons"), TEXT("Companions")
    };

    const FString ContentDir = FPaths::ProjectContentDir();

    for (const FString& Folder : Folders)
    {
        IFileManager::Get().MakeDirectory(*(ContentDir / Folder), true);
    }

    FAssetRegistryModule::AssetCreated(nullptr);
    ShowMessage(LOCTEXT("FoldersCreatedTitle", "Wistoria Toolkit"), LOCTEXT("FoldersCreatedMessage", "Core Wand and Wistoria folders have been created under /Game."));
}

bool FWistoriaDeveloperToolkitModule::CreateBlueprintAsset(const FString& AssetName, const FString& PackagePath, const FString& ParentClassPath)
{
    const FString FullAssetPath = PackagePath / AssetName;
    if (LoadObject<UObject>(nullptr, *FullAssetPath))
    {
        UE_LOG(LogTemp, Log, TEXT("Wistoria Toolkit: Blueprint already exists: %s"), *FullAssetPath);
        return true;
    }

    UClass* ParentClass = LoadObject<UClass>(nullptr, *ParentClassPath);
    if (!ParentClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("Wistoria Toolkit: Missing parent class: %s"), *ParentClassPath);
        return false;
    }

    UBlueprintFactory* Factory = NewObject<UBlueprintFactory>();
    Factory->ParentClass = ParentClass;

    FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
    UObject* NewAsset = AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, UBlueprint::StaticClass(), Factory);

    if (NewAsset)
    {
        FAssetRegistryModule::AssetCreated(NewAsset);
        NewAsset->MarkPackageDirty();
        UE_LOG(LogTemp, Log, TEXT("Wistoria Toolkit: Created Blueprint: %s"), *FullAssetPath);
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("Wistoria Toolkit: Failed to create Blueprint: %s"), *FullAssetPath);
    return false;
}

void FWistoriaDeveloperToolkitModule::BuildCoreBlueprints()
{
    CreateFolderStructure();

    int32 CreatedOrVerified = 0;

    CreatedOrVerified += CreateBlueprintAsset(TEXT("BP_WistoriaGameMode"), TEXT("/Game/Blueprints"), TEXT("/Script/MyProject.WistoriaGameMode")) ? 1 : 0;
    CreatedOrVerified += CreateBlueprintAsset(TEXT("BP_WistoriaPlayerController"), TEXT("/Game/Blueprints"), TEXT("/Script/MyProject.WistoriaPlayerController")) ? 1 : 0;
    CreatedOrVerified += CreateBlueprintAsset(TEXT("BP_WistoriaCharacter"), TEXT("/Game/Characters"), TEXT("/Script/MyProject.WistoriaCharacter")) ? 1 : 0;
    CreatedOrVerified += CreateBlueprintAsset(TEXT("BP_WistoriaEnemy"), TEXT("/Game/Enemies"), TEXT("/Script/MyProject.WistoriaEnemy")) ? 1 : 0;
    CreatedOrVerified += CreateBlueprintAsset(TEXT("BP_DungeonBoss"), TEXT("/Game/Bosses"), TEXT("/Script/MyProject.WistoriaBossEnemy")) ? 1 : 0;
    CreatedOrVerified += CreateBlueprintAsset(TEXT("BP_DungeonZone"), TEXT("/Game/Dungeons"), TEXT("/Script/MyProject.DungeonZone")) ? 1 : 0;
    CreatedOrVerified += CreateBlueprintAsset(TEXT("BP_DungeonWaveSpawner"), TEXT("/Game/Dungeons"), TEXT("/Script/MyProject.DungeonWaveSpawner")) ? 1 : 0;
    CreatedOrVerified += CreateBlueprintAsset(TEXT("BP_SavePoint"), TEXT("/Game/Blueprints"), TEXT("/Script/MyProject.SavePoint")) ? 1 : 0;
    CreatedOrVerified += CreateBlueprintAsset(TEXT("BP_TreasureChest"), TEXT("/Game/Items"), TEXT("/Script/MyProject.TreasureChest")) ? 1 : 0;
    CreatedOrVerified += CreateBlueprintAsset(TEXT("BP_CompanionMage"), TEXT("/Game/Companions"), TEXT("/Script/MyProject.WistoriaCompanionCharacter")) ? 1 : 0;
    CreatedOrVerified += CreateBlueprintAsset(TEXT("BP_NPCProfessor"), TEXT("/Game/Blueprints"), TEXT("/Script/MyProject.DialogueNPC")) ? 1 : 0;

    UEditorLoadingAndSavingUtils::SaveDirtyPackages(true, true);

    ShowMessage(
        LOCTEXT("BlueprintsBuiltTitle", "Wistoria Toolkit"),
        FText::FromString(FString::Printf(TEXT("Core Blueprint pass complete. Created or verified %d assets."), CreatedOrVerified))
    );
}

void FWistoriaDeveloperToolkitModule::GenerateCombatTestArena()
{
    UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
    if (!World)
    {
        ShowMessage(LOCTEXT("NoWorldTitle", "Wistoria Toolkit"), LOCTEXT("NoWorldMessage", "No editor world found. Open or create a level first."));
        return;
    }

    // Floor
    AStaticMeshActor* Floor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(0.f, 0.f, -50.f), FRotator::ZeroRotator);
    if (Floor)
    {
        UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
        Floor->SetActorLabel(TEXT("Wistoria_TestArena_Floor"));
        Floor->SetActorScale3D(FVector(50.f, 50.f, 1.f));
        if (CubeMesh)
        {
            Floor->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
        }
        Floor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
    }

    // Player Start
    World->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), FVector(0.f, -450.f, 150.f), FRotator(0.f, 0.f, 0.f));

    // Lights
    World->SpawnActor<ADirectionalLight>(ADirectionalLight::StaticClass(), FVector(0.f, 0.f, 800.f), FRotator(-45.f, 0.f, 0.f));
    World->SpawnActor<ASkyLight>(ASkyLight::StaticClass(), FVector(0.f, 0.f, 300.f), FRotator::ZeroRotator);

    // NavMesh volume
    ANavMeshBoundsVolume* NavVolume = World->SpawnActor<ANavMeshBoundsVolume>(ANavMeshBoundsVolume::StaticClass(), FVector(0.f, 0.f, 0.f), FRotator::ZeroRotator);
    if (NavVolume)
    {
        NavVolume->SetActorLabel(TEXT("Wistoria_TestArena_NavMeshBounds"));
        NavVolume->SetActorScale3D(FVector(25.f, 25.f, 5.f));
    }

    // Enemy Blueprint if present
    UBlueprint* EnemyBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Enemies/BP_WistoriaEnemy.BP_WistoriaEnemy"));
    if (EnemyBlueprint && EnemyBlueprint->GeneratedClass)
    {
        World->SpawnActor<AActor>(EnemyBlueprint->GeneratedClass, FVector(600.f, 0.f, 120.f), FRotator(0.f, 180.f, 0.f));
    }

    UEditorLoadingAndSavingUtils::SaveDirtyPackages(true, true);

    ShowMessage(
        LOCTEXT("ArenaGeneratedTitle", "Wistoria Toolkit"),
        LOCTEXT("ArenaGeneratedMessage", "Combat test arena actors were added to the current level. Run Build > Build Paths, then press P to view navigation.")
    );
}

void FWistoriaDeveloperToolkitModule::FixRedirectors()
{
    UEditorLoadingAndSavingUtils::SaveDirtyPackages(true, true);
    ShowMessage(
        LOCTEXT("RedirectorsTitle", "Wistoria Toolkit"),
        LOCTEXT("RedirectorsMessage", "Saved dirty packages. To fully fix redirectors, right-click /Game in Content Browser and choose Fix Up Redirectors in Folder.")
    );
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FWistoriaDeveloperToolkitModule, WistoriaDeveloperToolkit)
