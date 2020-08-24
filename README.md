KLEE-kupl (a temporary name)
=============================

## Build KLEE
We recommend to build our KLEE-kupl with LLVM 6.0. See [KLEE][klee].

Build:
```sh
$ git clone https://github.com/kupl/KLEE-kupl.git
$ cd ~/KLEE-kupl
$ mkdir build
$ cd build
$ cmake \
   -DENABLE_SOLVER_STP=ON\
   -DENABLE_POSIX_RUNTIME=ON\
   -DENABLE_KLEE_UCLIBC=ON\
   -DKLEE_UCLIBC_PATH=<KLEE_UCLIBC_SOURCE_DIR>\
   -DENABLE_UNIT_TESTS=OFF\
   -DENABLE_SYSTEM_TESTS=OFF\
   -DLLVM_CONFIG_BINARY=<PATH_TO_llvm-config-6.0>\
   -DLLVMCC=<PATH_TO_clang-6.0>\
   -DLLVMCXX=<PATH_TO_clang++-6.0>\
   -DENABLE_KLEE_LIBCXX=ON\
   -DKLEE_LIBCXX_DIR=<LIBCXX_INSTALL_DIR>/libc++-install-60\
   -DKLEE_LIBCXX_INCLUDE_DIR=<LIBCXX_INSTALL_DIR>/libc++-install-60/include/c++/v1\
   <KLEE-kupl_SRC_DIRECTORY>
$ make
```



[klee]:https://klee.github.io/releases/docs/v2.1/build-llvm60/)
