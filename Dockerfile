FROM klee/llvm:60_O_D_A_ubuntu_bionic-20200112 as llvm_base
FROM klee/gtest:1.7.0_ubuntu_bionic-20200112 as gtest_base
FROM klee/uclibc:klee_uclibc_v1.2_60_ubuntu_bionic-20200112 as uclibc_base
FROM klee/tcmalloc:2.7_ubuntu_bionic-20200112 as tcmalloc_base
FROM klee/stp:2.3.3_ubuntu_bionic-20200112 as stp_base
FROM klee/libcxx:60_ubuntu_bionic-20200112 as libcxx_base
FROM llvm_base as intermediate
COPY --from=gtest_base /tmp /tmp/
COPY --from=uclibc_base /tmp /tmp/
COPY --from=tcmalloc_base /tmp /tmp/
COPY --from=stp_base /tmp /tmp/
COPY --from=libcxx_base /tmp /tmp/
ENV BASE=/tmp
ENV COVERAGE=0
ENV ENABLE_DOXYGEN=0
ENV ENABLE_OPTIMIZED=1 
ENV ENABLE_DEBUG=1
ENV DISABLE_ASSERTIONS=0
ENV REQUIRES_RTTI=0
ENV LLVM_VERSION=6.0
ENV GTEST_VERSION=1.7.0
ENV UCLIBC_VERSION=klee_uclibc_v1.2
ENV USE_TCMALLOC=1
ENV TCMALLOC_VERSION=2.7
ENV SOLVERS=STP
ENV STP_VERSION=2.3.3
ENV MINISAT_VERSION=master
ENV USE_LIBCXX=1
ENV SANITIZER_BUILD=
ENV KLEE_RUNTIME_BUILD="Debug"

LABEL maintainer="kupl"

ENV SRC_DIR=dd-klee_src
ENV BUILD_DIR=dd-klee_build

# Create ``kupl`` user with password ``kupl``,
# given password-less sudo access
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt -y --no-install-recommends install \
    sudo emacs-nox vim-nox file python3-dateutil wget git && \
    rm -rf /var/lib/apt/lists/* && \
    useradd -m kupl && \
    echo kupl:kupl | chpasswd && \
    cp /etc/sudoers /etc/sudoers.bak && \
    echo 'kupl  ALL=(root) NOPASSWD: ALL' >> /etc/sudoers


# Copy across source files needed for build
COPY --chown=kupl:kupl . /tmp/${SRC_DIR}

# Build and set kupl to be owner
RUN /tmp/${SRC_DIR}/klee/scripts/build/build.sh --debug --install-system-deps klee && chown -R kupl:kupl /tmp/${BUILD_DIR}* && pip3 install flask wllvm && \
    rm -rf /var/lib/apt/lists/*

ENV BASE=/tmp

# Add KLEE header files to system standard include folder
RUN /bin/bash -c 'ln -s ${BASE}/${SRC_DIR}/klee/include/klee /usr/include/'

USER kupl
WORKDIR /home/kupl

# Add symbolic link to src, build directory at home
RUN /bin/bash -c 'ln -s ${BASE}/${SRC_DIR} /home/kupl/ && ln -s ${BASE}/${BUILD_DIR}* /home/kupl/${BUILD_DIR}' 

# Add KLEE binary directory to PATH
ENV PATH="$PATH:/tmp/llvm-60-install_O_D_A/bin:/home/kupl/${BUILD_DIR}/bin"

# Add KLEE library directory to LD_LIBRARY_PATH
ENV LD_LIBRARY_PATH /home/kupl/${BUILD_DIR}/lib
