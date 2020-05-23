#!/bin/bash

mkdir -p _build
pushd _build
cmake .. -DCMAKE_BUILD_TYPE=${1:-Release}
popd

./make_distrib.sh

