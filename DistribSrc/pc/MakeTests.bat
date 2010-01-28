mkdir TestsOutput

pushd TestScenes\ValuesTests
call render
popd

pushd TestScenes\MiscTests
call render
popd

pushd TestScenes\KillerooNURBS
..\..\RibRender killeroo.rib
move *.tif? ..\..\TestsOutput\KillerooNURBS.tif
popd

for /f %%a IN ('dir /b TestScenes\*.rib') do				( RibRender.exe TestScenes\%%a & move *.tif? TestsOutput\%%~na.tif )
for /f %%a IN ('dir /b TestScenes\SpacesTests\*.rib') do	( RibRender.exe TestScenes\SpacesTests\%%a & move *.tif? TestsOutput\%%~na.tif )
for /f %%a IN ('dir /b TestScenes\LightsTests\*.rib') do	( RibRender.exe TestScenes\LightsTests\%%a & move *.tif? TestsOutput\%%~na.tif )
for /f %%a IN ('dir /b TestScenes\Sponza\sponza.rib') do	( RibRender.exe TestScenes\Sponza\%%a & move *.tif? TestsOutput\%%~na.tif )
