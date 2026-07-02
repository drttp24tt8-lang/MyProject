param(
	[string]$ProjectRoot = "C:\Users\AJTOU\OneDrive\Documents\Unreal Projects\MyProject",
	[string]$OutputZip = "C:\Users\AJTOU\OneDrive\Documents\WandAndWistoria_BetaProject.zip"
)

$ErrorActionPreference = "Stop"

$projectRootItem = Get-Item -LiteralPath $ProjectRoot
$projectRootFullName = $projectRootItem.FullName.TrimEnd('\')
$stagingRoot = Join-Path ([System.IO.Path]::GetTempPath()) ("WandAndWistoria_BetaProject_" + [System.Guid]::NewGuid().ToString("N"))
$stagingProject = Join-Path $stagingRoot $projectRootItem.Name

$includeDirectories = @(
	"Config",
	"Content",
	"DataSeeds",
	"Source",
	"Tools"
)

$includeFiles = @(
	"MyProject.uproject",
	"BETA_TEST_README.md",
	"DUNGEON_LAYER_02_CORRUPTED_ABYSS.md",
	"MISSION_01_THE_FIRST_DESCENT.md",
	"NIAGARA_WILL_ULTIMATE_AURA_BLAST.md",
	"PACKAGING_AND_KEYBINDING.md",
	"WAND_AND_WISTORIA_PROJECT.md",
	"WORLD_PARTITION_OPTIMIZATION.md"
)

New-Item -ItemType Directory -Force -Path $stagingProject | Out-Null

foreach ($directory in $includeDirectories) {
	$source = Join-Path $projectRootFullName $directory
	if (Test-Path -LiteralPath $source) {
		Copy-Item -LiteralPath $source -Destination $stagingProject -Recurse -Force
	}
}

foreach ($file in $includeFiles) {
	$source = Join-Path $projectRootFullName $file
	if (Test-Path -LiteralPath $source) {
		Copy-Item -LiteralPath $source -Destination $stagingProject -Force
	}
}

if (Test-Path -LiteralPath $OutputZip) {
	Remove-Item -LiteralPath $OutputZip -Force
}

Compress-Archive -Path $stagingProject -DestinationPath $OutputZip -CompressionLevel Optimal
Remove-Item -LiteralPath $stagingRoot -Recurse -Force

Write-Host "Created beta project archive: $OutputZip"
