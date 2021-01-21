#!/bin/bash

export SCORE_DIR=$PWD

mkdir -p /build || true
cd /build

cmake $SCORE_DIR \
  -GNinja \
  -DSEGMENT_RELEASE=1 \
  -DCMAKE_INSTALL_PREFIX=install

cmake --build .
cmake --build . --target install