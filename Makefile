
ifndef DEPS_PATH
DEPS_PATH = $(shell pwd)/Familia/third_party
endif

ifndef PROTOC
PROTOC = ${DEPS_PATH}/bin/protoc
endif

all:deps proto
	

include depends.mk

# third party dependency
deps: ${GLOGS} ${GFLAGS} ${PROTOBUF}
	@echo "dependency installed!"

# build proto
proto = Familia/include/config.pb.h Familia/src/config.cpp
proto: Familia/proto/config.proto
	$(PROTOC) --cpp_out=./Familia/src --proto_path=./Familia/proto $<
	mv Familia/src/config.pb.h ./Familia/include/familia
	mv Familia/src/config.pb.cc ./Familia/src/config.cpp
	sed -i ''  's/"config.pb.h"/"familia\/config.pb.h"/g' Familia/src/config.cpp
install:
	CC=gcc-6 CXX=g++-6 pip install -U .
	

