#!/bin/bash

brew install boost
pip install sphinx quark-sphinx-theme gnu-tar wget
wget https://github.com/ubdirdoc/segment-editor/releases/download/sdk/qt-5.9.7-macos.tar.gz 
gtar xaf qt-5.9.7-macos.tar.gz 