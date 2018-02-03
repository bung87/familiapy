FROM quay.io/pypa/manylinux1_x86_64

COPY build_scripts/manylinux-build.sh .
RUN source manylinux-build.sh
