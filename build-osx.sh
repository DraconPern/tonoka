#!/bin/bash
set -xe

if [ "$1" == "Release" ]
then
  TYPE=Release
else
  TYPE=Debug
fi

BUILD_DIR=`pwd`
DEVSPACE=`pwd`
export MACOSX_DEPLOYMENT_TARGET=10.9
export CPPFLAGS=-D__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES=1
./build.sh $1

cd $BUILD_DIR/build-$TYPE
hdiutil create -volname tonoka -srcfolder $BUILD_DIR/build-$TYPE/tonoka.app -ov -format UDZO tonoka.dmg
