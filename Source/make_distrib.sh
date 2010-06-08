#!/bin/bash

mkdir -p ../Distrib
cp -r DistribSrc/base/* ../Distrib

rm ../Distrib/rendermn.ini

cp -r DistribSrc/linux/* ../Distrib

