#!/usr/bin/env bash

set -euo pipefail

build_type=${1:-Release}
script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)

cd "${script_dir}"

cmake -S . -B _build -DCMAKE_BUILD_TYPE="${build_type}"
bash ./make_distrib.sh
cmake --build _build --config "${build_type}"
