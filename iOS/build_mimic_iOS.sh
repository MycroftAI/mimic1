# build script for creating Mimic universal binary for iOS and simulator
# based on: 
# information at: http://blog.karmadust.com/building-universal-libraries-for-ios/
#
# Example build scripts:
#
# build_zlib_iOS.sh
# https://gist.github.com/mmick66/798726f87c21649d9daf
#
# png_build.sh
# https://gist.github.com/wuhao5/8061397

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Makefile in mimic/main/Makefile Requires Modification !!!!!!!!!!!!!!!!!!!!!
## if implementing for the first time in new original source not prepared for OSX or iOS
## For the prefix configure flag to have effect, and install the specific binaries to discrete directories, the cp -pd arg flags in two cases near the bottom of the file must be changed for OSX
## These changes are added and commented out in this version of the code and can be toggled by commenting/uncommenting appropriately near line 135
## -d flag is interpreted differently in OSX - needs to be -R, so cp -pd must be changed to cp -pR  
## see: http://stackoverflow.com/questions/23001775/error-installing-flite-on-mac-osx

TOP_DIR=`pwd`
BUILD_DIR=$TOP_DIR/build
echo BUILD_DIR $BUILD_DIR

# clean previous builds
cd $BUILD_DIR
echo Working directory ${PWD}
rm -rf install_*
rm -rf output-*
cd $TOP_DIR
echo Working directory ${PWD}


# up to /mimic dir
cd ../

echo Working directory ${PWD}

# build armv64  -arch arm64
echo Building arch arm64

./configure --prefix=$BUILD_DIR/install_arm64 CFLAGS="-Ofast -mios-version-min=5.0" LDFLAGS="-flto" CC="xcrun -sdk iphoneos clang -arch arm64" --host=arm
make clean && make && make install

# build armv7
echo Building arch armv7

./configure --prefix=$BUILD_DIR/install_armv7 CFLAGS="-Ofast -mios-version-min=5.0" LDFLAGS="-flto" CC="xcrun -sdk iphoneos clang -arch armv7" --host=arm
make clean && make && make install

# build armv7s
# buiilding for armv7s may not be absolutely required - recent developer forum thread suggest that armv7 runs fine on this arch, but not well documented
echo Building arch armv7s

./configure --prefix=$BUILD_DIR/install_armv7s CFLAGS="-Ofast -mios-version-min=5.0" LDFLAGS="-flto" CC="xcrun -sdk iphoneos clang -arch armv7s" --host=arm
make clean && make && make install

# build simulator i386 and x86_64
echo Building arch i386

./configure --prefix=$BUILD_DIR/install_i386 CFLAGS="-Ofast -mios-version-min=5.0" LDFLAGS="-flto" CC="xcrun -sdk iphonesimulator clang -arch i386"
make clean && make && make install

echo Building arch x86_64

./configure --prefix=$BUILD_DIR/install_x86_64 CFLAGS="-Ofast -mios-version-min=5.0" LDFLAGS="-flto" CC="xcrun -sdk iphonesimulator clang -arch x86_64"
make clean && make && make install

# build universal
echo Building Universal Static Lib

# back to build dir
#cd $TOP_DIR
cd $BUILD_DIR
echo Working directory ${PWD}

# dir for lipo output - universal binary
mkdir -p output-ios
# run lipo to link binaries
xcrun lipo -create $(find install_*/lib -name "libmimic.a") -o output-ios/libmimic.a
# copy inlcude (headers)
cp -r install_armv7/include output-ios/.


