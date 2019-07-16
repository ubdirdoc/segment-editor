#!/bin/bash -eux
if [[ "$TRAVIS_TAG" = "" ]];
then
    exit 0
fi

mv SEGMent*.AppImage "SEGMent-editor-$TRAVIS_TAG-amd64.AppImage"

  # find appdir -executable -type f -exec ldd {} \; | grep " => /usr" | cut -d " " -f 2-3 | sort | uniq # for debugging
  # curl --upload-file APPNAME*.AppImage https://transfer.sh/APPNAME-git.$(git rev-parse --short HEAD)-x86_64.AppImage
  # - wget -c https://github.com/probonopd/uploadtool/raw/master/upload.sh
  # - bash upload.sh APPNAME*.AppImage*