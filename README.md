# gene_HEll homomorphic crypto
project for class CS05177
## ONLY use for test!
# HOW TO USE
```
git clone https://github.com/lingering/gene_HEll
```
# Dependecies
## 1->SEAL
make sure to install microsoft [SEAL](https://github.com/Microsoft/SEAL)<br>
### Must in Global install
Have root access to the system and thus you can install Microsoft SEAL system-wide as follows:
```
  git clone https://github.com/Microsoft/SEAL
  cd native/src
  cmake .
  make
  sudo make install
  cd ../..
```
##   2->libboost
```
  sudo apt-get install libboost-all-dev
```
##  3->Cmake
### Building CMake from Scratch
 For User in UNIX/Mac OSX/MinGW/MSYS/Cygwin

You need to have a C++ compiler (supporting C++11) and a make installed. Run the bootstrap script you find in the source directory of CMake. You can use the --help option to see the supported options. You may use the --prefix=<install_prefix> option to specify a custom installation directory for CMake. Once this has finished successfully, run make and make install.
```
  download source file of cmake
  git clone https://github.com/Kitware/CMake
```
For example, if you simply want to build and install CMake from source, you can build directly in the source tree:
```
  ./bootstrap && make && sudo make install
```
Or, if you plan to develop CMake or otherwise run the test suite, create a separate build tree:
```
  mkdir cmake-build && cd cmake-build
  ../cmake-source/bootstrap && make
```
# Install
```
cd gene_HEll
cmake .
make
cd ..
```
ALl the executable files are in bin
# Test
```
  cd bin
  ./gene_server
  ./gene_client
```
