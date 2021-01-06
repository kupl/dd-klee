This directory contains the implementation of the algorithm to automatically learning search heuristics for dynamic symbolic execution, called ParaDySE (Parametric Dynamic Symbolic Execution). This approach has been implemented on top of [CREST](https://github.com/jburnim/crest) at first. (See [ParaDySE](https://github.com/kupl/ParaDySE) for our previous work.) And we extended it to be used for KLEE. For technical details, please see our paper below.

- Enhancing Dynamic Symbolic Execution by Automatically Learning Search Heuristics

## How to Automatically Generate a Search Heuristic?

The script for automatically generating a search heuristic is run on a benchmark built with gcov and llvm. 
For instance, we can generate a search heuristic for **gcal-4.1** as follows:

```sh
$ screen 
# Initially, each benchmark should be built with gcov and llvm, respectively:
$ cd ~/dd-klee/benchmarks/
$ BASE=~/dd-klee/benchmarks/gcal-4.1/ GCOV_CORE=20 ./build_benchmarks.sh 
# Run a paradyse
$ cd ~/dd-klee/paradyse/
$ python paradyse.py pgm_config/gcal.json 300 20 72000
```

Each argument of the last command means:

-   **pgm_config/gcal.json** : a json file to describe the benchmark.
-   **300** : the number of parameters to evaluate in **Find Phase**
-   **20** : the number of cpu cores to use in parallel
-   **72000**: the total testing budget  (second) 

If the script successfully ends, you can see the following command:

```sh
#############################################
Successfully Generate a Search Heuristic!!!!!
#############################################
```

Then, you can find the generated search heuristic (i.e. parameter values) as follows:

```sh
$ cd ~/dd-klee/experiments/1gcal__all__logs
$ vi best.w # (Automatically Generated Search Heuristic)
```

You can run gcal-4.1 with the generated heuristic as follows: 

```sh
$ cd ~/dd-klee/experiments/1gcal__all__logs
$ cp best.w ~/dd-klee/benchmarks/gcal-4.1/obj-llvm/src/best.w 
$ cd ~/dd-klee/paradyse 
$ python run_search_heuristic.py pgm_config/gcal.json best.w 10 10 1000
```

Each argument of the last command means:

-   **pgm_config/gcal.json** : a json file to describe the benchmark.
-   **best.w** : a search heuristic (e.g., dfs, random-path, nurs:covnew, ... )
-   **10**: the number of total trials 
-   **10** : the number of cpu cores to use in parallel
-   **1000**: the total testing budget (second) 

## ParameterizedSearcher

We implemented `ParameterizedSearcher` which inherits the class `Seacher` in KLEE. The `ParameterizedSearcher` uses various features and pretrained weights to decide which state to execute. KLEE takes command line arguments to set search strategy. You can use our strategy as follow.

```sh
klee --search=param --weight=<path_to_weight> [options] <input bytecode> <program arguments>...
```

A pretrained weight should be provided as a text file, 26 real numbers separated by new line(`\n`), since we use 26 features to differentiate each state.

### Note

We highly recommend to use `ParameterizedSearcher` with `BatchingSearcher` with enough batch instructions(e.g. 10000), since it may be very slow to extract features per one instruction for large program.

For example,

```sh
klee --search=param --weight=<path_to_weight> --use-batching-search --batch-instructions=10000 ...
```

## Features

Extracting feature is also implemented with OOP paradigm for extensibility. Features are categorized by three types, instruction features, symbolic memory features, and constraint features. For detail, please explore following header files and corresponding source files.

- klee/lib/Core/Feature.h
- klee/lib/Core/InstructionFeature.h
- klee/lib/Core/SymMemoryFeature.h
- klee/lib/Core/ConstraintFeature.h
- klee/lib/Core/FeatureHandler.h

Currently 26 boolean features are used, and they're calculated by 13 C++ classes who mark top 10% and bottom 10% of states based on statistics measured per each state.

## FeatureStats

We also provide a way to explore statistics related to features. This is currently experimental, and will be evolved for further research.

First, for [klee-stats](http://klee.github.io/docs/tools/#klee-stats), following command shows table with `FETotal` column, meaning the number of feature extractions done.

```
klee-stats --print-fstats <klee_result_dir>
```

Second, for `klee`, the option `--write-feature-map-after` extracts raw statistics of states with csv format. The column is for the statistics and the row is for states. Files are generated per every provided number of feature extractions.

```sh
klee --search=param --weight=<path_to_weight> --write-feature-map-after=100 ...
```

```sh
# results are saved in <klee_result_dir>
fmap000001.csv  fmap000002.csv  fmap000003.csv  fmap000004.csv  fmap000005.csv  fmap000006.csv  ...
```

