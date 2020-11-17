KLEE-kupl (a temporary name)
=============================

## Introduction

`KLEE-kupl` is a data-driven symbolic execution engine, implemented on top of [KLEE](klee.github.io). We are taking a data-driven approaches to deal with path-explosion problem of dynamic symbolic execution. The details of the techniques can be found in our papers, each of which consists of its own strategy. This repository is being maintained to provide all of such strategies with `KLEE`.

## Installation

Currently, out tool is implemented on top of 2.1 version of `KLEE`. Thus, we recommend to build the tool with LLVM 6.0.  The steps you should take are exactly same with the ones described in [Building KLEE with LLVM 6.0](https://klee.github.io/releases/docs/v2.1/build-llvm60/). 

Here, we provide the configuration of our tool, which is closest to the experimental settings in our papers. 

### Constraint solver

* minisat
* STP (2.3.3)

### C/C++ library

* klee-uclibc (v1.2): not supported on macOS

  This should be installed to enable the POSIX environment model, which is used in our experiments on GNU benchmarks.

* libcxx: This sholud be installed to be able to run C++ code.

  You can build with the scripts provided by `KLEE`, with following command.

```sh
LLVM_VERSION=6 SANITIZER_BUILD= BASE=<LIBCXX_INSTALL_DIR> REQUIRES_RTTI=1 DISABLE_ASSERTIONS=1 ENABLE_DEBUG=0 ENABLE_OPTIMIZED=1 ./scripts/build/build.sh libcxx
```

### KLEE-kupl

After inatalling dependencies, you can build our tool with `cmake`. Below is example configuration.

```sh
$ git clone https://github.com/kupl/KLEE-kupl.git
$ cd ~/KLEE-kupl
$ mkdir build
$ cd build
$ cmake \
   -DENABLE_SOLVER_STP=ON \
   -DENABLE_POSIX_RUNTIME=ON \
   -DENABLE_KLEE_UCLIBC=ON \
   -DKLEE_UCLIBC_PATH=<KLEE_UCLIBC_SOURCE_DIR> \ 
   -DENABLE_UNIT_TESTS=OFF \
   -DENABLE_SYSTEM_TESTS=OFF \
   -DLLVM_CONFIG_BINARY=<PATH_TO_llvm-config-6.0> \
   -DLLVMCC=<PATH_TO_clang-6.0> \
   -DLLVMCXX=<PATH_TO_clang++-6.0> \
   -DENABLE_KLEE_LIBCXX=ON \
   -DKLEE_LIBCXX_DIR=<LIBCXX_INSTALL_DIR>/libc++-install-60 \
   -DKLEE_LIBCXX_INCLUDE_DIR=<LIBCXX_INSTALL_DIR>/libc++-install-60/include/c++/v1 \
   <KLEE-kupl_SRC_DIRECTORY>
$ make
```



## Docker

We provide a docker image, which contains all installation steps described above. This will be useful to reproduce the experiment results we reported on the papers.

```sh
docker build -t kupl/klee <KLEE-kupl_SRC_DIRECTORY>
```

