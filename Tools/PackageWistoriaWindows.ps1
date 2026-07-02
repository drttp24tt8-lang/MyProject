param(
	[string]$EngineRoot = "F:\UE_5.8",
	[string]$ProjectPath = "C:\Users\AJTOU\OneDrive\Documents\Unreal Projects\MyProject\MyProject.uproject",
	[string]$OutputDirectory = "C:\WistoriaRPG_Builds",
	[string]$Configuration = "Shipping",
	[string[]]$MapsToCook = @(
		"/Game/Maps/LV1_TestArena",
		"/Game/Maps/Lvl_CombatTest",
		"/Game/DemoTemplate/_Core/Lvl_IntroRoom"
	)
)

$ErrorActionPreference = "Stop"

$uat = Join-Path $EngineRoot "Engine\Build\BatchFiles\RunUAT.bat"
if (-not (Test-Path -LiteralPath $uat)) {
	Write-Error "RunUAT not found at: $uat"
}

New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null

$mapArguments = @()
foreach ($map in $MapsToCook) {
	if (-not [string]::IsNullOrWhiteSpace($map)) {
		$mapArguments += "-map=$map"
	}
}

$uatArguments = @(
	"BuildCookRun",
	"-project=$ProjectPath",
	"-noP4",
	"-platform=Win64",
	"-clientconfig=$Configuration",
	"-build",
	"-cook",
	"-stage",
	"-pak",
	"-archive",
	"-archivedirectory=$OutputDirectory"
) + $mapArguments

& $uat @uatArguments

exit $LASTEXITCODE
