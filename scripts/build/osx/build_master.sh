#!/bin/zsh

pushd $(dirname "$0")/../../../ # go to project root dir.

git submodule update --init

cmake -S . -B build -DVKS_TEST=ON
cmake --build build

popd


