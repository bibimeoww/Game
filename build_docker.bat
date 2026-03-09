@echo off
echo Building Docker Image...
docker build -t sfml-game-builder .

echo.
echo Extracting Game Files...
rem Remove old release folder if exists
if exist release rmdir /s /q release
rem Run container and copy files out
docker run --rm -v "%cd%":/out sfml-game-builder bash -c "cp -r /release /out/"

echo.
echo Zipping files for sharing...
if exist GameRelease.zip del /Q GameRelease.zip
powershell -Command "Compress-Archive -Path '.\release\*' -DestinationPath '.\GameRelease.zip' -Force"

echo.
echo Done! You can find your game in GameRelease.zip
pause
