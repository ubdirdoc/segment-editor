#!/bin/bash -eux
export TAG=$GITTAGNOV

mv "Score.AppImage" "$BUILD_ARTIFACTSTAGINGDIRECTORY/SEGMent-$TAG-linux-amd64.AppImage"
