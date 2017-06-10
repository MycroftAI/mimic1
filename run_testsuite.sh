#!/bin/sh

if [ "$#" -eq 0 ]; then
  echo "./run_testsuite.sh requires a task/build target to be done"
  echo "  - ./run_testsuite.sh osx (OSX install dependencies, build and test)"
  echo "  - ./run_testsuite.sh coverage (Build with code coverage)"
  echo "  - ./run_testsuite.sh shared (Build with shared libraries)"
  echo "  - ./run_testsuite.sh gcc6 (Build using gcc6)"
  echo "  - ./run_testsuite.sh arm-linux-gnueabihf-gcc (crosscompiling)"
  echo "  - ./run_testsuite.sh winbuild (crosscompiling)"
  echo "  - ./run_testsuite.sh winbuild_shared (crosscompiling, with shared libraries)"
fi 

WHAT_TO_RUN="$1"

MIMIC_TOP_SRCDIR=`dirname \`readlink -f "$0"\``

export MANIFEST_TOOL=:

if [ "x${NCORES}" = "x" ]; then
    NCORES=1
fi

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
      CC=${HOST_TRIPLET}-gcc \
      LD=${HOST_TRIPLET}-ld \
      RANLIB=${HOST_TRIPLET}-ranlib \
      AR=${HOST_TRIPLET}-ar \
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
                           CC=${HOST_TRIPLET}-gcc \
                           CXX=${HOST_TRIPLET}-g++ \
                           LD=${HOST_TRIPLET}-ld \
                           RANLIB=${HOST_TRIPLET}-ranlib \
                           AR=${HOST_TRIPLET}-ar \
                           "$@" && \
    make -j ${NCORES} &&  make install) || exit 1
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
    ${MIMIC_TOP_SRCDIR}/configure --build="${BUILD_TRIPLET}" \
                 --host="${HOST_TRIPLET}" \
                 --prefix="${MIMIC_INSTALL_DIR}" \
                 CC=${HOST_TRIPLET}-gcc \
                 LD=${HOST_TRIPLET}-ld \
                 RANLIB=${HOST_TRIPLET}-ranlib \
                 AR=${HOST_TRIPLET}-ar \
                 PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig/:/usr/lib/${HOST_TRIPLET}/pkgconfig:/usr/${HOST_TRIPLET}/lib/pkgconfig" \
                 PKG_CONFIG_LIBDIR="" \
                 PKG_CONFIG=`which pkg-config` \
                 "$@" && \
    make -j ${NCORES} &&  make install ) || exit 1
}

put_dll_in_bindir()
{
    # if mingw32, then copy a DLL: (not needed with mingw-w64)
    if [ "x${HOST_TRIPLET}" = "xi586-mingw32msvc" ]; then
      # This one is needed from the mingw32-runtime package
      if [ -f /usr/share/doc/mingw32-runtime/mingwm10.dll.gz ]; then
          cat /usr/share/doc/mingw32-runtime/mingwm10.dll.gz | gunzip > "${MIMIC_INSTALL_DIR}/bin/mingwm10.dll" || exit 1
      else
          # it seems travis does not find it, so we get it directly from the package
          (  cd "${WORKDIR}" && \
       apt-get download mingw32-runtime && \
              ar p mingw32-runtime*.deb data.tar.gz | tar zx && \
             cat usr/share/doc/mingw32-runtime/mingwm10.dll.gz | gunzip > "${MIMIC_INSTALL_DIR}/bin/mingwm10.dll" ) || exit 1
      fi
    fi
    # ICU and portaudio libraries are installed into lib. wine can't find them.
    # Copy all libs to ${MIMIC_INSTALL_DIR}/bin
    for file in `ls "${MIMIC_INSTALL_DIR}/lib/"*.dll`; do cp "$file" "${MIMIC_INSTALL_DIR}/bin/"; done
}

