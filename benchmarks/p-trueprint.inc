
setup_build_variables_trueprint() {
	if [[ -z "${TRUEPRINT_VERSION}" ]]; then
		TRUEPRINT_VERSION="5.4"
	fi
	TRUEPRINT_INSTALL_PATH="${BASE}/GNU/trueprint-${TRUEPRINT_VERSION}"
	TRUEPRINT_GCOV_BUILD_PATH="${TRUEPRINT_INSTALL_PATH}/obj-gcov"
	TRUEPRINT_LLVM_BUILD_PATH="${TRUEPRINT_INSTALL_PATH}/obj-llvm"
}

download_trueprint() {
	if [[ ! -d "${BASE}/GNU" ]]; then
		mkdir -p "${BASE}/GNU"
	elif [[ -d "${TRUEPRINT_INSTALL_PATH}" ]]; then
		echo "trueprint-${TRUEPRINT_VERSION} exists"
		return
	fi
			
	cd "${BASE}/GNU"
	wget "https://mirror.yongbok.net/gnu/trueprint/trueprint-${TRUEPRINT_VERSION}.tar.gz"
	tar zxvf "trueprint-${TRUEPRINT_VERSION}.tar.gz"
	rm "trueprint-${TRUEPRINT_VERSION}.tar.gz"
}

build_trueprint_gcov() {
	if [[ -f "${TRUEPRINT_GCOV_BUILD_PATH}/.is_installed" ]]; then
		echo "(gcov) trueprint-${TRUEPRINT_VERSION} already installed"
		return
	fi

	mkdir -p "${TRUEPRINT_GCOV_BUILD_PATH}"
	cd "${TRUEPRINT_GCOV_BUILD_PATH}"
	../configure CFLAGS="-g -fprofile-arcs -ftest-coverage"
	make && touch "${TRUEPRINT_GCOV_BUILD_PATH}/.is_installed"
}

build_trueprint_llvm() {
	if [[ -f "${TRUEPRINT_LLVM_BUILD_PATH}/.is_installed" ]]; then
		echo "(llvm) trueprint-${TRUEPRINT_VERSION} already installed"
		return
	fi

	mkdir -p "${TRUEPRINT_LLVM_BUILD_PATH}"
	cd "${TRUEPRINT_LLVM_BUILD_PATH}"
	CC=wllvm ../configure CFLAGS="-g -O1 -Xclang -disable-llvm-passes -D__NO_STRING_INLINES -D_FORTIFY_SOURCE=0 -U__OPTIMIZE__"
	make && touch "${TRUEPRINT_LLVM_BUILD_PATH}/.is_installed"
	find ./src -executable -type f | xargs -I '{}' extract-bc '{}'
}
