
#### /vagrant/bash_profile

# llvm
export LLVM_HOME=/usr/lib/llvm-6.0
export PATH=${LLVM_HOME}/bin:${PATH}

# wllvm
export WLLVM_OUTPUT_LEVEL=WARNING
export LLVM_COMPILER=clang

# source-built binaries and libraries
export PATH=/home/vagrant/.local/bin:${PATH}
export LD_LIBRARY_PATH=/home/vagrant/.local/lib:${LD_LIBRARY_PATH}

# klee
export PATH=/home/vagrant/klee/bin:${PATH}

