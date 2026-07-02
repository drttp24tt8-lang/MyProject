using UnrealBuildTool;

public class WistoriaDeveloperToolkit : ModuleRules
{
    public WistoriaDeveloperToolkit(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate",
            "SlateCore",
            "UnrealEd",
            "ToolMenus",
            "AssetTools",
            "AssetRegistry",
            "Kismet",
            "EditorScriptingUtilities",
            "LevelEditor",
            "Projects",
            "NavigationSystem"
        });
    }
}
