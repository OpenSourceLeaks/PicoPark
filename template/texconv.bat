@echo off
for %%i in (convert\tex\*.png) do (
echo �t�@�C�����F%%i
"magick" %%i -compress RLE bin\resource\tex\%%~ni.tga
)
 

pause;