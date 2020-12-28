
setup_build_variables_coreutils() {
	if [[ -z "${COREUTILS_VERSION}" ]]; then
		COREUTILS_VERSION="8.32"
	fi
	COREUTILS_INSTALL_PATH="${BASE}/GNU/coreutils-${COREUTILS_VERSION}"
	COREUTILS_GCOV_BUILD_PATH="${COREUTILS_INSTALL_PATH}/obj-gcov"
	COREUTILS_LLVM_BUILD_PATH="${COREUTILS_INSTALL_PATH}/obj-llvm"
}

download_coreutils() {
	if [[ ! -d "${BASE}/GNU" ]]; then
		mkdir -p "${BASE}/GNU"
	elif [[ -d "${COREUTILS_INSTALL_PATH}" ]]; then
		echo "coreutils-${COREUTILS_VERSION} exists"
		return
	fi
			
	cd "${BASE}/GNU"
	wget "https://mirror.yongbok.net/gnu/coreutils/coreutils-${COREUTILS_VERSION}.tar.gz"
	tar zxvf "coreutils-${COREUTILS_VERSION}.tar.gz"
	rm "coreutils-${COREUTILS_VERSION}.tar.gz"
}

build_coreutils_gcov() {
	if [[ -f "${COREUTILS_GCOV_BUILD_PATH}/.is_installed" ]]; then
		echo "(gcov) coreutils-${COREUTILS_VERSION} already installed"
		return
	fi

	mkdir -p "${COREUTILS_GCOV_BUILD_PATH}"
	cd "${COREUTILS_GCOV_BUILD_PATH}"
	../configure --disable-nls CFLAGS="-g -fprofile-arcs -ftest-coverage"
	make && touch "${COREUTILS_GCOV_BUILD_PATH}/.is_installed"
}

build_coreutils_llvm() {
	if [[ -f "${COREUTILS_LLVM_BUILD_PATH}/.is_installed" ]]; then
		echo "(llvm) coreutils-${COREUTILS_VERSION} already installed"
		return
	fi

	mkdir -p "${COREUTILS_LLVM_BUILD_PATH}"
	cd "${COREUTILS_LLVM_BUILD_PATH}"
	CC=wllvm ../configure --disable-nls CFLAGS="-g -O1 -Xclang -disable-llvm-passes -D__NO_STRING_INLINES -D_FORTIFY_SOURCE=0 -U__OPTIMIZE__"
	make && touch "${COREUTILS_LLVM_BUILD_PATH}/.is_installed"
	find ./src -executable -type f | xargs -I '{}' extract-bc '{}'
}
