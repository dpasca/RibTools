REM == Set the RIBTOOLS_DIR variable
set RIBTOOLS_DIR=%cd%

REM == Create the TestsOutput directory
mkdir TestsOutput

REM == Render TestScenes\ValuesTests
pushd TestScenes\ValuesTests
call render
popd

REM == Render TestScenes\MiscTests
pushd TestScenes\MiscTests
call render
popd

REM == Render TestScenes\KillerooNURBS
pushd TestScenes\KillerooNURBS
..\..\RibRender killeroo.rib
move *.tif? ..\..\TestsOutput\KillerooNURBS.tif
popd

REM == Render everything else..
for /f %%a IN ('dir /b TestScenes\*.rib') do				( RibRender.exe TestScenes\%%a & move *.tif? TestsOutput\%%~na.tif )
for /f %%a IN ('dir /b TestScenes\SpacesTests\*.rib') do	( RibRender.exe TestScenes\SpacesTests\%%a & move *.tif? TestsOutput\%%~na.tif )
for /f %%a IN ('dir /b TestScenes\LightsTests\*.rib') do	( RibRender.exe TestScenes\LightsTests\%%a & move *.tif? TestsOutput\%%~na.tif )
for /f %%a IN ('dir /b TestScenes\Sponza\sponza.rib') do	( RibRender.exe TestScenes\Sponza\%%a & move *.tif? TestsOutput\%%~na.tif )
