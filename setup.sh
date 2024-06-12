#!/bin/bash

set -e

git submodule sync --recursive
git submodule update --init --recursive

cd cmd/spirs_tee_sdk
patch -p1 < ../../patches/spirs_tee_sdk.patch
