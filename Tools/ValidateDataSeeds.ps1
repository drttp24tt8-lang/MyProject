param(
	[string]$ProjectRoot = "C:\Users\AJTOU\OneDrive\Documents\Unreal Projects\MyProject",
	[int]$ExpectedWeaponRows = 1000,
	[int]$ExpectedSpellRows = 500,
	[int]$ExpectedRecipeRows = 120
)

$ErrorActionPreference = "Stop"
$failures = New-Object System.Collections.Generic.List[string]

function Test-CsvRows {
	param(
		[string]$Path,
		[int]$ExpectedRows,
		[string[]]$RequiredColumns
	)

	if (-not (Test-Path -LiteralPath $Path)) {
		$script:failures.Add("Missing CSV: $Path")
		return
	}

	$rows = Import-Csv -LiteralPath $Path
	if ($rows.Count -ne $ExpectedRows) {
		$script:failures.Add("Expected $ExpectedRows rows in $Path, found $($rows.Count).")
	}

	if ($rows.Count -gt 0) {
		$columns = $rows[0].PSObject.Properties.Name
		foreach ($column in $RequiredColumns) {
			if ($columns -notcontains $column) {
				$script:failures.Add("CSV $Path missing column $column.")
			}
		}
	}
}

$dataSeeds = Join-Path $ProjectRoot "DataSeeds"
Test-CsvRows `
	-Path (Join-Path $dataSeeds "DT_WeaponLog.csv") `
	-ExpectedRows $ExpectedWeaponRows `
	-RequiredColumns @("Name", "WeaponId", "WeaponName", "bIsWand", "BasePhysicalDamage", "BaseMagicalMultiplier", "EquipmentSlot")

Test-CsvRows `
	-Path (Join-Path $dataSeeds "DT_SpellBook.csv") `
	-ExpectedRows $ExpectedSpellRows `
	-RequiredColumns @("Name", "SpellId", "SpellName", "Element", "ManaCost", "BaseDamage", "StatusToApply", "StatusDuration", "StatusChance")

Test-CsvRows `
	-Path (Join-Path $dataSeeds "DT_CraftingRecipes.csv") `
	-ExpectedRows $ExpectedRecipeRows `
	-RequiredColumns @("Name", "RecipeId", "ResultItemId", "RequiredMaterials", "GoldCost")

if ($failures.Count -gt 0) {
	Write-Host "Data seed validation failed:" -ForegroundColor Red
	foreach ($failure in $failures) {
		Write-Host " - $failure" -ForegroundColor Red
	}
	exit 1
}

Write-Host "Data seed validation passed: $ExpectedWeaponRows weapons, $ExpectedSpellRows spells, $ExpectedRecipeRows recipes." -ForegroundColor Green