fix_portaudio_pc_file()
{
    # this is a hack not needed with mingw-w64
    if [ "x${HOST_TRIPLET}" = "xi586-mingw32msvc" ]; then
      # uuid is not a dll in mingw. I just remove it and hope mimic still works.
      sed -i -e 's:-luuid::g' "${MIMIC_INSTALL_DIR}/lib/pkgconfig/portaudio-2.0.pc"
    fi
}

case "${WHAT_TO_RUN}" in
  osx)
    brew install pkg-config automake libtool portaudio pcre2
    (cd "${MIMIC_TOP_SRCDIR}" && ./autogen.sh) || exit 1
    ./configure || exit 1
    make -j ${NCORES} || exit 1
    make check || exit 1
    ;;
  coverage)
    MIMIC_INSTALL_DIR=`pwd`"/install/${WHAT_TO_RUN}"
    cd "${MIMIC_TOP_SRCDIR}"
    ./dependencies.sh --prefix="${MIMIC_INSTALL_DIR}" || exit 1
    PKG_CONFIG_PATH=`pkg-config --variable pc_path pkg-config`
    PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig:${PKG_CONFIG_PATH}"
    ./autogen.sh || exit 1
    ./configure  CFLAGS="$CFLAGS --coverage --no-inline" LDFLAGS="$LDFLAGS --coverage" PKG_CONFIG_PATH="${PKG_CONFIG_PATH}" || exit 1
    make -j ${NCORES} || exit 1
    make check || exit 1
    ./do_gcov.sh
    ;;
  shared)
    MIMIC_INSTALL_DIR=`pwd`"/install/${WHAT_TO_RUN}"
    WORKDIR=`pwd`"/builds/${WHAT_TO_RUN}"
    mkdir -p "${MIMIC_INSTALL_DIR}"
    mkdir -p "${WORKDIR}"
    (cd "${WORKDIR}" && ${MIMIC_TOP_SRCDIR}/dependencies.sh --prefix="${MIMIC_INSTALL_DIR}") || exit 1
    (cd "${MIMIC_TOP_SRCDIR}" && ./autogen.sh) || exit 1
    PKG_CONFIG_PATH=`pkg-config --variable pc_path pkg-config`
    PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig:${PKG_CONFIG_PATH}"
    (cd "$WORKDIR" && \
      ${MIMIC_TOP_SRCDIR}/configure --enable-shared \
     --prefix="${MIMIC_INSTALL_DIR}" \
          CFLAGS="$CFLAGS --std=c99" \
          PKG_CONFIG_PATH="${PKG_CONFIG_PATH}" && \
      make -j ${NCORES} && \
      make check ) || exit 1
    ;;
  gcc6)
    export CC="/usr/bin/gcc-6"
    export CXX="/usr/bin/g++-6"
    MIMIC_INSTALL_DIR=`pwd`"/install/${WHAT_TO_RUN}"
    WORKDIR=`pwd`"/builds/${WHAT_TO_RUN}"
    mkdir -p "${MIMIC_INSTALL_DIR}"
    mkdir -p "${WORKDIR}"
    (cd "${WORKDIR}" && ${MIMIC_TOP_SRCDIR}/dependencies.sh --prefix="${MIMIC_INSTALL_DIR}") || exit 1
    (cd "${MIMIC_TOP_SRCDIR}" && ./autogen.sh) || exit 1
    export CFLAGS="$CFLAGS --std=c99"
    PKG_CONFIG_PATH=`pkg-config --variable pc_path pkg-config`
    PKG_CONFIG_PATH="${MIMIC_INSTALL_DIR}/lib/pkgconfig:${PKG_CONFIG_PATH}"
    (cd "$WORKDIR" && \
      ${MIMIC_TOP_SRCDIR}/configure --enable-shared \
     --prefix="${MIMIC_INSTALL_DIR}" \
          CFLAGS="$CFLAGS --std=c99" \
          PKG_CONFIG_PATH="${PKG_CONFIG_PATH}" && \
      make -j ${NCORES} && \
      make check ) || exit 1
    ;;
  arm-linux-gnueabihf-gcc)
    MIMIC_INSTALL_DIR=`pwd`"/install/${WHAT_TO_RUN}"
    WORKDIR=`pwd`"/builds/${WHAT_TO_RUN}"
    mkdir -p "${MIMIC_INSTALL_DIR}"
    mkdir -p "${WORKDIR}"
    (cd "${MIMIC_TOP_SRCDIR}" && ./autogen.sh) || exit 1
    export BUILD_TRIPLET=`sh ./config/config.guess`
    export HOST_TRIPLET="arm-linux-gnueabihf"
  crosscompile_dependencies
    crosscompile_mimic --with-audio=none    
    ;;
  winbuild)
    MIMIC_INSTALL_DIR=`pwd`"/install/${WHAT_TO_RUN}"
    WORKDIR=`pwd`"/builds/${WHAT_TO_RUN}"
    mkdir -p "${MIMIC_INSTALL_DIR}"
    mkdir -p "${WORKDIR}"
    (cd "${MIMIC_TOP_SRCDIR}" && ./autogen.sh) || exit 1
    export BUILD_TRIPLET=`sh ./config/config.guess`
    if [ `which i586-mingw32msvc-gcc` ]; then
        export HOST_TRIPLET="i586-mingw32msvc"
    elif [ `which i686-w64-mingw32-gcc` ]; then
        export HOST_TRIPLET="i686-w64-mingw32"
    else
        echo "No windows cross-compiler found"
        exit 1
    fi
    crosscompile_dependencies
    crosscompile_portaudio --disable-shared --enable-static
    crosscompile_mimic  --disable-shared --enable-static --with-audio=portaudio
    put_dll_in_bindir
    # Test mimic:
    cd "$WORKDIR" || exit 1
    if [ "x${DISPLAY}" = "x" ]; then
      xvfb-run wine "${MIMIC_INSTALL_DIR}/bin/mimic.exe" -voice ap -t "hello world" "hello_world_winbuild.wav" || exit 1
    else
      wine "${MIMIC_INSTALL_DIR}/bin/mimic.exe" -voice ap -t "hello world" "hello_world_winbuild.wav" || exit 1
    fi
    echo "fbe80cc64ed244c0ee02c62a8489f182  hello_world_winbuild.wav" | md5sum -c || exit 1
    ;;
  winbuild_shared)
    MIMIC_INSTALL_DIR=`pwd`"/install/${WHAT_TO_RUN}"
    WORKDIR=`pwd`"/builds/${WHAT_TO_RUN}"
    mkdir -p "${MIMIC_INSTALL_DIR}"
    mkdir -p "${WORKDIR}"
    (cd "${MIMIC_TOP_SRCDIR}" && ./autogen.sh) || exit 1
    export BUILD_TRIPLET=`sh ./config/config.guess`
    if [ `which i586-mingw32msvc-gcc` ]; then
        export HOST_TRIPLET="i586-mingw32msvc"
    elif [ `which i686-w64-mingw32-gcc` ]; then
        export HOST_TRIPLET="i686-w64-mingw32"
    else
        echo "No windows cross-compiler found"
        exit 1
    fi
    crosscompile_dependencies
    crosscompile_portaudio --enable-shared
    fix_portaudio_pc_file
    crosscompile_mimic --enable-shared --with-audio=portaudio
    put_dll_in_bindir
    # Test mimic:
    cd "$WORKDIR" || exit 1
    if [ "x${DISPLAY}" = "x" ]; then
      xvfb-run wine "${MIMIC_INSTALL_DIR}/bin/mimic.exe" -voice ap -t "hello world" "hello_world_winbuild.wav" || exit 1
    else
      wine "${MIMIC_INSTALL_DIR}/bin/mimic.exe" -voice ap -t "hello world" "hello_world_winbuild.wav" || exit 1
    fi
    echo "fbe80cc64ed244c0ee02c62a8489f182  hello_world_winbuild.wav" | md5sum -c || exit 1
    ;;
  *)
    echo "Unknown WHAT_TO_RUN: ${WHAT_TO_RUN}"
    exit 1
    ;;
esac

