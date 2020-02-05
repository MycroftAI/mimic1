#!/bin/sh

#### Help message ####
if [ "$#" -eq 0 ]; then
  echo "./run_testsuite.sh requires a task/build target to be done"
  echo "  - ./run_testsuite.sh osx (OSX install dependencies, build and test)"
  echo "  - ./run_testsuite.sh ios (OSX install dependencies, build universal lib)"
  echo "  - ./run_testsuite.sh coverage (Build with code coverage)"
  echo "  - ./run_testsuite.sh shared (Build with shared libraries)"
  echo "  - ./run_testsuite.sh gcc6 (Build using gcc6)"
  echo "  - ./run_testsuite.sh arm-linux-gnueabihf-gcc (crosscompiling)"
  echo "  - ./run_testsuite.sh winbuild (crosscompiling)"
  echo "  - ./run_testsuite.sh winbuild_shared (crosscompiling, with shared libraries)"
fi 

#### This function emulates readlink -f, not available on osx ####
readlink2()
{
MYWD=`pwd`
TARGET_FILE=$1

cd `dirname $TARGET_FILE`
TARGET_FILE=`basename $TARGET_FILE`

# Iterate down a (possible) chain of symlinks
while [ -L "$TARGET_FILE" ]
do
    TARGET_FILE=`readlink $TARGET_FILE`
    cd `dirname $TARGET_FILE`
    TARGET_FILE=`basename $TARGET_FILE`
done

# Compute the canonicalized name by finding the physical path 
# for the directory we're in and appending the target file.
PHYS_DIR=`pwd -P`
RESULT=$PHYS_DIR/$TARGET_FILE
# restore working directory
cd "$MYWD"
echo $RESULT
}

# Cross-compilation in Windows looks for a "Manifest tool". autotools
# mistakenly finds `/bin/mt` and assumes it is the manifest tool, even though
# it is an unrelated program. This export makes sure that autotools does not
# find the wrong manifest tool.
export MANIFEST_TOOL=:


# The task we want to run
WHAT_TO_RUN="$1"

# Set up an environment variable to increase compilation speed (and RAM usage) 
if [ "x${NCORES}" = "x" ]; then
    NCORES=1
fi

# The directory where the mimic source code is.
export MIMIC_TOP_SRCDIR=`dirname \`readlink2 "$0"\``


# The following functions are used later on. They assume the following variables
# will be set:
# Assumes MIMIC_TOP_SRCDIR points to mimic sources directory (where configure is)
# Assumes MIMIC_INSTALL_DIR points to where mimic will be installed
# Assumes WORKDIR points to a directory where build items can be placed

