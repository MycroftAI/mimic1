#!/bin/sh

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

# Absolute path to this script, e.g. /home/user/bin/foo.sh
SCRIPT=$(readlink2 "$0")
# Absolute path this script is in, thus /home/user/bin
SCRIPTPATH=$(dirname "$SCRIPT")

CURDIR="$PWD"

if [ "x${WORKDIR}" = "x" ]; then
    WORKDIR="$PWD/build"
fi
mkdir -p "$WORKDIR" || exit 1

if [ "x${PKG_CONFIG}" = "x" ]; then
  PKG_CONFIG=`which pkg-config`
fi

which "$PKG_CONFIG" ||(echo "Please install pkg-config"; exit 1)

"$PKG_CONFIG" --exists libpcre2-8 && HAVE_PCRE2="yes" || HAVE_PCRE2="no"
echo "Have PCRE2? [${HAVE_PCRE2}]"

if [ "x${HAVE_PCRE2}" = "xno" ]; then
  # Prepare pcre2-10.23 source:
  (mkdir -p "${WORKDIR}/thirdparty/" && \
    cd "${WORKDIR}/thirdparty" && \
    wget "ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre2-10.23.zip"
    echo "56c07f59ccd052ccdbdedadf24574a63  pcre2-10.23.zip" | md5sum -c || exit 1
    unzip "pcre2-10.23.zip" && \
    cd "pcre2-10.23" && \
    patch -p1 < "${SCRIPTPATH}/thirdparty/pcre2.patch" && \
    autoreconf -fi ) || exit 1
  # Build pcre2-10.23:
  mkdir -p "${WORKDIR}/thirdparty/build_pcre2" || exit 1
  cd "${WORKDIR}/thirdparty/build_pcre2" || exit 1
  ${WORKDIR}/thirdparty/pcre2-10.23/configure --disable-option-checking $@ || exit 1
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
  cd "${WORKDIR}/thirdparty/build_pcre2"
  sudo make install
In case you do not have admin permissions you can install everything to a custom
directory by running dependencies.sh with --prefix="/a/writable/directory"
EOF
      exit 1)
    exit 1 )
fi

cd "${CURDIR}"

