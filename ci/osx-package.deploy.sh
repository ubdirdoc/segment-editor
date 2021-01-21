#!/bin/bash -eux
export TAG=$GITTAGNOV

export HOMEBREW_NO_AUTO_UPDATE=1
export SRC_PATH=$PWD
brew install graphicsmagick imagemagick npm
npm install --global create-dmg

cd $SRC_PATH/install/

cp $SRC_PATH/LICENSE.txt license.txt
create-dmg *.app
ls

mv *.dmg "$BUILD_ARTIFACTSTAGINGDIRECTORY/SEGMent-$TAG-macOS.dmg"
