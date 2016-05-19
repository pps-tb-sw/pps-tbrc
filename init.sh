#!/bin/sh

git submodule update --init --recursive # fetch the latest version of xpedaq
git submodule foreach git pull origin master
cd external/xpedaq/quickusb-2.15.2/linux && source $PWD/setup.sh && qusb_link_libs
cd -