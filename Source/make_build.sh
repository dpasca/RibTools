#!/bin/bash

mkdir -p ../build
cd ../build
cmake ../Source

cd ../Source
./make_distrib.sh

