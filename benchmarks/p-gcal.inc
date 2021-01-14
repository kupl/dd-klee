
setup_build_variables_gcal() {
	if [[ -z "${GCAL_VERSION}" ]]; then
		GCAL_VERSION="4.1"
	fi
	GCAL_INSTALL_PATH="${BASE}/gcal-${GCAL_VERSION}"
	GCAL_GCOV_BUILD_PATH="${GCAL_INSTALL_PATH}/obj-gcov"
	GCAL_LLVM_BUILD_PATH="${GCAL_INSTALL_PATH}/obj-llvm"
}

download_gcal() {
	if [[ ! -d "${BASE}" ]]; then
		mkdir -p "${BASE}"
	elif [[ -d "${GCAL_INSTALL_PATH}" ]]; then
		echo "gcal-${GCAL_VERSION} exists"
		return
	fi
			
	cd "${BASE}"
	wget "https://mirror.yongbok.net/gnu/gcal/gcal-${GCAL_VERSION}.tar.gz"
	tar zxvf "gcal-${GCAL_VERSION}.tar.gz"
	rm "gcal-${GCAL_VERSION}.tar.gz"
}

build_gcal_gcov() {

	for i in $(seq 1 $core); do	
		local GCAL_GCOV_BUILD_PATH_
		local cnt
		if ! [[ $core -eq "1" ]]; then
			GCAL_GCOV_BUILD_PATH_=${GCAL_GCOV_BUILD_PATH}${i}
			cnt=" ${i}"
		else
			GCAL_GCOV_BUILD_PATH_=${GCAL_GCOV_BUILD_PATH}
			cnt=""
		fi

		if [[ -f "${GCAL_GCOV_BUILD_PATH_}/.is_installed" ]]; then
			echo "(obj-gcov${cnt}) gcal-${GCAL_VERSION} already installed"
			continue
		fi
		
		mkdir -p "${GCAL_GCOV_BUILD_PATH_}"
		cd "${GCAL_GCOV_BUILD_PATH_}"
		../configure --disable-nls CFLAGS="-g -fprofile-arcs -ftest-coverage"
		make && touch "${GCAL_GCOV_BUILD_PATH_}/.is_installed"
	done
}

build_gcal_llvm() {
	if [[ -f "${GCAL_LLVM_BUILD_PATH}/.is_installed" ]]; then
		echo "(obj-llvm) gcal-${GCAL_VERSION} already installed"
		return
	fi

	mkdir -p "${GCAL_LLVM_BUILD_PATH}"
	cd "${GCAL_LLVM_BUILD_PATH}"
	CC=wllvm ../configure --disable-nls CFLAGS="-g -O1 -Xclang -disable-llvm-passes -D__NO_STRING_INLINES -D_FORTIFY_SOURCE=0 -U__OPTIMIZE__"
	make && touch "${GCAL_LLVM_BUILD_PATH}/.is_installed"
	find ./src -executable -type f | xargs -I '{}' extract-bc '{}'
}
