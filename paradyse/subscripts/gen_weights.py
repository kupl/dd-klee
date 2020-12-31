import random
import os
import sys

n_feats=26

size = int(sys.argv[1])

def gen_random_weights(n_features):
	w = [str(random.uniform(-10, 10)) for _ in range(n_features)]
	return w

def gen_weight_file(n_weights):
	for idx in range(1, n_weights+1):
		weights = gen_random_weights(n_feats)
		fname = "1_weights/" + str(idx) + ".weight"
		with open(fname, 'w') as f:
			for w in weights:
				f.write(str(w) + "\n")

if not os.path.exists("1_weights"):
    os.mkdir("1_weights")
gen_weight_file(size)

