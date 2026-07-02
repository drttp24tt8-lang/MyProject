using UnrealBuildTool;

public class WistoriaDeveloperToolkit : ModuleRules
{
    public WistoriaDeveloperToolkit(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "UnrealEd",
            "LevelEditor",
            "ToolMenus",
            "Projects",
            "AssetTools",
            "AssetRegistry",
            "NavigationSystem",
            "EditorFramework",
            "EditorStyle",
            "InputCore"
        });
    }
}
