#!/usr/bin/env bash

set -euo pipefail

preset=${1:-release}
script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)

cd "${script_dir}"

case "${preset}" in
    Release|release)
        preset=release
        ;;
    Debug|debug)
        preset=debug
        ;;
    Sanitize|sanitize)
        preset=sanitize
        ;;
    *)
        echo "Unknown build preset: ${preset}" >&2
        echo "Expected release, debug, or sanitize." >&2
        exit 2
        ;;
esac

cmake --preset "${preset}"
cmake --build --preset "${preset}" --parallel
