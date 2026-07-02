param(
	[string]$ProjectRoot = "C:\Users\AJTOU\OneDrive\Documents\Unreal Projects\MyProject",
	[string]$BetaZip = "C:\Users\AJTOU\OneDrive\Documents\WandAndWistoria_BetaProject.zip"
)

$ErrorActionPreference = "Stop"
$failures = New-Object System.Collections.Generic.List[string]

function Test-RequiredPath {
	param([string]$Path, [string]$Label)
	if (-not (Test-Path -LiteralPath $Path)) {
		$script:failures.Add("Missing $Label`: $Path")
	}
}

Test-RequiredPath (Join-Path $ProjectRoot "MyProject.uproject") "uproject"
Test-RequiredPath (Join-Path $ProjectRoot "Config\DefaultEngine.ini") "DefaultEngine.ini"
Test-RequiredPath (Join-Path $ProjectRoot "Content\Maps\LV1_TestArena.umap") "default beta test map"
Test-RequiredPath (Join-Path $ProjectRoot "Source\MyProject\Public\WistoriaCharacter.h") "player character header"
Test-RequiredPath (Join-Path $ProjectRoot "Source\MyProject\Private\WistoriaCharacter.cpp") "player character source"
Test-RequiredPath (Join-Path $ProjectRoot "Source\MyProject\Public\WistoriaTitleGameMode.h") "title game mode"
Test-RequiredPath (Join-Path $ProjectRoot "Source\MyProject\Public\MiasmaHazardVolume.h") "miasma hazard volume"
Test-RequiredPath (Join-Path $ProjectRoot "Source\MyProject\Public\WistoriaBetaTestProbe.h") "beta test probe header"
Test-RequiredPath (Join-Path $ProjectRoot "Source\MyProject\Private\WistoriaBetaTestProbe.cpp") "beta test probe source"
Test-RequiredPath (Join-Path $ProjectRoot "Source\MyProject\Private\WistoriaAutomationTests.cpp") "automation smoke tests"
Test-RequiredPath (Join-Path $ProjectRoot "DataSeeds\DT_WeaponLog.csv") "weapon data seed"
Test-RequiredPath (Join-Path $ProjectRoot "DataSeeds\DT_SpellBook.csv") "spell data seed"
Test-RequiredPath (Join-Path $ProjectRoot "BETA_TEST_README.md") "beta readme"
Test-RequiredPath (Join-Path $ProjectRoot "Tools\ValidateDataSeeds.ps1") "data seed validator"
Test-RequiredPath (Join-Path $ProjectRoot "Tools\RunWistoriaAutomationTests.ps1") "automation test runner"
Test-RequiredPath $BetaZip "beta zip"

$engineConfig = Join-Path $ProjectRoot "Config\DefaultEngine.ini"
if (Test-Path -LiteralPath $engineConfig) {
	$configText = Get-Content -LiteralPath $engineConfig -Raw
	if ($configText -notmatch "/Game/Maps/LV1_TestArena\.LV1_TestArena") {
		$failures.Add("DefaultEngine.ini does not point to /Game/Maps/LV1_TestArena.")
	}
	if ($configText -notmatch "GameInstanceClass=/Script/MyProject\.WistoriaGameInstance") {
		$failures.Add("DefaultEngine.ini does not use WistoriaGameInstance.")
	}
}

$gameConfig = Join-Path $ProjectRoot "Config\DefaultGame.ini"
if (Test-Path -LiteralPath $gameConfig) {
	$gameConfigText = Get-Content -LiteralPath $gameConfig -Raw
	foreach ($mapPath in @("/Game/Maps/LV1_TestArena", "/Game/Maps/Lvl_CombatTest", "/Game/DemoTemplate/_Core/Lvl_IntroRoom")) {
		if ($gameConfigText -notmatch [Regex]::Escape($mapPath)) {
			$failures.Add("DefaultGame.ini packaging settings do not include map: $mapPath")
		}
	}
}

$uproject = Join-Path $ProjectRoot "MyProject.uproject"
if (Test-Path -LiteralPath $uproject) {
	$projectJson = Get-Content -LiteralPath $uproject -Raw | ConvertFrom-Json
	if ($projectJson.EngineAssociation -ne "5.8") {
		$failures.Add("Expected EngineAssociation 5.8, found $($projectJson.EngineAssociation).")
	}
	if (($projectJson.Plugins | Where-Object { $_.Name -eq "AIAssistant" -and $_.Enabled }).Count -gt 0) {
		$failures.Add("AIAssistant plugin should not be required by the beta project.")
	}
}

if (Test-Path -LiteralPath $BetaZip) {
	Add-Type -AssemblyName System.IO.Compression.FileSystem
	$archive = [IO.Compression.ZipFile]::OpenRead($BetaZip)
	try {
		$requiredEntries = @(
			"MyProject/MyProject.uproject",
			"MyProject/BETA_TEST_README.md",
			"MyProject/Config/DefaultEngine.ini",
			"MyProject/Content/Maps/LV1_TestArena.umap",
			"MyProject/Source/MyProject/Public/WistoriaCharacter.h",
			"MyProject/Source/MyProject/Public/WistoriaBetaTestProbe.h",
			"MyProject/Source/MyProject/Private/WistoriaAutomationTests.cpp",
			"MyProject/Tools/RunWistoriaAutomationTests.ps1"
		)
		foreach ($entry in $requiredEntries) {
			$normalized = $archive.Entries | Where-Object { $_.FullName.Replace('\','/') -eq $entry }
			if (-not $normalized) {
				$failures.Add("Beta zip missing entry: $entry")
			}
		}
	}
	finally {
		$archive.Dispose()
	}
}

if ($failures.Count -gt 0) {
	Write-Host "Beta validation failed:" -ForegroundColor Red
	foreach ($failure in $failures) {
		Write-Host " - $failure" -ForegroundColor Red
	}
	exit 1
}

Write-Host "Beta validation passed." -ForegroundColor Green
