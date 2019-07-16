#!/bin/bash -eux
cmake -DSEGMENT_RELEASE=1 -DDEPLOYMENT_BUILD=1 -DCMAKE_INSTALL_PREFIX=inst -DCMAKE_SKIP_RPATH=ON -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 -DCMAKE_PREFIX_PATH=$PWD/qt-5.9.7

cmake --build . --target all_unity -- -j$(nproc)
cmake --build . --target docs
cmake --build . --target install/strip/fast

find inst/
