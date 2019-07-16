#!/bin/bash -eux
if [[ "$TRAVIS_TAG" = "" ]];
then
    exit 0
fi

mv inst/segment-editor.app "SEGMent-editor-$TRAVIS_TAG.app"
zip -r -9 "SEGMent-editor-$TRAVIS_TAG-macOS.zip" "SEGMent-editor-$TRAVIS_TAG.app"
