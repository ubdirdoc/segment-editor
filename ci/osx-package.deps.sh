#!/bin/bash -eux

set +e

export HOMEBREW_NO_AUTO_UPDATE=1
brew install gnu-tar ninja
wget -nv https://github.com/jcelerier/cninja/releases/download/v3.7.5/cninja-v3.7.5-macOS.tar.gz -O cninja.tgz &

wget -nv https://github.com/phracker/MacOSX-SDKs/releases/download/10.15/MacOSX10.15.sdk.tar.xz &

SDK_ARCHIVE=sdk-macOS.tar.gz
wget -nv https://github.com/ossia/score-sdk/releases/download/sdk18/$SDK_ARCHIVE -O $SDK_ARCHIVE
sudo mkdir -p /opt/ossia-sdk
sudo chown -R $(whoami) /opt
sudo chmod -R a+rwx /opt
gtar xhaf $SDK_ARCHIVE --strip-components=2 --directory /opt/ossia-sdk/
ls /opt/ossia-sdk

sudo rm -rf /Library/Developer/CommandLineTools
sudo rm -rf /usr/local/include/c++

wait wget || true
gtar xhaf cninja.tgz
sudo cp -rf cninja /usr/local/bin/

echo "Copying sdks..."
gtar xhaf MacOSX10.15.sdk.tar.xz
mv MacOSX10.15.sdk /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/
sudo cp -rf cninja /usr/local/bin/
set -e
