#!/bin/bash
unset LD_LIBRARY_PATH
source /opt/hirain-imx-linux/4.14-sumo/environment-setup-aarch64-poky-linux

set -x
rm -rf build
mkdir build;cd build

cmake ..

make install
