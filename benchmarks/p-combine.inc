
setup_build_variables_combine() {
	if [[ -z "${COMBINE_VERSION}" ]]; then
		COMBINE_VERSION="0.4.0"
	fi
	COMBINE_INSTALL_PATH="${BASE}/combine-${COMBINE_VERSION}"
	COMBINE_GCOV_BUILD_PATH="${COMBINE_INSTALL_PATH}/obj-gcov"
	COMBINE_LLVM_BUILD_PATH="${COMBINE_INSTALL_PATH}/obj-llvm"
}

download_combine() {
	if [[ ! -d "${BASE}" ]]; then
		mkdir -p "${BASE}"
	elif [[ -d "${COMBINE_INSTALL_PATH}" ]]; then
		echo "combine-${COMBINE_VERSION} exists"
		return
	fi
			
	cd "${BASE}"
	wget "https://mirror.yongbok.net/gnu/combine/combine-${COMBINE_VERSION}.tar.gz"
	tar zxvf "combine-${COMBINE_VERSION}.tar.gz"
	rm "combine-${COMBINE_VERSION}.tar.gz"
}

build_combine_gcov() {

	for i in $(seq 1 $core); do	
		local COMBINE_GCOV_BUILD_PATH_
		local cnt
		if ! [[ $core -eq "1" ]]; then
			COMBINE_GCOV_BUILD_PATH_=${COMBINE_GCOV_BUILD_PATH}${i}
			cnt=" ${i}"
		else
			COMBINE_GCOV_BUILD_PATH_=${COMBINE_GCOV_BUILD_PATH}
			cnt=""
		fi

		if [[ -f "${COMBINE_GCOV_BUILD_PATH_}/.is_installed" ]]; then
			echo "(obj-gcov${cnt}) combine-${COMBINE_VERSION} already installed"
			continue
		fi
		
		mkdir -p "${COMBINE_GCOV_BUILD_PATH_}"
		cd "${COMBINE_GCOV_BUILD_PATH_}"
		../configure --disable-nls CFLAGS="-g -fprofile-arcs -ftest-coverage"
		make && touch "${COMBINE_GCOV_BUILD_PATH_}/.is_installed"
	done
}

build_combine_llvm() {
	if [[ -f "${COMBINE_LLVM_BUILD_PATH}/.is_installed" ]]; then
		echo "(obj-llvm) combine-${COMBINE_VERSION} already installed"
		return
	fi

	mkdir -p "${COMBINE_LLVM_BUILD_PATH}"
	cd "${COMBINE_LLVM_BUILD_PATH}"
	CC=wllvm ../configure --disable-nls CFLAGS="-g -O1 -Xclang -disable-llvm-passes -D__NO_STRING_INLINES -D_FORTIFY_SOURCE=0 -U__OPTIMIZE__"
	make && touch "${COMBINE_LLVM_BUILD_PATH}/.is_installed"
	find ./src -executable -type f | xargs -I '{}' extract-bc '{}'
}