crosscompile_dependencies()
{
    # Ubuntu precise & trusty bug (inherited from debian):
    # ${HOST_TRIPLET}-pkg-config ignores PKG_CONFIG_PATH
    # We need PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig/"
    # so we use pkg-config without the triplet.
    # and we set PKG_CONFIG_PATH manually to the right search paths
    # pkg-config in Debian stretch and in Ubuntu xenial have this bug fixed so
    # in the future the ${HOST_TRIPLET}-pkg-config can be used with a simple
    # PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig/"
    (cd "${WORKDIR}" && \
      PKG_CONFIG_LIBDIR="" \
      PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig/:/usr/lib/${HOST_TRIPLET}/pkgconfig:/usr/${HOST_TRIPLET}/lib/pkgconfig" \
      PKG_CONFIG=`which pkg-config` \
        ${MIMIC_TOP_SRCDIR}/dependencies.sh \
        --prefix="${MIMIC_INSTALL_DIR}" \
        --build="${BUILD_TRIPLET}" \
        --host="${HOST_TRIPLET}" \
                 "$@" ) || exit 1
}

crosscompile_portaudio()
{
    # Download & Extract portaudio
    if [ ! -e "${WORKDIR}/pa_stable_v190600_20161030.tgz" ]; then 
        wget -O "${WORKDIR}/pa_stable_v190600_20161030.tgz" "http://www.portaudio.com/archives/pa_stable_v190600_20161030.tgz"
    fi
    echo "4df8224e047529ca9ad42f0521bf81a8  ${WORKDIR}/pa_stable_v190600_20161030.tgz" | md5sum -c || exit 1
    tar xzf "${WORKDIR}/pa_stable_v190600_20161030.tgz" -C "${WORKDIR}" # creates directory "portaudio"

    # Patch makefile due to bug:
    # http://sites.music.columbia.edu/pipermail/portaudio/2014-April/016026.html
    sed -i -e 's:src/hostapi/wmme \\:src/hostapi/wmme src/hostapi/skeleton \\:' "${WORKDIR}/portaudio/Makefile.in" || exit 1

    # Cross compile portaudio:
    mkdir -p "${WORKDIR}/portaudio_build"
    (cd "${WORKDIR}/portaudio_build" && \
    ../portaudio/configure --build="${BUILD_TRIPLET}" \
                           --host="${HOST_TRIPLET}" \
                           --prefix="${MIMIC_INSTALL_DIR}" \
                           "$@" && \
    make -j ${NCORES} && make install) || exit 1
}

crosscompile_mimic() 
{
    # Cross compile mimic:
    mkdir -p "${WORKDIR}/mimic_build" || exit 1
    ( cd "${WORKDIR}/mimic_build" && \
    # Ubuntu precise & trusty bug (inherited from debian):
    # ${HOST_TRIPLET}-pkg-config ignores PKG_CONFIG_PATH
    # We need PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig/"
    # so we use pkg-config without the triplet.
    # and we set PKG_CONFIG_PATH manually to the right search paths
    # pkg-config in Debian stretch and in Ubuntu xenial have this bug fixed so
    # in the future the ${HOST_TRIPLET}-pkg-config can be used with a simple
    # PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig/"
    ${MIMIC_TOP_SRCDIR}/configure \
                 --build="${BUILD_TRIPLET}" \
                 --host="${HOST_TRIPLET}" \
                 --prefix="${MIMIC_INSTALL_DIR}" \
                 PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig/:/usr/lib/${HOST_TRIPLET}/pkgconfig:/usr/${HOST_TRIPLET}/lib/pkgconfig" \
                 PKG_CONFIG_LIBDIR="" \
                 PKG_CONFIG=`which pkg-config` \
                 "$@" && \
    make -j ${NCORES} &&  make install ) || exit 1
}

put_dll_in_bindir()
{
    # Portaudio libraries are installed into lib. wine can't find them.
    # Copy all libs to ${MIMIC_INSTALL_DIR}/bin
    for file in `ls "${MIMIC_INSTALL_DIR}/lib/"*.dll`; do
        cp "$file" "${MIMIC_INSTALL_DIR}/bin/"
    done
}

fix_portaudio_pc_file()
{
  # can't find uuid in mingw. I just remove it and hope mimic still works.
  sed -i -e 's:-luuid::g' "${MIMIC_INSTALL_DIR}/lib/pkgconfig/portaudio-2.0.pc"
}

run_mimic_autogen()
{
    (cd "${MIMIC_TOP_SRCDIR}" && ./autogen.sh) || exit 1
}

set_build_and_install_dir()
{
    MIMIC_INSTALL_DIR=`pwd`"/install/${WHAT_TO_RUN}"
    export WORKDIR=`pwd`"/builds/${WHAT_TO_RUN}"
    mkdir -p "${MIMIC_INSTALL_DIR}"
    mkdir -p "${WORKDIR}"
}

test_windows_build()
{
    # Test mimic:
    cd "$WORKDIR" || exit 1
    if [ "x${DISPLAY}" = "x" ]; then
      xvfb-run wine "${MIMIC_INSTALL_DIR}/bin/mimic.exe" -voice ap -t "hello world" "hello_world_winbuild.wav" || exit 1
    else
      wine "${MIMIC_INSTALL_DIR}/bin/mimic.exe" -voice ap -t "hello world" "hello_world_winbuild.wav" || exit 1
    fi
    echo "fbe80cc64ed244c0ee02c62a8489f182  hello_world_winbuild.wav" | md5sum -c || exit 1
}

set_windows_triplet()
{
    export BUILD_TRIPLET=`sh ./config/config.guess`
    export HOST_TRIPLET="i686-w64-mingw32"
}

compile_dependencies()
{
    (cd "${WORKDIR}" && ${MIMIC_TOP_SRCDIR}/dependencies.sh --prefix="${MIMIC_INSTALL_DIR}") || exit 1
}

compile_mimic()
{
  (cd "$WORKDIR" && \
    ${MIMIC_TOP_SRCDIR}/configure \
      PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig" \
      --prefix="${MIMIC_INSTALL_DIR}" \
      "$@" && \
    make -j ${NCORES} && \
    make check ) || exit 1
}

do_gcov()
{
  for direct in $(find . -type d); do
    (cd "$direct"; 
      for file in $(ls | grep "\\.gcno$"); do
        echo "$PWD";
        gcov -b "$file";
      done)
  done
}

case "${WHAT_TO_RUN}" in
  osx)
    brew update
    brew install pkg-config automake libtool portaudio pcre2
    set_build_and_install_dir
    run_mimic_autogen
    compile_mimic
    ;;
  ios)
    brew update
    brew install pkg-config automake libtool md5sha1sum
    run_mimic_autogen
    # arm64
    MIMIC_INSTALL_DIR=`pwd`"/install/ios_arm64"
    export WORKDIR=`pwd`"/builds/ios_arm64"
    mkdir -p "${MIMIC_INSTALL_DIR}"
    mkdir -p "${WORKDIR}"
    export CC="xcrun -sdk iphoneos clang -arch arm64"
    export CFLAGS="-Ofast -mios-version-min=5.0"
    export LDFLAGS="-flto"
  (cd "$WORKDIR" && \
    ${MIMIC_TOP_SRCDIR}/dependencies.sh \
      PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig" \
      PKG_CONFIG_LIBDIR="" \
      --prefix="${MIMIC_INSTALL_DIR}" \
       --host=arm ) || exit 1
  (cd "$WORKDIR" && \
    ${MIMIC_TOP_SRCDIR}/configure \
      PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig" \
      PKG_CONFIG_LIBDIR="" \
      --prefix="${MIMIC_INSTALL_DIR}" \
      --with-audio=none --disable-voices-all \
      --host=arm && \
    make -j ${NCORES} && \
    make install) || exit 1

    # armv7
#    MIMIC_INSTALL_DIR=`pwd`"/install/ios_armv7"
#    export WORKDIR=`pwd`"/builds/ios_armv7"
#    mkdir -p "${MIMIC_INSTALL_DIR}"
#    mkdir -p "${WORKDIR}"
#    export CC="xcrun -sdk iphoneos clang -arch armv7"
#    export CFLAGS="-Ofast -mios-version-min=5.0"
#    export LDFLAGS="-flto"
#(cd "$WORKDIR" && \
#    ${MIMIC_TOP_SRCDIR}/dependencies.sh \
#      PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig" \
#      PKG_CONFIG_LIBDIR="" \
#      --prefix="${MIMIC_INSTALL_DIR}" \
#      --host=arm ) || exit 1
#  (cd "$WORKDIR" && \
#    ${MIMIC_TOP_SRCDIR}/configure \
#      PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig" \
#      PKG_CONFIG_LIBDIR="" \
#      --prefix="${MIMIC_INSTALL_DIR}" \
#      --with-audio=none --disable-voices-all \
#      CFLAGS="-Ofast -mios-version-min=5.0" LDFLAGS="-flto" CC="xcrun -sdk iphoneos clang -arch armv7" --host=arm && \
#    make -j ${NCORES} && \
#    make install) || exit 1

    # armv7s
#    MIMIC_INSTALL_DIR=`pwd`"/install/ios_armv7s"
#    export WORKDIR=`pwd`"/builds/ios_armv7s"
#    mkdir -p "${MIMIC_INSTALL_DIR}"
#    mkdir -p "${WORKDIR}"
#    export CC="xcrun -sdk iphoneos clang -arch armv7s"
#    export CFLAGS="-Ofast -mios-version-min=5.0"
#    export LDFLAGS="-flto"
#(cd "$WORKDIR" && \
#    ${MIMIC_TOP_SRCDIR}/dependencies.sh \
#      PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig" \
#      PKG_CONFIG_LIBDIR="" \
#      --prefix="${MIMIC_INSTALL_DIR}" \
#      --host=arm ) || exit 1
#  (cd "$WORKDIR" && \
#    ${MIMIC_TOP_SRCDIR}/configure \
#      PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig" \
#      PKG_CONFIG_LIBDIR="" \
#      --prefix="${MIMIC_INSTALL_DIR}" \
#      --with-audio=none --disable-voices-all \
#      --host=arm && \
#    make -j ${NCORES} && \
#    make install) || exit 1

  # i386
#    MIMIC_INSTALL_DIR=`pwd`"/install/ios_i386"
#    export WORKDIR=`pwd`"/builds/ios_i386"
#    mkdir -p "${MIMIC_INSTALL_DIR}"
#    mkdir -p "${WORKDIR}"
#    export CC="xcrun -sdk iphonesimulator clang -arch i386"
#    export CFLAGS="-Ofast -mios-version-min=5.0"
#    export LDFLAGS="-flto"
#(cd "$WORKDIR" && \
#    ${MIMIC_TOP_SRCDIR}/dependencies.sh \
#      PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig" \
#      PKG_CONFIG_LIBDIR="" \
#      --prefix="${MIMIC_INSTALL_DIR}"  ) || exit 1
#  (cd "$WORKDIR" && \
#    ${MIMIC_TOP_SRCDIR}/configure \
#      PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig" \
#      PKG_CONFIG_LIBDIR="" \
#      --prefix="${MIMIC_INSTALL_DIR}" \
#      --with-audio=none --disable-voices-all && \
#    make -j ${NCORES} && \
#    make install) || exit 1

  # x86_64
#    MIMIC_INSTALL_DIR=`pwd`"/install/ios_x86_64"
#    export WORKDIR=`pwd`"/builds/ios_x86_64"
#    mkdir -p "${MIMIC_INSTALL_DIR}"
#    mkdir -p "${WORKDIR}"
#    export CC="xcrun -sdk iphonesimulator clang -arch x86_64"
#    export CFLAGS="-Ofast -mios-version-min=5.0"
#    export LDFLAGS="-flto"
#(cd "$WORKDIR" && \
#    ${MIMIC_TOP_SRCDIR}/dependencies.sh \
#      PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig" \
#      PKG_CONFIG_LIBDIR="" \
#      --prefix="${MIMIC_INSTALL_DIR}"  ) || exit 1
#  (cd "$WORKDIR" && \
#    ${MIMIC_TOP_SRCDIR}/configure \
#      PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig" \
#      PKG_CONFIG_LIBDIR="" \
#      --prefix="${MIMIC_INSTALL_DIR}" \
#      --with-audio=none --disable-voices-all && \
#    make -j ${NCORES} && \
#    make install) || exit 1

#    echo "Building Universal Static Lib"
#    mkdir -p "install/ios_universal"
#    # run lipo to link binaries
#    for mylib in `ls install/ios_armv7/lib/*.a`; do
#        libname=`basename "${mylib}"`
#        echo "Processing ${libname} using lipo..."
#    xcrun lipo -create `find install/*/lib -name "${libname}"` -o "install/ios_universal/${libname}.a" || exit 1
#    done
#    cp -r "install/ios_armv7/include" "install/ios_universal/"
#    echo "Universal lib found in install/ios_universal"

    ;;
  coverage)
    MIMIC_INSTALL_DIR=`pwd`"/install/${WHAT_TO_RUN}"
    WORKDIR=`pwd` # do_gcov needs to be adapted to out of tree builds
    mkdir -p "${MIMIC_INSTALL_DIR}"
    compile_dependencies
    run_mimic_autogen
    compile_mimic CFLAGS="$CFLAGS --coverage --no-inline" LDFLAGS="$LDFLAGS --coverage" 
    do_gcov
    ;;
  shared)
    set_build_and_install_dir
    compile_dependencies
    run_mimic_autogen
    compile_mimic --enable-shared CFLAGS="$CFLAGS --std=c99"
    ;;
  gcc6)
      export CC="/usr/bin/gcc-6"
      export CXX="/usr/bin/g++-6"
    set_build_and_install_dir
    compile_dependencies
    run_mimic_autogen
    compile_mimic --enable-shared CFLAGS="$CFLAGS --std=c99"
    ;;
  arm-linux-gnueabihf-gcc)
    set_build_and_install_dir
    run_mimic_autogen
    export BUILD_TRIPLET=`sh ./config/config.guess`
    export HOST_TRIPLET="arm-linux-gnueabihf"
    crosscompile_dependencies
    crosscompile_mimic --with-audio=none    
    ;;
  winbuild)
    set_build_and_install_dir
    run_mimic_autogen
    set_windows_triplet
    crosscompile_dependencies
    crosscompile_portaudio --disable-shared --enable-static
    crosscompile_mimic  --disable-shared --enable-static --with-audio=portaudio
    put_dll_in_bindir
    #test_windows_build
    ;;
  winbuild_shared)
    set_build_and_install_dir
    run_mimic_autogen
    set_windows_triplet
    crosscompile_dependencies
    crosscompile_portaudio --enable-shared
    fix_portaudio_pc_file
    crosscompile_mimic --enable-shared --with-audio=portaudio
    put_dll_in_bindir
    #test_windows_build
    ;;
  *)
    echo "Unknown WHAT_TO_RUN: ${WHAT_TO_RUN}"
    exit 1
    ;;
esac

