#!/bin/bash

mkdir -p _distrib
cp -r DistribSrc/base/* _distrib

rm _distrib/rendermn.ini

cp -r DistribSrc/linux/* _distrib

