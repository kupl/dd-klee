
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

	mkdir -p ${BASE}
	BASE=$(cd "$(dirname "${BASE}/.")"; echo "${PWD}")
	SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

	whole_benchmarks=(
		coreutils
		combine
		trueprint
		gcal
	)

	if [[ -z "${BENCHMARKS}" ]]; then
		benchmarks=${whole_benchmarks[@]}
	else
		benchmarks=$(echo $BENCHMARKS | sed "s/,/ /g")
	fi
	
	for benchmark in $benchmarks; do
		cd "${SCRIPT_DIR}"
		echo $benchmark
		source "p-${benchmark}.inc"
		setup_build_variables_${benchmark}
		download_${benchmark}
		build_${benchmark}_gcov ${core}
		build_${benchmark}_llvm
	done
		
}

main "$@"
