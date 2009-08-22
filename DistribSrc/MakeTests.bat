mkdir TestsOutput

for /f %%a IN ('dir /b TestScenes\*.rib') do RibRenderFile.exe TestScenes\%%a TestsOutput\%%~na.jpg
for /f %%a IN ('dir /b TestScenes\Sponza\sponza.rib') do RibRenderFile.exe TestScenes\Sponza\%%a TestsOutput\%%~na.jpg
