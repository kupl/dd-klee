
main() {
	if [[ -z "${BASE}" ]]; then
		echo "BASE Required but not set"
		exit 1
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
		build_${benchmark}_gcov
		build_${benchmark}_llvm
	done
		
}

main "$@"
