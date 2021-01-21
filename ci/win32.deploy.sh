#!/bin/bash
export SCORE_DIR="$PWD"
export SDK_DIR="$PWD/build/SDK"

# Copy windows binary
(
  cd build
  ls
  mv "ossia score-3.0.0-win64.exe" "$BUILD_ARTIFACTSTAGINGDIRECTORY/ossia score-$GITTAGNOV-win64.exe"
)
