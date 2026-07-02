param(
    [string]$OutputDirectory = ".\DataSeeds",
    [int]$WeaponCount = 1000,
    [int]$SpellCount = 500,
    [int]$RecipeCount = 120
)

$ErrorActionPreference = "Stop"

New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null

$weaponPath = Join-Path $OutputDirectory "DT_WeaponLog.csv"
$spellPath = Join-Path $OutputDirectory "DT_SpellBook.csv"
$recipePath = Join-Path $OutputDirectory "DT_CraftingRecipes.csv"

$weaponLines = New-Object System.Collections.Generic.List[string]
$weaponLines.Add("Name,WeaponId,WeaponName,bIsWand,BasePhysicalDamage,BaseMagicalMultiplier,EquipmentSlot,WeaponMeshAsset")

for ($i = 1; $i -le $WeaponCount; $i++) {
    $isWand = ($i % 2 -eq 0)
    $prefix = if ($isWand) { "Wand" } else { "Sword" }
    $id = "{0}_{1:D4}" -f $prefix, $i
    $name = "{0} {1:D4}" -f $prefix, $i
    $physical = if ($isWand) { 6 + [math]::Floor($i / 20) } else { 12 + [math]::Floor($i / 12) }
    $magic = if ($isWand) { 1.10 + ($i % 75) / 100.0 } else { 1.00 + ($i % 25) / 200.0 }
    $slot = "MainHand"
    $weaponLines.Add(("{0},{0},`"{1}`",{2},{3},{4:N2},{5}," -f $id, $name, $isWand.ToString().ToLowerInvariant(), $physical, $magic, $slot))
}

Set-Content -LiteralPath $weaponPath -Value $weaponLines -Encoding UTF8

$elements = @("Fire", "Ice", "Lightning", "Miasma")
$spellLines = New-Object System.Collections.Generic.List[string]
$spellLines.Add("Name,SpellId,SpellName,Element,ManaCost,BaseDamage,ExplosionRadius,StatusToApply,StatusDuration,StatusChance,SpellVFXAsset")

for ($i = 1; $i -le $SpellCount; $i++) {
    $element = $elements[($i - 1) % $elements.Count]
    $id = "Spell_{0}_{1:D3}" -f $element, $i
    $name = "{0} Invocation {1:D3}" -f $element, $i
    $mana = 8 + ($i % 40)
    $damage = 20 + [math]::Floor($i / 3)
    $radius = if ($i % 5 -eq 0) { 250 + ($i % 200) } else { 0 }
    $status = switch ($element) {
        "Fire" { "Burn" }
        "Ice" { "Freeze" }
        "Miasma" { "Miasma" }
        default { "None" }
    }
    $duration = switch ($status) {
        "Burn" { 4.0 + ($i % 4) }
        "Freeze" { 2.0 + ($i % 3) }
        "Miasma" { 6.0 + ($i % 5) }
        default { 0.0 }
    }
    $chance = if ($status -eq "None") { 0.0 } else { [math]::Min(1.0, 0.55 + ($i % 10) / 20.0) }
    $spellLines.Add(("{0},{0},`"{1}`",{2},{3},{4},{5},{6},{7:N1},{8:N2}," -f $id, $name, $element, $mana, $damage, $radius, $status, $duration, $chance))
}

Set-Content -LiteralPath $spellPath -Value $spellLines -Encoding UTF8

$materials = @("Aether_Iron", "Dungeon_Core_Shard", "Arcane_Silk", "Mana_Crystal", "Beast_Fang", "Sky_Rune")
$recipeLines = New-Object System.Collections.Generic.List[string]
$recipeLines.Add("Name,RecipeId,ResultItemId,ResultDisplayName,ResultEquipmentSlot,RequiredMaterials,GoldCost,ResultPhysicalAttackBonus,ResultMagicPowerBonus,ResultCourageBonus")

for ($i = 1; $i -le $RecipeCount; $i++) {
    $isWand = ($i % 2 -eq 0)
    $resultPrefix = if ($isWand) { "Wand" } else { "Sword" }
    $recipeId = "Forge_{0}_{1:D3}" -f $resultPrefix, $i
    $resultItemId = "{0}_Forged_{1:D3}" -f $resultPrefix, $i
    $resultName = "Forged {0} {1:D3}" -f $resultPrefix, $i
    $matA = $materials[($i - 1) % $materials.Count]
    $matB = $materials[$i % $materials.Count]
    $qtyA = 2 + ($i % 4)
    $qtyB = 1 + ($i % 3)
    $required = "((MaterialItemId=`"$matA`",RequiredQuantity=$qtyA),(MaterialItemId=`"$matB`",RequiredQuantity=$qtyB))"
    $requiredCsv = $required.Replace('"', '""')
    $gold = 50 + ($i * 15)
    $physical = if ($isWand) { 8 + [math]::Floor($i / 8) } else { 20 + [math]::Floor($i / 4) }
    $magic = if ($isWand) { 0.35 + ($i % 30) / 100.0 } else { 0.05 + ($i % 10) / 100.0 }
    $courage = if ($isWand) { 0 } else { 5 + ($i % 12) }
    $recipeLines.Add(("{0},{0},{1},`"{2}`",MainHand,`"{3}`",{4},{5},{6:N2},{7}" -f $recipeId, $resultItemId, $resultName, $requiredCsv, $gold, $physical, $magic, $courage))
}

Set-Content -LiteralPath $recipePath -Value $recipeLines -Encoding UTF8

Write-Host "Wrote $WeaponCount weapon rows to $weaponPath"
Write-Host "Wrote $SpellCount spell rows to $spellPath"
Write-Host "Wrote $RecipeCount crafting recipe rows to $recipePath"
