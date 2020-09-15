FROM klee/llvm:60_O_D_A_ubuntu_bionic-20200112 as llvm_base
FROM klee/uclibc:klee_uclibc_v1.2_60_ubuntu_bionic-20200112 as uclibc_base
FROM klee/tcmalloc:2.7_ubuntu_bionic-20200112 as tcmalloc_base
FROM klee/stp:2.3.3_ubuntu_bionic-20200112 as stp_base
FROM klee/libcxx:60_ubuntu_bionic-20200112 as libcxx_base
FROM llvm_base as intermediate
COPY --from=uclibc_base /tmp /tmp/
COPY --from=tcmalloc_base /tmp /tmp/
COPY --from=stp_base /tmp /tmp/
COPY --from=libcxx_base /tmp /tmp/
ENV LLVM_VERSION=6.0

LABEL maintainer="kupl"


# TODO remove adding sudo package
# Create ``kupl`` user for container with password ``kupl``.
# and give it password-less sudo access (temporarily so we can use the TravisCI scripts)
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt -y --no-install-recommends install sudo build-essential curl libcap-dev cmake libncurses5-dev python3-minimal python3-pip unzip zlib1g-dev libgoogle-perftools-dev libsqlite3-dev doxygen && \
		pip3 install tabulate && \
    rm -rf /var/lib/apt/lists/* && \
    useradd -m kupl && \
    echo kupl:kupl | chpasswd && \
    cp /etc/sudoers /etc/sudoers.bak && \
    echo 'kupl  ALL=(root) NOPASSWD: ALL' >> /etc/sudoers

# Copy across source files needed for build
COPY --chown=kupl:kupl . /tmp/klee-kupl_src/

# Build and set kupl user to be owner
WORKDIR /tmp/klee-kupl_build
RUN cmake \
  	-DCMAKE_BUILD_TYPE=Debug \
  	-DKLEE_RUNTIME_BUILD_TYPE=Debug \
  	-DENABLE_SOLVER_STP=ON \
		-DSTP_DIR=/tmp/stp-2.3.3-install/lib/cmake/STP \
  	-DENABLE_UNIT_TESTS=OFF \
  	-DENABLE_SYSTEM_TESTS=OFF \
		-DENABLE_POSIX_RUNTIME=ON \
		-DENABLE_KLEE_UCLIBC=ON \
		-DKLEE_UCLIBC_PATH=/tmp/klee-uclibc-60 \
		-DLLVM_CONFIG_BINARY=/tmp/llvm-60-install_O_D_A/bin/llvm-config \
  	-DLLVMCC=/tmp/llvm-60-install_O_D_A/bin/clang \
  	-DLLVMCXX=/tmp/llvm-60-install_O_D_A/bin/clang++ \
  	/tmp/klee-kupl_src && \
		make && \
		chown -R kupl:kupl /tmp/klee-kupl_build* && \
		rm -rf /var/lib/apt/lists*	

ENV PATH="$PATH:/tmp/llvm-60-install_O_D_A/bin:/home/kupl/klee-kupl_build/bin"
ENV BASE=/tmp

USER kupl
WORKDIR /home/kupl
ENV LD_LIBRARY_PATH /home/kupl/klee-kupl_build/lib/

# Add KLEE binary directory to PATH
RUN /bin/bash -c 'ln -s ${BASE}/klee-kupl_src /home/kupl/ && ln -s ${BASE}/klee-kupl_build* /home/kupl/klee-kupl_build' 
