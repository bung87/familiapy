#!/usr/bin/env bash
# build system prrerequisites
set -e -x

if [[ "$(uname -m)" = i686 ]]; then
	TOOLCHAIN_URL='https://github.com/Noctem/pogeo-toolchain/releases/download/v1.5/gcc-7.3-centos5-i686.tar.bz2'
	export LD_LIBRARY_PATH="/toolchain/lib:${LD_LIBRARY_PATH}"
	MFLAG="-m32"
else
	TOOLCHAIN_URL='https://github.com/Noctem/pogeo-toolchain/releases/download/v1.5/gcc-7.3-centos5-x86-64.tar.bz2'
	export LD_LIBRARY_PATH="/toolchain/lib64:/toolchain/lib:${LD_LIBRARY_PATH}"
	MFLAG="-m64"
fi

curl -L "$TOOLCHAIN_URL" -o toolchain.tar.bz2
tar -C / -xf toolchain.tar.bz2
rm toolchain.tar.bz2
export MANYLINUX=1
export PATH="/toolchain/bin:${PATH}"
# export CFLAGS="-I/toolchain/include ${MFLAG}"
# export CXXFLAGS="-I/toolchain/include ${MFLAG}"