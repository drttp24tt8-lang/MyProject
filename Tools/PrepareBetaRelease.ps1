param(
	[string]$ProjectRoot = "C:\Users\AJTOU\OneDrive\Documents\Unreal Projects\MyProject",
	[string]$BetaZip = "C:\Users\AJTOU\OneDrive\Documents\WandAndWistoria_BetaProject.zip",
	[switch]$Build,
	[switch]$RunAutomationTests
)

$ErrorActionPreference = "Stop"

Push-Location $ProjectRoot
try {
	.\Tools\ValidateDataSeeds.ps1 -ProjectRoot $ProjectRoot
	.\Tools\CreateBetaProjectZip.ps1 -ProjectRoot $ProjectRoot -OutputZip $BetaZip
	.\Tools\ValidateBetaProject.ps1 -ProjectRoot $ProjectRoot -BetaZip $BetaZip

	if ($Build) {
		.\Tools\BuildWistoriaEditor.ps1
		if ($RunAutomationTests) {
			.\Tools\RunWistoriaAutomationTests.ps1
		}
		else {
			Write-Host "Automation tests not requested. Rerun with -Build -RunAutomationTests after closing the editor." -ForegroundColor Yellow
		}
	}
	else {
		$liveCoding = Get-Process -Name "LiveCodingConsole" -ErrorAction SilentlyContinue
		if ($liveCoding) {
			Write-Host "Skipping editor build because Live Coding is active. Close Unreal or press Ctrl+Alt+F11, then rerun with -Build." -ForegroundColor Yellow
		}
		else {
			Write-Host "Build not requested. Rerun with -Build to compile the editor target." -ForegroundColor Yellow
		}

		if ($RunAutomationTests) {
			Write-Host "Skipping automation tests because -Build was not requested. Rerun with -Build -RunAutomationTests." -ForegroundColor Yellow
		}
	}

	Write-Host "Beta release preparation completed: $BetaZip" -ForegroundColor Green
}
finally {
	Pop-Location
}
