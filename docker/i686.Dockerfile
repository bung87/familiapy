FROM quay.io/pypa/manylinux1_i686

COPY build_scripts/manylinux-build.sh .
RUN source manylinux-build.sh
