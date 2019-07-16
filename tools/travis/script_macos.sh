#!/bin/bash
cmake -DSEGMENT_RELEASE=1 -DDEPLOYMENT_BUILD=1 -DCMAKE_INSTALL_PREFIX=inst -DCMAKE_SKIP_RPATH=ON

cmake --build . --target all_unity -- -j$(nproc)
cmake --build . --target docs
cmake --build . --target install/strip/fast

find inst/
