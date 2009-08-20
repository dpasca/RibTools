mkdir TestsOutput

for /f %%a IN ('dir /b TestScenes\*.rib') do RibRenderFile.exe TestScenes\%%a TestsOutput\%%~na.jpg
