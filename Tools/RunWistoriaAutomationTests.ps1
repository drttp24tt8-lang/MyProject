param(
	[string]$EngineRoot = "F:\UE_5.8",
	[string]$ProjectPath = "C:\Users\AJTOU\OneDrive\Documents\Unreal Projects\MyProject\MyProject.uproject",
	[string]$TestFilter = "WandAndWistoria.Core"
)

$ErrorActionPreference = "Stop"

$editorCmd = Join-Path $EngineRoot "Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
if (-not (Test-Path -LiteralPath $editorCmd)) {
	Write-Error "UnrealEditor-Cmd not found at: $editorCmd"
}

& $editorCmd "$ProjectPath" `
	-Unattended `
	-NoSplash `
	-NullRHI `
	-ExecCmds="Automation RunTests $TestFilter; Quit" `
	-TestExit="Automation Test Queue Empty"

exit $LASTEXITCODE
