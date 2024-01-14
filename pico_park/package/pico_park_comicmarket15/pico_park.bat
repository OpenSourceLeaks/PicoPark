cd /d %~dp0
cd bin
taskkill /F /IM pico_park_dx9.exe /T
start "" "pico_park_dx9.exe"