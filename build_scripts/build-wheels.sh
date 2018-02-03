#!/usr/bin/env bash
# build wheels
set -e -x
if [[ -z "$TOOLCHAIN_DIR" ]]; then
	TOOLCHAIN_DIR=/toolchain
fi
# source "$(dirname $0)/manylinux-build.sh"
if [[ "$(uname -m)" == i686 ]]; then
	export LD_LIBRARY_PATH="${TOOLCHAIN_DIR}/lib:${LD_LIBRARY_PATH}"
	MFLAG="-m32"
else
	export LD_LIBRARY_PATH="${TOOLCHAIN_DIR}/lib64:${TOOLCHAIN_DIR}/lib:${LD_LIBRARY_PATH}"
	MFLAG="-m64"
fi

echo $LD_LIBRARY_PATH
export MANYLINUX=1
export PATH="${TOOLCHAIN_DIR}/bin:${PATH}"
export CFLAGS="-I${TOOLCHAIN_DIR}/include ${MFLAG}"
export CXXFLAGS="-I${TOOLCHAIN_DIR}/include ${MFLAG}"

export  CC=${TOOLCHAIN_DIR}/bin/gcc
export CXX=${TOOLCHAIN_DIR}/bin/g++
cd /io && make


# Compile wheels
for PYBIN in /opt/python/*/bin; do
    if [[ $PYBIN == *cp27* || $PYBIN == *cp33* || $PYBIN == *cp34* ]]; then
    continue
    fi
    # "${PYBIN}/pip" install -r /io/dev-requirements.txt
    # "${PYBIN}/python" setup.py bdist_wheel -h
    cd /io && "${PYBIN}/python" setup.py bdist_wheel --plat-name linux_$(uname -m) -k -d `dirname $0`/../wheelhouse/
done

# Bundle external shared libraries into the wheels
for whl in wheelhouse/*.whl; do
    auditwheel repair "$whl" -w `dirname $0`/../wheelhouse/
done

# Install packages and test
for PYBIN in /opt/python/*/bin/; do
    if [[ $PYBIN == *cp27* ]]; then
    continue
    fi
    "${PYBIN}/pip" install familiapy --no-index -f `dirname $0`/../wheelhouse
    (cd "$HOME"; "${PYBIN}/nosetests" familiapy)
done