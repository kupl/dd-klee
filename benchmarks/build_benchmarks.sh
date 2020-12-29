
main() {
	if [[ -z "${BASE}" ]]; then
		echo "BASE Required but not set"
		exit 1
	fi

	core=""
	if [[ -z "${GCOV_CORE}" ]]; then
		core=1
	elif [[ ! $GCOV_CORE =~ ^[0-9]+$ ]]; then
		echo "GCOV_CORE must be an unsigned integer"
		exit 1
	else
		core=$GCOV_CORE
	fi
	

	CWD=$PWD

	mkdir -p ${BASE}
	 	
	benchmarks=(
		coreutils
		combine
		trueprint
		gcal
	)

	for benchmark in "${benchmarks[@]}"; do
		cd "${CWD}"
		source "p-${benchmark}.inc"
		setup_build_variables_${benchmark}
		download_${benchmark}
		build_${benchmark}_gcov ${core}
		build_${benchmark}_llvm
	done
		
}

main "$@"
