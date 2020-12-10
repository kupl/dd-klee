#!/usr/bin/env sh

DEPS_DIR=/home/vagrant/klee_deps
LLVM_HOME=/usr/lib/llvm-6.0

# stp 2.3.3
git clone https://github.com/stp/stp $DEPS_DIR/stp
cd $DEPS_DIR/stp
git checkout tags/2.3.3
mkdir build
cd build
cmake ..
make

# klee-uclibc v1.2
git clone https://github.com/klee/klee-uclibc $DEPS_DIR/klee-uclibc
cd $DEPS_DIR/klee-uclibc
git checkout tags/klee_uclibc_v1.2
./configure \
	--with-cc=$LLVM_HOME/bin/clang \
	--with-llvm-config=$LLVM_HOME/bin/llvm-config \
	--make-llvm-lib
make

# klee-stats dependencies
pip3 install tabulate

cat /vagrant/bash_profile >> /home/vagrant/.bashrc

