KLEE-kupl (a temporary name)
=============================

KLEE-kupl is a data-driven symbolic execution engine, implemented on top of [KLEE](klee.github.io). We are taking data-driven approaches to deal with path-explosion problem of dynamic symbolic execution. The details of the techniques can be found in our papers, each of which consists of its own strategy. This repository is being maintained to provide all of such strategies with KLEE.

- [Installation](#installation)
  - [From Source](#from-source)
  - [Vagrant Box](#vagrant-box)
  - [Docker Image](#docker-image)
- [Getting Started](#getting-started)
- [Resources](#resources)

## Installation

### From Source

Currently, out tool is implemented on top of 2.1 version of KLEE. Thus, we recommend to build with LLVM 6.0.  The steps you should take are exactly same with the ones described in [Building KLEE with LLVM 6.0](https://klee.github.io/releases/docs/v2.1/build-llvm60/), official documentation of vanilla KLEE.

Thus, here we briefly provide the configuration to setup KLEE  and to use approaches described in our papers, which will reproduce the experimental results most closely.

#### Constraint solver

* minisat
* STP (2.3.3)

#### C/C++ library

* klee-uclibc (v1.2): not supported on macOS

  This should be installed to enable the POSIX environment model, which is used in our experiments on GNU benchmarks.

* (optional) libcxx: This sholud be installed to be able to run C++ code.

  You can build with the scripts provided by KLEE, with following command.

```sh
LLVM_VERSION=6 SANITIZER_BUILD= BASE=<LIBCXX_INSTALL_DIR> REQUIRES_RTTI=1 DISABLE_ASSERTIONS=1 ENABLE_DEBUG=0 ENABLE_OPTIMIZED=1 ./klee/scripts/build/build.sh libcxx
```

#### KLEE

After installing dependencies, you can build our extension of KLEE with `cmake`. Below is example configuration.

```sh
git clone https://github.com/kupl/KLEE-kupl.git
cd ~/KLEE-kupl
mkdir build
cd build
cmake \
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
make
```

### Vagrant Box

We provide a Vagrant Box to easily setup environment for our tool. The `Vagrantfile` is supplied to build a box with Ubuntu 18.04 LTS running on VirtualBox machine. For installation and detailed manual of it, read [Vagrant](https://vagrantup.com).

You can customize virtual machine, depending on your system spec. The following part of `Vagrantfile` can be modified for such purpose.

```ruby
Vagrant.configure("2") do |config|
  config.vagrant.plugins = ["vagrant-disksize", "vagrant-vbguest"]
  config.disksize.size = "20GB"
  # ...
  config.vm.provider "virtualbox" do |vb|
    vb.memory = "2048"
    vb.cpus = "2"
    # ...
  end  
  # ...
end
```

A command below from the project directory (where `Vagrantfile` is located) creates a virtual machine and installs some dependencies, which may be better to be installed on system. If you want to configure it, see `bootstrap.sh`.

```sh
vagrant up
```

Next, you should install main `KLEE-kupl`. This proedure is done with `provision`, the subcommand of `vagrant`. Provisioning with `klee_deps` builds some dependencies (e.g. STP) from source. This is done by the script `install_deps.sh`. Provisioning with `klee` builds our extension of KLEE. The script `install_klee.sh` is used and it includes `cmake` usage described in the section [From Source](#From-Source).

```sh
vagrant provision --with-provision klee_deps,klee
```

Now you can `ssh` the Ubuntu 18.04 VirtualBox machine and use our tool. It's easy to halt the machine after exitting ssh session.

```sh
vagrant ssh

# halt the machine after exitting ssh
vagrant halt
```

If you've done `vagrant up` once, it is not necessary to update and install dependent softwares (by `bootstrap.sh`) every time you run the machine. Then, the option  `--no-provision` is useful to power on the machine quickly.

```sh
vagrant up --no-provision
```

### Docker Image

We provide a `Dockerfile` to build docker image. This uses the build script provided by vanilla KLEE, which is explained in [Building KLEE and its dependencies](http://klee.github.io/build-script/). 

```sh
docker build -t kupl/klee .
```

# Getting Started

We provide separate manuals for each approach we've taken on top of KLEE.

Pointers to get you started:

- [ParaDySE(Parametric Dynamic Symbolic Execution)](paradyse)

# Resources

- [KLEE](http://klee.github.io)
- [KLEE Tutorials](http://klee.github.io/tutorials/)

# 
