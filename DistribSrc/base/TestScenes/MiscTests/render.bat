@echo off
REM This batch file runs the tests in this folder.

mkdir ..\..\TestsOutput

for /f %%a IN ('dir /b *.rib') do ..\..\RibRenderFile.exe clientmode %%a ..\..\TestsOutput\%%~na.jpg
