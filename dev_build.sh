pip uninstall pymmcore
rm -rf tmp/
python setup.py sdist
mkdir tmp
tar xvzf dist/pymmcore-*.tar.gz -C tmp
mv tmp/pymmcore-* tmp/pymmcore
cd tmp/pymmcore
python setup.py build_ext -j2
python setup.py build
python setup.py install
