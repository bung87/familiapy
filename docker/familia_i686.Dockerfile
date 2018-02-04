FROM gcc_i686

# COPY build_scripts /io/build_scripts
# COPY Familia /io/Familia
# COPY familiapy /io/familiapy
# COPY pybind11 /io/pybind11
COPY depends.mk /io/depends.mk
COPY Makefile /io/Makefile
RUN cd /io && make
