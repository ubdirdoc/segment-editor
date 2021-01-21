#!/bin/bash -eux

export TAG=$GITTAGNOV

mv "segment.tar.xz" "$BUILD_ARTIFACTSTAGINGDIRECTORY/segment-$TAG-src.tar.xz"