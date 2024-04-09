#!/bin/bash

set -e

if [[ ! -v KEYSTONE_SDK_DIR ]]
then
    echo "KEYSTONE_SDK_DIR not set! Please set this to the location where Keystone SDK has been installed."
    exit 1
fi

cp -r yang/ $KEYSTONE_SDK_DIR/../../build/overlay/root/
cp install_yang_modules.sh $KEYSTONE_SDK_DIR/../../build/overlay/root/

mkdir build_kl && cd build_kl
cmake -DKEYSTONE_LIB=ON ..
make
make install

cd ../cmd/spirs_tee_sdk
source source.sh
mkdir build && cd build
cmake ..
make
make overlay
make image
