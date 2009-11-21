@echo off
REM This batch file runs the tests throught Aqsis if installed.
REM This is used only for internal testing. RibTools does not require Aqsis to function.

mkdir ..\TestsOutputAqsis

pushd ..\TestScenes

pushd ValuesTests
call render_aqsis
popd

pushd SpacesTests
aqsl *.sl
for %%X in (*.rib) do ( aqsis %%X & move ri.pic ..\..\TestsOutputAqsis\%%~nX.tif & move *.tif ..\..\TestsOutputAqsis\ & move *.tiff ..\..\TestsOutputAqsis\ )
del *.slx
popd

pushd LightsTests
aqsl *.sl
for %%X in (*.rib) do ( aqsis %%X & move ri.pic ..\..\TestsOutputAqsis\%%~nX.tif & move *.tif ..\..\TestsOutputAqsis\ & move *.tiff ..\..\TestsOutputAqsis\ )
del *.slx
popd

aqsl *.sl
for %%X in (*.rib) do ( aqsis %%X & move ri.pic ..\TestsOutputAqsis\%%~nX.tif & move *.tif ..\TestsOutputAqsis\ & move *.tiff ..\TestsOutputAqsis\ )
del *.slx

cd Sponza
aqsl *.sl
for %%X in (*.rib) do ( aqsis %%X & move ri.pic ..\TestsOutputAqsis\%%~nX.tif & move *.tif ..\TestsOutputAqsis\ & move *.tiff ..\TestsOutputAqsis\ )
del *.slx
cd ..

popd
