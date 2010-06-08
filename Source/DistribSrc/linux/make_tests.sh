#!/bin/bash

# == Set the RIBTOOLS_DIR variable
RIBTOOLS_DIR=`pwd`

# == Create the TestsOutput directory
mkdir TestsOutput

# == Render TestScenes/ValuesTests
pushd TestScenes/ValuesTests
. render
popd

# == Render TestScenes/MiscTests
pushd TestScenes/MiscTests
. render
popd

# == Render TestScenes/KillerooNURBS
pushd TestScenes/KillerooNURBS
../../RibRender killeroo.rib
move *.tif? ../../TestsOutput/KillerooNURBS.tif
popd

# == Render everything else..
# TODO !!
# for /f %%a IN ('dir /b TestScenes\*.rib') do				( RibRender.exe TestScenes\%%a & move *.tif? TestsOutput\%%~na.tif )
# for /f %%a IN ('dir /b TestScenes\SpacesTests\*.rib') do	( RibRender.exe TestScenes\SpacesTests\%%a & move *.tif? TestsOutput\%%~na.tif )
# for /f %%a IN ('dir /b TestScenes\LightsTests\*.rib') do	( RibRender.exe TestScenes\LightsTests\%%a & move *.tif? TestsOutput\%%~na.tif )
# for /f %%a IN ('dir /b TestScenes\Sponza\sponza.rib') do	( RibRender.exe TestScenes\Sponza\%%a & move *.tif? TestsOutput\%%~na.tif )
