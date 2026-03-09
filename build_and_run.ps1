$sfmlUrl = "https://github.com/SFML/SFML/releases/download/2.5.1/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit.zip"
$zipFile = "SFML.zip"
$extractDir = "SFML_Extracted"
$finalDir = "SFML"

if (-Not (Test-Path -Path $finalDir)) {
    Write-Host "Downloading SFML 2.5.1 (MinGW 64-bit)..."
    Invoke-WebRequest -Uri $sfmlUrl -OutFile $zipFile
    
    Write-Host "Extracting SFML..."
    Expand-Archive -Path $zipFile -DestinationPath $extractDir -Force
    
    # Move the inner SFML-2.5.1 folder to just SFML
    $innerFolder = Get-ChildItem -Path $extractDir | Select-Object -First 1
    Move-Item -Path $innerFolder.FullName -Destination $finalDir -Force
    
    Write-Host "Cleaning up temp files..."
    Remove-Item -Path $zipFile -Force
    Remove-Item -Path $extractDir -Recurse -Force
}

Write-Host "Copying SFML DLLs to the game directory..."
Copy-Item -Path "$finalDir\bin\*.dll" -Destination ".\" -Force



Write-Host "Compiling the game using g++..."
$sources = "main.cpp", "GameTypes.cpp", "Core.cpp", "battle.cpp", "UI.cpp", "Dungeon.cpp"
$compilerArgs = @(
    "-std=c++17",
    "-O2"
)
$compilerArgs += $sources
$compilerArgs += @(
    "-I.\$finalDir\include",
    "-L.\$finalDir\lib",
    "-lsfml-graphics",
    "-lsfml-window",
    "-lsfml-system",
    "-mwindows",
    "-static-libgcc",
    "-static-libstdc++",
    "-o",
    "RPG_Game.exe"
)

# Start g++ with the arguments array
$process = Start-Process -FilePath "g++" -ArgumentList $compilerArgs -NoNewWindow -Wait -PassThru

if ($process.ExitCode -eq 0) {
    Write-Host "Compilation successful!" -ForegroundColor Green
    Write-Host "Starting the game..."
    Start-Process -FilePath ".\RPG_Game.exe"
} else {
    Write-Host "Compilation failed with exit code $($process.ExitCode)." -ForegroundColor Red
}
