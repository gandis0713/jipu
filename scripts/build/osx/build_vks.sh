#!/bin/zsh

SCRIPT_DIR=$(dirname "$0")
ROOT_DIR=${SCRIPT_DIR}/../../../ # go to project root dir.

pushd ${ROOT_DIR}

cmake -S . -B build -DVKS_TEST=ON
cmake --build build

popd # ROOT_DIR