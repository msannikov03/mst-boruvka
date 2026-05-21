param(
    [Parameter(Mandatory=$true)][string]$Graph,
    [int[]]$Threads = @(1, 2, 4, 8)
)

$bench = if (Test-Path "build\tools\Release\bench.exe") { "build\tools\Release\bench.exe" }
         elseif (Test-Path "build\tools\bench.exe")     { "build\tools\bench.exe" }
         else { Write-Error "missing bench.exe; build first: cmake --build build --config Release"; exit 1 }

Get-Content $Graph | & $bench kruskal     1
Get-Content $Graph | & $bench prim        1
Get-Content $Graph | & $bench boruvka_seq 1
foreach ($t in $Threads) {
    Get-Content $Graph | & $bench boruvka_omp $t
}
