#!/bin/bash

set -e

mkdir build_ra
cd ./build_ra
cmake -DENARX_RA=ON ..
make

cd ..
mkdir build_ta
cd ./build_ta
cmake -DENARX_TA=ON ..
make
make install

cd ../cmd/i2nsf_enarx/enarx
cargo build --release --target=wasm32-wasi
