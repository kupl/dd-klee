
setup_build_variables_trueprint() {
	if [[ -z "${TRUEPRINT_VERSION}" ]]; then
		TRUEPRINT_VERSION="5.4"
	fi
	TRUEPRINT_INSTALL_PATH="${BASE}/trueprint-${TRUEPRINT_VERSION}"
	TRUEPRINT_GCOV_BUILD_PATH="${TRUEPRINT_INSTALL_PATH}/obj-gcov"
	TRUEPRINT_LLVM_BUILD_PATH="${TRUEPRINT_INSTALL_PATH}/obj-llvm"
}

download_trueprint() {
	if [[ ! -d "${BASE}" ]]; then
		mkdir -p "${BASE}"
	elif [[ -d "${TRUEPRINT_INSTALL_PATH}" ]]; then
		echo "trueprint-${TRUEPRINT_VERSION} exists"
		return
	fi
			
	cd "${BASE}"
	wget "https://mirror.yongbok.net/gnu/trueprint/trueprint-${TRUEPRINT_VERSION}.tar.gz"
	tar zxvf "trueprint-${TRUEPRINT_VERSION}.tar.gz"
	rm "trueprint-${TRUEPRINT_VERSION}.tar.gz"
}

build_trueprint_gcov() {

	for i in $(seq 1 $core); do	
		local TRUEPRINT_GCOV_BUILD_PATH_
		local cnt
		if ! [[ $core -eq "1" ]]; then
			TRUEPRINT_GCOV_BUILD_PATH_=${TRUEPRINT_GCOV_BUILD_PATH}${i}
			cnt=" ${i}"
		else
			TRUEPRINT_GCOV_BUILD_PATH_=${TRUEPRINT_GCOV_BUILD_PATH}
			cnt=""
		fi

		if [[ -f "${TRUEPRINT_GCOV_BUILD_PATH_}/.is_installed" ]]; then
			echo "(obj-gcov${cnt}) trueprcntnt-${TRUEPRINT_VERSION} already cntnstalled"
			continue
		fi
		
		mkdir -p "${TRUEPRINT_GCOV_BUILD_PATH_}"
		cd "${TRUEPRINT_GCOV_BUILD_PATH_}"
		../configure --disable-nls CFLAGS="-g -fprofile-arcs -ftest-coverage"
		make && touch "${TRUEPRINT_GCOV_BUILD_PATH_}/.is_installed"
	done
}

build_trueprint_llvm() {
	if [[ -f "${TRUEPRINT_LLVM_BUILD_PATH}/.is_installed" ]]; then
		echo "(obj-llvm) trueprint-${TRUEPRINT_VERSION} already installed"
		return
	fi

	mkdir -p "${TRUEPRINT_LLVM_BUILD_PATH}"
	cd "${TRUEPRINT_LLVM_BUILD_PATH}"
	CC=wllvm ../configure CFLAGS="-g -O1 -Xclang -disable-llvm-passes -D__NO_STRING_INLINES -D_FORTIFY_SOURCE=0 -U__OPTIMIZE__"
	make && touch "${TRUEPRINT_LLVM_BUILD_PATH}/.is_installed"
	find ./src -executable -type f | xargs -I '{}' extract-bc '{}'
}
