pip uninstall mmrunner -y
swig -c++ -python mmrunner.i 
c++ -c mmrunner_wrap.cxx -I/home/ubuntu/miniconda3/envs/micro-manager/include/python3.7m -fPIC
pip install -e .