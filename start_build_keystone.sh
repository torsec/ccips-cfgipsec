#!/bin/bash

set -e

if [[ ! -v KEYSTONE_SDK_DIR ]]
then
    echo "KEYSTONE_SDK_DIR not set! Please set this to the location where Keystone SDK has been installed."
    exit 1
fi

mkdir build_kl
cd ./build_kl
cmake -DKEYSTONE_LIB=ON ..
make
make install

cd ..
mkdir build_ka
cd ./build_ka
cmake -DKEYSTONE_APP=ON ..
make
make keystone-ccips-package
cp keystone-ccips.ke $KEYSTONE_SDK_DIR/../../build/overlay/root/

cd ..
cp -r yang/ $KEYSTONE_SDK_DIR/../../build/overlay/root/
cp install_yang_modules.sh $KEYSTONE_SDK_DIR/../../build/overlay/root/

cd $KEYSTONE_SDK_DIR/../../build
make image
