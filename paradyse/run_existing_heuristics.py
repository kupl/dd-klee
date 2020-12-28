from multiprocessing import Process

import signal
import subprocess
import os
import sys
import random
import json
import argparse
import datetime
import shutil
import re

start_time = datetime.datetime.now()
date = start_time.strftime('%m%d')

configs = {
	'script_path': os.path.abspath(os.getcwd()),
	'date': date,
    'b_dir': os.path.abspath('../build/'),
	'top_dir': os.path.abspath('../experiments/')
}

def load_pgm_config(config_file):
    with open(config_file, 'r') as f:
        parsed = json.load(f)
    
    return parsed
	
max_time=21600


def Timeout_Checker(total_time):
    current_time = datetime.datetime.now()
    elapsed_time = (current_time-start_time).total_seconds()
    if total_time < elapsed_time:
        os.chdir(configs['script_path'])
        print ("#############################################")
        print ("################Time Out!!!!!################")
        print ("#############################################")
        sys.exit()

def gen_run_cmd(pgm, stgy, idx):
    argv = "--sym-args 0 1 10 --sym-args 0 2 2 --sym-files 1 8 --sym-stdin 8 --sym-stdout"
    if stgy.find('.w') >=0:
        stgy="param --weight="+stgy 
    if stgy=="roundrobin": 
        stgy="random-path --search=nurs:covnew"
    run_cmd = " ".join([configs['b_dir']+"/bin/klee", 
                                "-only-output-states-covering-new", "--simplify-sym-indices", "--output-module=false",
                                "--output-source=false", "--output-stats=false", "--disable-inlining", 
                                "--optimize", "--use-forked-solver", "--use-cex-cache", "--libc=uclibc", 
                                "--posix-runtime", "-env-file="+configs['b_dir']+"/../test.env",
                                "--max-sym-array-size=4096", "--max-memory-inhibit=false", 
                                "--switch-type=internal", "--use-batching-search", "--batch-instructions=10000", 
                                "--watchdog -max-time="+str(max_time), "--search="+stgy,
                                pgm+".bc", argv]) 
    return run_cmd

def running_function(pgm, stgy, top_dir, log_dir, group_id, count, trial, total_time):
    group_dir = top_dir + "/" + str(group_id)
    os.system(" ".join(["cp -r", pconfig['pgm_dir'], group_dir]))
    os.chdir(group_dir+pconfig['exec_dir'])
    if not os.path.exists("logs"):
        os.mkdir("logs")
    cnt=0
    log_name = "__".join([pgm,stgy,str(group_id),"log"])
    for idx in range((group_id - 1)*count+1, (group_id)*count+1):
        run_cmd = gen_run_cmd(pgm, stgy, idx)
        Timeout_Checker(total_time)
        with open(os.devnull, 'wb') as devnull:
            os.system(run_cmd)
        klee_dir = "klee-out-"+str(cnt)

        cp2_cmd = " ".join(["cp", "-r", klee_dir, log_dir+"/"+str(trial)+"__"+str(group_id)+"__"+klee_dir]) 
        print cp2_cmd
        os.system(cp2_cmd)
        
        cp3_cmd = " ".join(["cp", "time_result", log_dir+"/"+str(trial)+"__"+str(group_id)+"__"+klee_dir+"/"+pgm+"__"+stgy+"__time_result"+str(group_id)]) 
        print cp3_cmd
        os.system(cp3_cmd)
        
        cnt = cnt+1 
    cp_cmd = " ".join(["cp", log_name, log_dir]) 
    os.system(cp_cmd)


def run_all(pgm, stgy, n_iter, n_groups, trial, total_time):
    top_dir = "/".join([configs['top_dir'], configs['date']+"__"+stgy+str(trial), pgm])
    log_dir = top_dir + "/" + "__".join([pgm, stgy+str(trial), "logs"])
    if not os.path.exists(log_dir):
        os.makedirs(log_dir)
        
    procs = []
    count = int(n_iter / n_groups)
    mod = int(n_iter % n_groups)
    for gid in range(1, n_groups+1):
        procs.append(Process(target=running_function, args=(pgm, stgy, top_dir, log_dir, gid, count, trial, total_time)))
    for p in procs:
        p.start()
	
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("pgm_config")
    parser.add_argument("stgy")
    parser.add_argument("n_iter")
    parser.add_argument("n_parallel")
    parser.add_argument("trial")
    parser.add_argument("total_time")
    
    args = parser.parse_args()
    pconfig = load_pgm_config(args.pgm_config)
    pgm = pconfig['pgm_name']
    stgy = args.stgy
    n_iter = int(args.n_iter)
    n_parallel = int(args.n_parallel)
    trial = int(args.trial)
    total_time = int(args.total_time)
    
    run_all(pgm, stgy, n_iter, n_parallel, trial, total_time)
	
