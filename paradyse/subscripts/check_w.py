from multiprocessing import Process, Value, Queue

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
import glob
import subprocess
from subprocess import Popen, PIPE
import time
from threading import Timer
import math

start_time = datetime.datetime.now()
date = start_time.strftime('%m')
__run_count = Value('i', 0)

configs = {
	'script_path': os.path.abspath(os.getcwd()),
	'date': date,
    'b_dir': os.path.abspath('../build/'),
	'top_dir': os.path.abspath('../experiments/')
}

def Timeout_Checker(total_time, init_time):
    init_time=datetime.datetime.strptime(init_time,'%Y-%m-%d %H:%M:%S.%f')
    current_time = datetime.datetime.now()
    elapsed_time = (current_time-init_time).total_seconds()
    if total_time < elapsed_time:
        os.chdir(configs['script_path'])
        print ("#############################################")
        print ("################Time Out!!!!!################")
        print ("#############################################")
        return 100
    else:
        return 0


def load_pgm_config(config_file):
    with open(config_file, 'r') as f:
        parsed = json.load(f)
    
    return parsed
	

def gen_run_cmd(pgm, weight_idx, trial, a_budget, idx):
    argv = "--sym-args 0 1 10 --sym-args 0 2 2 --sym-files 1 8 --sym-stdin 8 --sym-stdout"
    log = "logs/" + "__".join([pgm+"check"+trial, str(weight_idx), "ours", str(idx)]) + ".log"
    weight = configs['script_path'] +"/"+trial+ "_weights/" + str(weight_idx) + ".weight"
    
    run_cmd = " ".join([configs['b_dir']+"/bin/klee", 
                                "-only-output-states-covering-new", "--simplify-sym-indices", "--output-module=false",
                                "--output-source=false", "--output-stats=false", "--disable-inlining", 
                                "--optimize", "--use-forked-solver", "--use-cex-cache", "--libc=uclibc", 
                                "--posix-runtime", "-env-file="+configs['b_dir']+"/../test.env",
                                "--max-sym-array-size=4096", "--max-memory-inhibit=false",
                                "--switch-type=internal", "--use-batching-search", "--batch-instructions=10000", 
                                "--watchdog -max-time="+a_budget, "-search=param",
                                "-weight="+weight, pgm+".bc", argv]) 
    
    return (run_cmd, log)


def Cal_Coverage(cov_file):
    coverage=0
    with open(cov_file, 'r') as f:
        lines= f.readlines()
        for line in lines:
            if "Taken at least" in line:
                data=line.split(':')[1]
                percent=float(data.split('% of ')[0])
                total=int((data.split('% of ')[1]).strip())
                cov=int(percent*total/100)
                coverage=coverage+cov

    return coverage

def Kill_Process(process, testcase):
    with open(configs['script_path']+"/killed_history", 'a') as f:
        f.write(testcase+"\n")
    os.killpg(os.getpgid(process.pid), signal.SIGTERM)
    print("timeover!")




def run_all(pconfig, n_parallel, pgm, n_iter, weights, trial, total_time, init_time, a_budget, ith_trial):
    top_dir = "/".join([configs['top_dir'], ith_trial+"__check"+trial, pgm])
    log_dir = top_dir + "/" + "__".join([pgm,"check"+trial, "logs"])
    if not os.path.exists(log_dir):
        os.makedirs(log_dir)    
        
    Tasks=[]
    for w_idx in weights:
        for i in range(1,n_iter+1):
            Tasks.append((w_idx,i))
    
    A_core_task = int(math.floor(len(Tasks)/n_parallel))
    remain_tasks= len(Tasks)%n_parallel
    idx_list=[0]
    acc_idx=0
    for i in range(0,n_parallel):
        if i < remain_tasks:
            acc_idx=acc_idx+A_core_task+1
        else:
            acc_idx=acc_idx+A_core_task
        idx_list.append(acc_idx)
   
    procs = []
    rets = []
    q = Queue()
    for group_id in range(0,n_parallel):
        subTask=Tasks[idx_list[group_id]:idx_list[group_id+1]]
        procs.append(Process(target=running_function, args=(pconfig, pgm, top_dir, log_dir, subTask, group_id+1, trial, total_time, a_budget, init_time, q)))
    for p in procs:
        p.start()
    for p in procs:
        ret = q.get()
        rets.append(ret)
    if 100 in rets:
        sys.exit(100)

