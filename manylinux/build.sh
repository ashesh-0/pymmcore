#!/bin/bash

set -e -x

test -n "$PARALLEL" || PARALLEL=-j2

cd /


git clone https://github.com/swig/swig.git
pushd swig
git checkout rel-4.0.1
curl -fLO https://iweb.dl.sourceforge.net/project/pcre/pcre/8.45/pcre-8.45.tar.gz || \
    curl -fLJO https://sourceforge.net/projects/pcre/files/pcre/8.45/pcre-8.45.tar.gz/download
./Tools/pcre-build.sh
./autogen.sh
./configure
make $PARALLEL
make install
popd


# NUMPY_VERSIONS contains alternating ABI tags and NumPy versions.
# Convert it to an associative array.
numpy_versions=($NUMPY_VERSIONS)
declare -A numpy_version_map
abitags=() # To preserve ordering
for ((i=0; i<${#numpy_versions[@]}; i+=2)); do
    abitag=${numpy_versions[i]}
    numpy_version=${numpy_versions[i+1]}

    abitags+=($abitag)
    numpy_version_map[$abitag]=$numpy_version
done


cd /io
for abitag in ${abitags[@]}; do
    numpy_version=${numpy_version_map[$abitag]}
    pybin=/opt/python/$abitag/bin

    # Avoid altering NumPy compile+link (-fvisibility=hidden will break it)
    export CFLAGS=
    export LDFLAGS=
    $pybin/pip install --upgrade pip
    $pybin/pip install --upgrade setuptools wheel numpy==${numpy_version}

    # Package and extract sources to ensure sdist is correct
    rm -rf tmp dist/pymmcore-*.tar.gz
    $pybin/python setup.py sdist
    mkdir tmp
    tar xzf dist/pymmcore-*.tar.gz -C tmp
    cd tmp/pymmcore-*

    export CFLAGS="-fvisibility=hidden -Wno-deprecated -Wno-unused-variable"
    export LDFLAGS="-Wl,--strip-debug" # Sane file size
    $pybin/python setup.py build_ext $PARALLEL
    $pybin/python setup.py build
    $pybin/python setup.py bdist_wheel
    mkdir -p /io/prelim-wheels
    mv dist/*.whl /io/prelim-wheels
done


# Update ABI tag
cd /io
compgen -G "prelim-wheels/*.whl" # Fail if none built
mkdir -p wheelhouse
for wheel in prelim-wheels/*.whl; do
    auditwheel show $wheel
    auditwheel repair $wheel -w wheelhouse
done


# Smoke test
cd /
for abitag in ${abitags[@]}; do
    pybin=/opt/python/$abitag/bin
    $pybin/pip install pymmcore --no-index -f /io/wheelhouse
    $pybin/python /io/smoketest/smoke.py
done
