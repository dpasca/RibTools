@echo off
REM This batch file runs the tests throught Aqsis if installed.
REM This is used only for internal testing. RibTools does not require Aqsis to function.

mkdir ..\..\TestsOutputAqsis

aqsl *.sl
move *.slx ..\..\TestsOutputAqsis\
for %%X in (*.rib) do ( aqsis %%X -shaders=..\..\TestsOutputAqsis & move ri.pic ..\..\TestsOutputAqsis\%%~nX.tif & move *.tif ..\..\TestsOutputAqsis\ & move *.tiff ..\..\TestsOutputAqsis\ )
REM del *.slx
