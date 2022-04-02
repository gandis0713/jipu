#!/bin/zsh

SCRIPT_DIR=$(dirname "$0")
ROOT_DIR=${FILE_DIR}/../../../ # go to project root dir.

pushd ${ROOT_DIR}

git submodule update --init

rm -rf build

popd # ROOT_DIR

pushd ${SCRIPT_DIR}

./build_vks.sh

popd # SCRIPT_DIR



