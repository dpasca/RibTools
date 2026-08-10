#!/usr/bin/env bash

set -euo pipefail

build_type=${1:-Release}

cmake --build _build --config "${build_type}"
