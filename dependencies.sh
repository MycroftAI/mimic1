#!/bin/sh
#POSIX

# Reset all variables that might be set
enable_gpl="yes"

show_help()
{
cat << EOF
Usage: ${0##*/} [ --enable-gpl | --disable-gpl ] [ other flags passed to configure ]
Install dependencies for mimic

    -h          display this help and exit
    --enable-gpl Builds and installs optional GPL-licensed dependencies
    --disable-gpl Skips GPL dependencies.
EOF
}

case $1 in
  -h|-\?|--help)   # Call a "show_help" function to display a synopsis, then exit.
    show_help
    exit
    ;;
  --disable-gpl)
    enable_gpl="no"
    shift
    ;;
  --enable-gpl)
    enable_gpl="yes"
    shift
    ;;
  *)
    ;;
esac

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

# Install system requirements if any of those are missing.
# This needs testing on all the platforms
if [ -f "/etc/arch-release" ]; then
  pacman -Qs make >/dev/null && \
   pacman -Qs pkg-config >/dev/null && \
   pacman -Qs gcc >/dev/null && \
   pacman -Qs autoconf >/dev/null && \
   pacman -Qs automake >/dev/null && \
   pacman -Qs libtool >/dev/null && \
   pacman -Qs alsa-lib  >/dev/null || \
      ( cat << EOF
Some packages are needed and they need to be installed. You can either stop
this script with Ctrl+C and manually run:
  sudo pacman -S --needed make pkg-config  gcc autoconf automake libtool alsa-lib
or give the sudo password now and let the script install those packages for you
EOF
      sudo pacman -S --needed make pkg-config  gcc autoconf automake libtool alsa-lib
     )
elif [ -f "/etc/debian_version" ]; then
 dpkg -s gcc make pkg-config automake libtool libasound2-dev 2>/dev/null >/dev/null || \
    ( cat << EOF
Some packages are needed and they need to be installed. You can either stop
this script with Ctrl+C and manually run:
    sudo apt-get install gcc make pkg-config automake libtool libasound2-dev
or give the sudo password now and let the script install those packages for you
EOF
    sudo apt-get install gcc make pkg-config automake libtool libasound2-dev
    )
elif [ -f "/etc/redhat-release" ]; then
dnf list installed gcc 2>/dev/null >/dev/null && \
 dnf list installed make 2>/dev/null >/dev/null && \
 dnf list installed pkgconfig 2>/dev/null >/dev/null && \
 dnf list installed automake 2>/dev/null >/dev/null && \
 dnf list installed libtool 2>/dev/null >/dev/null && \
 dnf list installed alsa-lib-devel 2>/dev/null >/dev/null || \
  ( cat << EOF
Some packages are needed and they need to be installed. You can either stop
this script with Ctrl+C and manually run:
    sudo dnf install gcc make pkgconfig automake libtool alsa-lib-devel
or give the sudo password now and let the script install those packages for you
EOF
    sudo dnf install gcc make pkgconfig automake libtool alsa-lib-devel
  )
fi

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

if [ "x${enable_gpl}" = "xyes" ]; then

  "$PKG_CONFIG" --exists saga && HAVE_SAGA="yes" || HAVE_SAGA="no"
  echo "Have SAGA? [${HAVE_SAGA}]"

  if [ "x${HAVE_SAGA}" = "xno" ]; then
    # Prepare saga source:
    (mkdir -p "${WORKDIR}/thirdparty/" && \
      cd "${WORKDIR}/thirdparty" && \
      wget "https://github.com/TALP-UPC/saga/archive/f0148f86b8bc2c4cf27bb186449c8d88082d0d62.zip" && \
      echo "3a9b51e922b26397a040b356bafdbbc7  f0148f86b8bc2c4cf27bb186449c8d88082d0d62.zip" | md5sum -c || exit 1
      mv "f0148f86b8bc2c4cf27bb186449c8d88082d0d62.zip" "saga.zip" || exit 1
      unzip "saga.zip" &&  \
      mv "saga-f0148f86b8bc2c4cf27bb186449c8d88082d0d62" "saga" || exit 1
      cd "saga" && ./autogen.sh)  || exit 1
    # Build saga
    mkdir -p "${WORKDIR}/thirdparty/build_saga" || exit 1
    cd "${WORKDIR}/thirdparty/build_saga" || exit 1
    ${WORKDIR}/thirdparty/saga/configure --disable-option-checking $@ || exit 1
    make || exit 1
    make install && echo "saga installation succeeded" || (
      cat << EOF
Saga was successfully compiled. However, it could not be installed.
The most likely cause is a lack of permissions. This script will try to run
the installation with sudo, asking your password.
EOF
      sudo make install && echo "Saga installation succeeded" || (
        cat << EOF
The installation failed. Please run this or check other possible errors:
  cd "${WORKDIR}/thirdparty/build_saga"
  sudo make install
In case you do not have admin permissions you can install everything to a custom
directory by running dependencies.sh with --prefix="/a/writable/directory"
EOF
        exit 1)
      exit 1 )
  fi
fi
