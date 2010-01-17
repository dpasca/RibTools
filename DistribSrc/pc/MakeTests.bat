mkdir TestsOutput

pushd TestScenes\ValuesTests
call render
popd

pushd TestScenes\MiscTests
call render
popd

for /f %%a IN ('dir /b TestScenes\*.rib') do RibRender.exe clientmode TestScenes\%%a TestsOutput\%%~na.jpg
for /f %%a IN ('dir /b TestScenes\KillerooNURBS\*.rib') do RibRender.exe clientmode TestScenes\KillerooNURBS\%%a TestsOutput\%%~na.jpg
for /f %%a IN ('dir /b TestScenes\SpacesTests\*.rib') do RibRender.exe clientmode TestScenes\SpacesTests\%%a TestsOutput\%%~na.jpg
for /f %%a IN ('dir /b TestScenes\LightsTests\*.rib') do RibRender.exe clientmode TestScenes\LightsTests\%%a TestsOutput\%%~na.jpg
for /f %%a IN ('dir /b TestScenes\Sponza\sponza.rib') do RibRender.exe clientmode TestScenes\Sponza\%%a TestsOutput\%%~na.jpg
