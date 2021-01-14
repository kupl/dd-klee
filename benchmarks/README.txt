This directory provides build scripts for the benchmarks we used to
evaluate our techniques.

We are maintaining this directory to easily add/remove the benchmarks
according to our experiment. It's organized as follows:

	p-BENCHMARK.inc				- build description 
	build_benchmarks.sh		- main build scripts

The prefix of `p-BENCHMARK.inc` means platform-specific. We currently
assume benchmarks to be built on Ubuntu, but leave the possibility to
add benchmarks for other platforms. To build all of benchmarks we
present, you can do this:

	BASE=<build_directory> ./build_benchmarks.sh

You can also specify the benchmarks with `BENCHMARKS` variable:

	BASE=<build_directory> BENCHMARKS=gcal,coreutils \
		./build_benchmarks.sh

It'll build two types of binary for each benchmark, one is with
`gcov` support and the other is with `llvm`. You can pass `GCOV_CORE`
variable to build multiple binaries for `gcov` experiment. It is just
for efficiency when there is need for multiple measurements, and it
is set to 1 by default. For example:

	BASE=<build_directory> GCOV_CORE=<value> ./build_benchmarks.sh

This directory is highly inspired by the following resources provided
by KLEE:

	https://klee.github.io/build-script/
	https://klee.github.io/tutorials/testing-coreutils/


Appendix
========

There're additional directories containing data used for each
project:

	weights-paradyse - pretrained weights for ParaDySE




	
