#!/bin/sh

WHAT_TO_RUN="$1"
HOST_TRIPLET=`./config/config.guess`
WORKDIR="$PWD"
export MANIFEST_TOOL=:

crosscompile_portaudio()
{
    # Download & Extract portaudio
    if [ ! -e "pa_stable_v19_20140130.tgz" ]; then 
        wget "http://www.portaudio.com/archives/pa_stable_v19_20140130.tgz"
    fi
    echo "7f220406902af9dca009668e198cbd23  pa_stable_v19_20140130.tgz" | md5sum -c || exit 1
    tar xzf "pa_stable_v19_20140130.tgz" # creates directory "portaudio"
    # Cross compile portaudio:
    mkdir portaudio_build
    cd portaudio_build
    ../portaudio/configure --build="${HOST_TRIPLET}" \
                           --prefix="$WORKDIR/install" \
                            "$@" || exit 1
    make || exit 1
    make install || exit 1
}

crosscompile() 
{
    # Cross compile mimic:
    cd "$WORKDIR" || exit 1
    export PKG_CONFIG_PATH="$WORKDIR/install/lib/pkgconfig/"
    mkdir mimic_build || exit 1
    cd mimic_build || exit 1
    ../configure --build="${HOST_TRIPLET}" \
                 --prefix="$WORKDIR/install" \
                 "$@" || exit 1
    make || exit 1
    make install || exit 1
}

case "${WHAT_TO_RUN}" in
  coverage)
    ./autogen.sh
    ./configure  CFLAGS="--coverage --no-inline" LDFLAGS="--coverage" || exit 1
    make || exit 1
    make check || exit 1
    ./do_gcov.sh
    ;;
  shared)
    ./autogen.sh
    ./configure  --enable-shared || exit 1
    make || exit 1
    make check || exit 1
    ;;
  arm-linux-gnueabihf-gcc)
    export CC=arm-linux-gnueabihf-gcc
    export LD=arm-linux-gnueabihf-ld
    export RANLIB=arm-linux-gnueabihf-ranlib
    export AR=arm-linux-gnueabihf-ar
    #export AS=arm-linux-gnueabihf-as
    ./autogen.sh
    crosscompile --host=arm-linux-gnueabihf --with-audio=none    
    ;;
  winbuild)
    export CC=i586-mingw32msvc-gcc
    export LD=i586-mingw32msvc-ld
    export RANLIB=i586-mingw32msvc-ranlib
    export AR=i586-mingw32msvc-ar
    #export CC=i686-w64-mingw32-gcc
    #export LD=i686-w64-mingw32-ld
    # --host=i686-w64-mingw32
    ./autogen.sh
    #crosscompile_portaudio --host=i586-mingw32msvc --enable-shared
    crosscompile --host=i586-mingw32msvc --with-audio=none
    # Test mimic:
    cd "$WORKDIR" || exit 1
    wine "install/bin/mimic.exe" -voice ap -t "hello world" "hello_world_winbuild.wav"
    ;;
  winbuild_shared)
    export CC=i586-mingw32msvc-gcc
    export LD=i586-mingw32msvc-ld
    export RANLIB=i586-mingw32msvc-ranlib
    export AR=i586-mingw32msvc-ar
    #export CC=i686-w64-mingw32-gcc
    #export LD=i686-w64-mingw32-ld
    # --host=i686-w64-mingw32
    ./autogen.sh
    #crosscompile_portaudio --host=i586-mingw32msvc --enable-shared
    crosscompile --host=i586-mingw32msvc --enable-shared --with-audio=none
    # Test mimic:
    cd "$WORKDIR" || exit 1
    wine "install/bin/mimic.exe" -voice ap -t "hello world" "hello_world_winbuild_shared.wav"
    ;;
  *)
    echo "Unknown WHAT_TO_RUN: ${WHAT_TO_RUN}"
    exit 1
    ;;
esac


