#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FWistoriaDeveloperToolkitModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void RegisterMenus();

    void ValidateProject();
    void CreateFolderStructure();
    void BuildCoreBlueprints();
    void GenerateCombatTestArena();
    void FixRedirectors();

    UClass* LoadWistoriaClass(const FString& ClassPath) const;
    UObject* CreateBlueprintAsset(const FString& AssetName, const FString& PackagePath, const FString& ParentClassPath) const;
    void Log(const FString& Message) const;
    void Warn(const FString& Message) const;
};
