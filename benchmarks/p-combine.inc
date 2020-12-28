
setup_build_variables_combine() {
	if [[ -z "${COMBINE_VERSION}" ]]; then
		COMBINE_VERSION="0.4.0"
	fi
	COMBINE_INSTALL_PATH="${BASE}/GNU/combine-${COMBINE_VERSION}"
	COMBINE_GCOV_BUILD_PATH="${COMBINE_INSTALL_PATH}/obj-gcov"
	COMBINE_LLVM_BUILD_PATH="${COMBINE_INSTALL_PATH}/obj-llvm"
}

download_combine() {
	if [[ ! -d "${BASE}/GNU" ]]; then
		mkdir -p "${BASE}/GNU"
	elif [[ -d "${COMBINE_INSTALL_PATH}" ]]; then
		echo "combine-${COMBINE_VERSION} exists"
		return
	fi
			
	cd "${BASE}/GNU"
	wget "https://mirror.yongbok.net/gnu/combine/combine-${COMBINE_VERSION}.tar.gz"
	tar zxvf "combine-${COMBINE_VERSION}.tar.gz"
	rm "combine-${COMBINE_VERSION}.tar.gz"
}

build_combine_gcov() {
	if [[ -f "${COMBINE_GCOV_BUILD_PATH}/.is_installed" ]]; then
		echo "(gcov) combine-${COMBINE_VERSION} already installed"
		return
	fi

	mkdir -p "${COMBINE_GCOV_BUILD_PATH}"
	cd "${COMBINE_GCOV_BUILD_PATH}"
	../configure --disable-nls CFLAGS="-g -fprofile-arcs -ftest-coverage"
	make && touch "${COMBINE_GCOV_BUILD_PATH}/.is_installed"
}

build_combine_llvm() {
	if [[ -f "${COMBINE_LLVM_BUILD_PATH}/.is_installed" ]]; then
		echo "(llvm) combine-${COMBINE_VERSION} already installed"
		return
	fi

	mkdir -p "${COMBINE_LLVM_BUILD_PATH}"
	cd "${COMBINE_LLVM_BUILD_PATH}"
	CC=wllvm ../configure --disable-nls CFLAGS="-g -O1 -Xclang -disable-llvm-passes -D__NO_STRING_INLINES -D_FORTIFY_SOURCE=0 -U__OPTIMIZE__"
	make && touch "${COMBINE_LLVM_BUILD_PATH}/.is_installed"
	find ./src -executable -type f | xargs -I '{}' extract-bc '{}'
}
