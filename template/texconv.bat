@echo off
for %%i in (convert\tex\*.png) do (
echo ファイル名：%%i
"magick" %%i -compress RLE bin\resource\tex\%%~ni.tga
)
 

pause;