def running_function(pconfig, pgm, top_dir, log_dir, subTask, group_id, trial, total_time, a_budget, init_time, queue):
    instance_dir = top_dir + "/" + str(group_id)
    dir_cp_cmd = " ".join(["cp -r", pconfig['pgm_dir'], instance_dir])
    os.system(dir_cp_cmd)
    tc_location=instance_dir+"/"+pconfig['exec_dir']

    os.chdir(tc_location)
    if not os.path.exists("logs"):
        os.mkdir("logs")

    cnt=0
    rc=0
    for task in subTask:
        weight_idx=task[0]
        idx=task[1]
        os.chdir(tc_location)
        (run_cmd, log) = gen_run_cmd(pgm, weight_idx, trial, a_budget, idx)
        rc=Timeout_Checker(total_time, init_time)
        if rc==100:
            break
        os.system(run_cmd)

        klee_dir ="klee-out-"+str(cnt)
        os.chdir(klee_dir)
        testcases= glob.glob("*.ktest")
        testcases.sort(key=lambda x:float((x.split('.ktest')[0]).split('test')[1]))       
 
        gcov_location="/".join([configs['script_path'], pconfig['gcov_path']+str(group_id), pconfig['exec_dir']])
        os.chdir(gcov_location)
        rm_cmd = " ".join(["rm", pconfig['gcov_file'], pconfig['gcda_file'], "cov_result"])
        os.system(rm_cmd)
        
        log_name = pgm+trial+"__check"+str(weight_idx)+"__"+str(idx)+".log"
        
        klee_dir_location="/".join([tc_location, klee_dir])
        for tc in testcases:
            run_cmd=[configs['b_dir']+"/bin/klee-replay", "./"+pgm, klee_dir_location+"/"+tc] 
            proc = subprocess.Popen(run_cmd, preexec_fn=os.setsid, stdout=PIPE, stderr=PIPE) 
            my_timer = Timer(0.1, Kill_Process, [proc, tc_location+"/"+tc])
            try:
                my_timer.start()
                stdout, stderr = proc.communicate()
                lines = stderr.splitlines()
            finally:
                my_timer.cancel()
            
        gcov_file="cov_result"
        gcov_cmd=" ".join(["gcov", "-b", pconfig['gcda_file'], "> "+gcov_file])
        os.system(gcov_cmd)
            
        coverage=Cal_Coverage(gcov_file)

        with open(log_name, "a") as tf:
            tf.write(str(weight_idx)+": "+str(coverage)+"\n") 
 
        cnt = cnt+1 
        
        mv_cmd = " ".join(["mv", log_name, log_dir]) 
        os.system(mv_cmd)
    os.chdir(tc_location)
    os.chdir(configs['script_path'])
    queue.put(rc)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    
    parser.add_argument("pgm_config")
    parser.add_argument("n_parallel")
    parser.add_argument("n_iter")
    parser.add_argument("trial")
    parser.add_argument("-l", type=lambda s:[int(item) for item in s.split(',')])
    parser.add_argument("total_time")
    parser.add_argument("init_time")
    parser.add_argument("a_budget")
    parser.add_argument("ith_trial")
    
    args = parser.parse_args()
    pconfig = load_pgm_config(args.pgm_config)
    n_parallel = int(args.n_parallel)
    pgm = pconfig['pgm_name']
    n_iter = int(args.n_iter)
    trial = args.trial
    weights = args.l
    total_time = int(args.total_time)
    init_time = args.init_time
    a_budget = args.a_budget
    ith_trial= args.ith_trial
    
    run_all(pconfig, n_parallel, pgm, n_iter, weights, trial, total_time, init_time, a_budget, ith_trial)
