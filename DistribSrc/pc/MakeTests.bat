mkdir TestsOutput

for /f %%a IN ('dir /b TestScenes\*.rib') do RibRenderFile.exe clientmode TestScenes\%%a TestsOutput\%%~na.jpg
for /f %%a IN ('dir /b TestScenes\Sponza\sponza.rib') do RibRenderFile.exe clientmode TestScenes\Sponza\%%a TestsOutput\%%~na.jpg
