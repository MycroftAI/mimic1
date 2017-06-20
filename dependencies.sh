#!/bin/sh

# Absolute path to this script, e.g. /home/user/bin/foo.sh
SCRIPT=$(readlink -f "$0")
# Absolute path this script is in, thus /home/user/bin
SCRIPTPATH=$(dirname "$SCRIPT")

CURDIR="$PWD"

if [ "x${BUILDDIR}" = "x" ]; then
    BUILDFROM="$PWD/build"
fi
mkdir -p "$BUILDFROM" || exit 1

if [ "x${PKG_CONFIG}" = "x" ]; then
  PKG_CONFIG=`which pkg-config`
fi

which "$PKG_CONFIG" ||(echo "Please install pkg-config"; exit 1)

"$PKG_CONFIG" --exists libpcre2-8 && HAVE_PCRE2="yes" || HAVE_PCRE2="no"
echo "Have PCRE2? [${HAVE_PCRE2}]"

if [ "x${HAVE_PCRE2}" = "xno" ]; then
  # Prepare pcre2-10.23 source:
  (mkdir -p "${BUILDFROM}/thirdparty/" && \
    cd "${BUILDFROM}/thirdparty" && \
    wget "ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre2-10.23.zip"
    echo "56c07f59ccd052ccdbdedadf24574a63  pcre2-10.23.zip" | md5sum -c --status || exit 1
    unzip "pcre2-10.23.zip" && \
    cd "pcre2-10.23" && \
    patch -p1 < "${SCRIPTPATH}/thirdparty/pcre2.patch" && \
    autoreconf -fi )
  # Build pcre2-10.23:
  mkdir -p "${BUILDFROM}/thirdparty/build_pcre2" || exit 1
  cd "${BUILDFROM}/thirdparty/build_pcre2" || exit 1
  ${BUILDFROM}/thirdparty/pcre2-10.23/configure --disable-option-checking $@ || exit 1
  make || exit 1
  make install && echo "PCRE2 installation succeeded" || (
    cat << EOF
PCRE was successfully compiled. However, it could not be installed.
The most likely cause is a lack of permissions. This script will try to run
the installation with sudo asking your password.
EOF
    sudo make install && echo "PCRE2 installation succeeded" || (
      cat << EOF
The installation failed. Please run this or check other possible errors:
  cd "${BUILDFROM}/thirdparty/build_pcre2"
  sudo make install
In case you do not have admin permissions you can install everything to a custom
directory by running dependencies.sh with --prefix="/a/writable/directory"
EOF
      exit 1)
    exit 1 )
fi

cd "${CURDIR}"

