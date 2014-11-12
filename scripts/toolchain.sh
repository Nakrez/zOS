#! /bin/sh

if [ $# -ge 1 ]; then
    echo "Build toolchain"
    echo "Useful variables:"
    echo -e '\t$BUILD_PATH: Directory used to build files (/tmp)'
    echo -e '\t$TARGET: Target architecture (i686-elf)'
    echo -e '\t$PREFIX: Installation path (/opt/cross)'
    echo -e '\t$BINUTILS_VERSION: Version of binutils to download (2.24)'
    echo -e '\t$GCC_VERSION: Version of gcc to download (4.9.2)'

    exit 0
fi

CURDIR=`pwd`

BUILD_PATH=${BUILD_PATH-"/tmp"}

TARGET=${TARGET-"i686-elf"}
BINUTILS_VERSION=${BINUTILS_VERSION-"2.24"}
GCC_VERSION=${GCC_VERSION-"4.9.2"}
PREFIX=${PREFIX-"/opt/cross"}

# Binutils variable
BINUTILS_TAR="${BUILD_PATH}/binutils-${BINUTILS_VERSION}.tar.bz2"
BINUTILS_DIR="${BUILD_PATH}/binutils-${BINUTILS_VERSION}"
BINUTILS_BUILD="${BUILD_PATH}/build-binutils"
BINUTILS_URL="ftp://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.bz2"

# Gcc variables
GCC_TAR="${BUILD_PATH}/gcc-${GCC_VERSION}.tar.bz2"
GCC_DIR="${BUILD_PATH}/gcc-${GCC_VERSION}"
GCC_BUILD="${BUILD_PATH}/build-gcc"
GCC_URL="ftp://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.bz2"

# Download gcc
if ! [ -e $BINUTILS_TAR ]; then
    wget -O $BINUTILS_TAR $BINUTILS_URL

    if [ $? -ne 0 ]; then
        exit 1
    fi
fi

if ! [ -e $GCC_TAR ]; then
    wget -O $GCC_TAR $GCC_URL

    if [ $? -ne 0 ]; then
        exit 1
    fi
fi

# build binutils
echo "Building Binutils"

if ! [ -e $BINUTILS_DIR ]; then
    tar xf ${BINUTILS_TAR} -C $BUILD_PATH
fi
mkdir -p ${BINUTILS_BUILD}
cd ${BINUTILS_BUILD}
${BINUTILS_DIR}/configure --prefix=${PREFIX} --target=${TARGET} --disable-nls --with-sysroot --disable-werror
make -C ${BINUTILS_BUILD} -j 8
sudo make install -C ${BINUTILS_BUILD} -j8

# build gcc
echo "Building gcc"

if ! [ -e $GCC_DIR ]; then
    tar xf ${GCC_TAR} -C ${BUILD_PATH}
fi
mkdir -p ${GCC_BUILD}
cd ${GCC_BUILD}
${GCC_DIR}/configure --prefix=${PREFIX} --target=${TARGET} --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc -C ${GCC_BUILD} -j 8
make all-target-libgcc -C ${GCC_BUILD} -j 8
sudo make install-gcc -C ${GCC_BUILD} -j 8
sudo make install-target-libgcc -C ${GCC_BUILD} -j 8

cd $CURDIR
