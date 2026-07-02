param(
	[switch]$WhatIf,
	[switch]$IncludeSaved,
	[switch]$IncludeDerivedDataCache
)

$ProjectRoot = Split-Path -Parent $PSScriptRoot
$ProjectFile = Join-Path $ProjectRoot "MyProject.uproject"

if (!(Test-Path -LiteralPath $ProjectFile)) {
	throw "Could not find MyProject.uproject at $ProjectFile"
}

$Targets = @(
	"Binaries",
	"Intermediate",
	".vs",
	"MyProject.sln"
)

if ($IncludeSaved) {
	$Targets += "Saved"
}

if ($IncludeDerivedDataCache) {
	$Targets += "DerivedDataCache"
}

Write-Host "Cleaning generated Unreal files in: $ProjectRoot"
Write-Host "Keep Source, Content, Config, DataSeeds, and Tools intact."

foreach ($Target in $Targets) {
	$Path = Join-Path $ProjectRoot $Target
	if (!(Test-Path -LiteralPath $Path)) {
		continue
	}

	if ($WhatIf) {
		Write-Host "[WhatIf] Would remove $Path"
		continue
	}

	Write-Host "Removing $Path"
	Remove-Item -LiteralPath $Path -Recurse -Force
}

Write-Host ""
Write-Host "Cleanup complete."
Write-Host "Next steps:"
Write-Host "1. Install Visual Studio C++ tools and Windows SDK 10.0.19041.0 or newer."
Write-Host "2. Right-click MyProject.uproject and choose Generate Visual Studio project files."
Write-Host "3. Build MyProjectEditor Win64 Development."
