# build script for cleaning out all files created by running build_mimic_iOS.sh

TOP_DIR=`pwd`
BUILD_DIR=$TOP_DIR/build

# clean previous builds
cd $BUILD_DIR
echo Working directory ${PWD}
rm -rf install_*
rm -rf output-*
cd $TOP_DIR
echo Working directory ${PWD}