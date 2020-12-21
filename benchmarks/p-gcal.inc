
setup_build_variables_gcal() {
	if [[ -z "${GCAL_VERSION}" ]]; then
		GCAL_VERSION="4.1"
	fi
	GCAL_INSTALL_PATH="${BASE}/GNU/gcal-${GCAL_VERSION}"
	GCAL_GCOV_BUILD_PATH="${GCAL_INSTALL_PATH}/obj-gcov"
	GCAL_LLVM_BUILD_PATH="${GCAL_INSTALL_PATH}/obj-llvm"
}

download_gcal() {
	if [[ ! -d "${BASE}/GNU" ]]; then
		mkdir -p "${BASE}/GNU"
	elif [[ -d "${GCAL_INSTALL_PATH}" ]]; then
		echo "gcal-${GCAL_VERSION} exists"
		return
	fi
			
	cd "${BASE}/GNU"
	wget "https://mirror.yongbok.net/gnu/gcal/gcal-${GCAL_VERSION}.tar.gz"
	tar zxvf "gcal-${GCAL_VERSION}.tar.gz"
	rm "gcal-${GCAL_VERSION}.tar.gz"
}

build_gcal_gcov() {
	if [[ -f "${GCAL_GCOV_BUILD_PATH}/.is_installed" ]]; then
		echo "(gcov) gcal-${GCAL_VERSION} already installed"
		return
	fi

	mkdir -p "${GCAL_GCOV_BUILD_PATH}"
	cd "${GCAL_GCOV_BUILD_PATH}"
	../configure --disable-nls CFLAGS="-g -fprofile-arcs -ftest-coverage"
	make && touch "${GCAL_GCOV_BUILD_PATH}/.is_installed"
}

build_gcal_llvm() {
	if [[ -f "${GCAL_LLVM_BUILD_PATH}/.is_installed" ]]; then
		echo "(llvm) gcal-${GCAL_VERSION} already installed"
		return
	fi

	mkdir -p "${GCAL_LLVM_BUILD_PATH}"
	cd "${GCAL_LLVM_BUILD_PATH}"
	CC=wllvm ../configure --disable-nls CFLAGS="-g -O1 -Xclang -disable-llvm-passes -D__NO_STRING_INLINES -D_FORTIFY_SOURCE=0 -U__OPTIMIZE__"
	make && touch "${GCAL_LLVM_BUILD_PATH}/.is_installed"
	find ./src -executable -type f | xargs -I '{}' extract-bc '{}'
}
