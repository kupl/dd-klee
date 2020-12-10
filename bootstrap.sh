#!/usr/bin/env sh

#### /vagrant/bootstrap.sh - install system-wide program

apt-get update

DEBIAN_FRONTEND=noninteractive apt-get install -y \
	build-essential cmake \
	python3 python3-pip \
	gcc-multilib g++-multilib \
	git vim

# llvm
DEBIAN_FRONTEND=noninteractive apt-get install -y \
	curl libcap-dev libncurses5-dev python-minimal \
	python-pip unzip libtcmalloc-minimal4 \
	libgoogle-perftools-dev libsqlite3-dev doxygen

DEBIAN_FRONTEND=noninteractive apt-get install -y \
	clang-6.0 llvm-6.0 llvm-6.0-dev llvm-6.0-tools

# stp dependencies
DEBIAN_FRONTEND=noninteractive apt-get install -y \
	bison flex libboost-all-dev python perl zlib1g-dev minisat

# z3 dependencies
# TODO

# klee experiment dependencies
pip3 install wllvm

