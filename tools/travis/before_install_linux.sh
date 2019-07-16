#!/bin/bash

sudo apt-get install apt-transport-https ca-certificates gnupg software-properties-common wget

wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add -

sudo add-apt-repository -y ppa:beineri/opt-qt-5.12.3-xenial
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo add-apt-repository -y ppa:mhier/libboost-latest
sudo apt-add-repository -y 'deb https://apt.kitware.com/ubuntu/ xenial main'

sudo apt-get update -qq

sudo apt-get install kitware-archive-keyring

sudo apt-key --keyring /etc/apt/trusted.gpg del C1F34CDD40CD72DA

sudo pip install sphinx quark-sphinx-theme
