#pragma once

#include "Modules/ModuleManager.h"

class FWistoriaDeveloperToolkitModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void RegisterMenus();
    void UnregisterMenus();

    void ValidateProject();
    void CreateFolderStructure();
    void BuildCoreBlueprints();
    void GenerateCombatTestArena();
    void FixRedirectors();

    bool CreateBlueprintAsset(const FString& AssetName, const FString& PackagePath, const FString& ParentClassPath);
    void ShowMessage(const FText& Title, const FText& Message) const;
};
