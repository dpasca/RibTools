@echo off
REM This batch file runs the tests throught Aqsis if installed.
REM This is used only for internal testing. RibTools does not require Aqsis to function.

mkdir ..\..\TestsOutput

for /f %%a IN ('dir /b *.rib') do ..\..\RibRenderFile.exe clientmode %%a ..\..\TestsOutput\%%~na.jpg
