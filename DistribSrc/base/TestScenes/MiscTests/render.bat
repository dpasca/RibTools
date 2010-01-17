@echo off
REM This batch file runs the tests in this folder.

mkdir ..\..\TestsOutput

for /f %%a IN ('dir /b *.rib') do ..\..\RibRender.exe clientmode %%a ..\..\TestsOutput\%%~na.jpg
