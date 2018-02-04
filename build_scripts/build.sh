#!/usr/bin/env bash
PROJECT_PATH="$( cd "$(dirname "$0")/../" ; pwd -P )"
echo $PROJECT_PATH
# docker pull $DOCKER_IMAGE
for DOCKER_IMAGE in "familia_x86_64" "familia_i686"
do
rm -rf `dirname $0`/../third_party
rm -rf `dirname $0`/../build/
docker run --rm -v $PROJECT_PATH/wheelhouse:/io/wheelhouse -v $PROJECT_PATH/Familia:/io/Familia -v $PROJECT_PATH/pybind11:/io/pybind11 -v $PROJECT_PATH/build_scripts:/io/build_scripts -v $PROJECT_PATH/setup.py:/io/setup.py -v $PROJECT_PATH/familiapy:/io/familiapy $DOCKER_IMAGE sh /io/build_scripts/build-wheels.sh
done
# docker run --rm -v $PROJECT_PATH:/io familia_i686 sh /io/build_scripts/build-wheels.sh