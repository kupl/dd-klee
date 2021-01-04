#!/usr/bin/env sh

# set environment variables
DEPS_DIR=/home/vagrant/klee_deps
LLVM_HOME=/usr/lib/llvm-6.0
. /vagrant/bash_profile

# stp 2.3.3
git clone https://github.com/stp/stp $DEPS_DIR/stp
cd $DEPS_DIR/stp
git checkout tags/2.3.3
mkdir build
cd build
CC=clang CXX=clang++ cmake \
	-DCMAKE_INSTALL_PREFIX=/home/vagrant/.local/ ..
make
make install

# z3
git clone https://github.com/Z3Prover/z3 $DEPS_DIR/z3
cd $DEPS_DIR/z3
mkdir build
cd build
CC=clang CXX=clang++ cmake -G "Unix Makefiles" \
	-DCMAKE_INSTALL_PREFIX=/home/vagrant/.local/ ..
make
make install

# klee-uclibc v1.2
git clone https://github.com/klee/klee-uclibc $DEPS_DIR/klee-uclibc
cd $DEPS_DIR/klee-uclibc
git checkout tags/klee_uclibc_v1.2
./configure --make-llvm-lib
make

# klee-stats dependencies
pip3 install tabulate

