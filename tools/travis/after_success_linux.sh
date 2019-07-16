#!/bin/bash -eux
if [[ "$TRAVIS_TAG" = "" ]];
then
    exit 0
fi

mv SEGMent*.AppImage "SEGMent-editor-$TRAVIS_TAG-amd64.AppImage"