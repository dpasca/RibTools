pushd _distrib
cmake --build ../_build --config ${1:-Release}
popd
