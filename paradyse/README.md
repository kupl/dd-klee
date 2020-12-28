This directory contains the implementation of the algorithm to automatically learning search heuristics for dynamic symbolic execution, called ParaDySE (Parametric Dynamic Symbolic Execution). This approach has been implemented on top of [CREST](https://github.com/jburnim/crest) at first. (See [ParaDySE](https://github.com/kupl/ParaDySE) for our previous work.) And we extended it to be used for KLEE. For technical details, please see our paper below.

- Enhancing Dynamic Symbolic Execution by Automatically Learning Search Heuristics

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

