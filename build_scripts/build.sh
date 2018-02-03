#!/usr/bin/env bash
PROJECT_PATH="$( cd "$(dirname "$0")/../" ; pwd -P )"
echo $PROJECT_PATH
# docker pull $DOCKER_IMAGE
for DOCKER_IMAGE in "familiapy_x86_64" "familiapy_i686"
do
rm -rf `dirname $0`/../Familia/third_party
rm -rf `dirname $0`/../build/
docker run --rm -v $PROJECT_PATH:/io $DOCKER_IMAGE $PRE_CMD sh /io/build_scripts/build-wheels.sh
done