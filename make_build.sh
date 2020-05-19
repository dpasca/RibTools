#!/bin/bash

mkdir -p _build
pushd _build
cmake ..
popd

./make_distrib.sh

