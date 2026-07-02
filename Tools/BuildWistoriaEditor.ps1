param(
	[string]$EngineRoot = "F:\UE_5.8",
	[string]$ProjectPath = "C:\Users\AJTOU\OneDrive\Documents\Unreal Projects\MyProject\MyProject.uproject",
	[string]$Configuration = "Development"
)

$ErrorActionPreference = "Stop"

$liveCoding = Get-Process -Name "LiveCodingConsole" -ErrorAction SilentlyContinue
if ($liveCoding) {
	Write-Error "Live Coding is active. Close Unreal Editor or press Ctrl+Alt+F11 in the editor, then rerun this script."
}

$ubt = Join-Path $EngineRoot "Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
if (-not (Test-Path -LiteralPath $ubt)) {
	Write-Error "UnrealBuildTool not found at: $ubt"
}

& $ubt MyProjectEditor Win64 $Configuration -Project="$ProjectPath" -WaitMutex -FromMsBuild
exit $LASTEXITCODE
