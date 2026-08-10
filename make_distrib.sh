#!/usr/bin/env bash

set -euo pipefail

mkdir -p _distrib
cp -R DistribSrc/base/. _distrib/

rm -f _distrib/rendermn.ini

cp -R DistribSrc/linux/. _distrib/
