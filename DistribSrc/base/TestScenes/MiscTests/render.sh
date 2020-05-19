#!/bin/bash

for file in *.rib
do
    ../../RibRender $file

    filename=$(basename "$file")
    filename=${filename%.*}
    mv *.tif? ../../TestOutput/ $filename.tif
done
