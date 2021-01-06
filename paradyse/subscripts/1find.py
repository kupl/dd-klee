from multiprocessing import Process, Queue

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

start_time = datetime.datetime.now()
date = start_time.strftime('%m')
configs = {
	'script_path': os.path.abspath(os.getcwd()),
    'top_dir': os.path.abspath('../experiments/'),
    'b_dir': os.path.abspath('../build/'),
	'date': date,
}

def load_pgm_config(config_file):
    with open(config_file, 'r') as f:
        parsed = json.load(f)
    
    return parsed

def Timeout_Checker(total_time, init_time):
    init_time=datetime.datetime.strptime(init_time,'%Y-%m-%d %H:%M:%S.%f')
    current_time = datetime.datetime.now()
    elapsed_time = (current_time-init_time).total_seconds()
    if total_time < elapsed_time:
        os.chdir(configs['script_path'])
        print ("#############################################")
        print ("###############Time Out!!!!!#################")
        print ("#############################################")
        return 100
    else:
        return 0


def gen_run_cmd(pgm, idx, a_budget):
    argv = "--sym-args 0 1 10 --sym-args 0 2 2 --sym-files 1 8 --sym-stdin 8 --sym-stdout"
    log = "logs/" + "__".join([pgm+"find"+args.trial, "ours", str(idx)]) + ".log"
    weight = configs['script_path'] +"/"+ args.trial + "_weights/" + str(idx) + ".weight"
    
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


def running_function(pconfig, pgm, top_dir, log_dir, group_id, idx_list, trial, total_time, init_time, a_budget, ith_trial, queue):
    group_dir = top_dir + "/" + str(group_id)
    os.system(" ".join(["cp -r", pconfig['pgm_dir'], group_dir]))
    tc_location=group_dir+"/"+pconfig['exec_dir']
    os.chdir(tc_location)
    if not os.path.exists("logs"):
        os.mkdir("logs")
    cnt=0
    find_total_log = pgm+"__tfind"+trial+"__"+str(group_id)+".log"
    lf = open(find_total_log, "a")
    rc=0
    for idx in idx_list:
        os.chdir(tc_location)
        (run_cmd, log) = gen_run_cmd(pgm, idx, a_budget)
        rc=Timeout_Checker(total_time, init_time)
        if rc==100:
            break
        with open(os.devnull, 'wb') as devnull:
            os.system(run_cmd)
        
        klee_dir ="klee-out-"+str(cnt)
        os.chdir(klee_dir)
        testcases= glob.glob("*.ktest")
        testcases.sort(key=lambda x:float((x.split('.ktest')[0]).split('test')[1]))       
        
        gcov_location="/".join([configs['script_path'], pconfig['gcov_path']+str(group_id), pconfig['exec_dir']])
        os.chdir(gcov_location)
        rm_cmd = " ".join(["rm", pconfig['gcov_file'], pconfig['gcda_file'], "cov_result"])
        os.system(rm_cmd)

        log_name = pgm+"__find"+trial+"__"+str(idx)+".log"
        
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
            tf.write(str(idx)+": "+str(coverage)+"\n") 
        lf.write(str(idx)+": "+str(coverage)+"\n")
        
        cnt = cnt+1 
        
        mv_cmd = " ".join(["mv", log_name, log_dir]) 
        os.system(mv_cmd)
    
    lf.close()
    os.chdir(tc_location)
    cp_cmd = " ".join(["cp", find_total_log, log_dir]) 
    os.system(cp_cmd)

    os.chdir(configs['script_path'])
    queue.put(rc) 

def run_find(pconfig, pgm, n_iter, n_groups, trial, total_time, init_time, a_budget, ith_trial):
    top_dir = "/".join([configs['top_dir'], ith_trial+"__find"+trial, pgm])
    log_dir = top_dir + "/" + "__".join([pgm,"find"+trial, "logs"])
    if not os.path.exists(log_dir):
        os.makedirs(log_dir)    
    procs = []
    rets = []
    q = Queue()
    #Total parameters:50 / count:2 / mod:10
    count = int(n_iter / n_groups)
    mod = int(n_iter % n_groups)
    tasks=[1] 
    acc_count=0
    for a_core in range(1, n_groups+1):
        if 0<mod:
            acc_count=acc_count+count+1
            tasks.append(acc_count)
            mod=mod-1
        else:
            acc_count=acc_count+count
            tasks.append(acc_count)
    idx_list_list= [[val for val in range(tasks[0],tasks[1]+1)]]
    for i in range(2,len(tasks)):
        min_val=tasks[i-1]+1
        max_val=tasks[i]+1
        a_list=[val for val in range(min_val,max_val)]
        idx_list_list.append(a_list)
    for gid in range(1, n_groups+1):
        idx_list= idx_list_list[gid-1]
        procs.append(Process(target=running_function, args=(pconfig, pgm, top_dir, log_dir, gid, idx_list, trial, total_time, init_time, a_budget, ith_trial, q)))
    for p in procs:
        p.start()
    for p in procs:
        ret = q.get()
        rets.append(ret)
    if 100 in rets:
        sys.exit(100)
	
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("pgm_config")
    parser.add_argument("n_iter")
    parser.add_argument("n_parallel")
    parser.add_argument("trial")
    parser.add_argument("total_time")
    parser.add_argument("init_time")
    parser.add_argument("a_budget")
    parser.add_argument("ith_trial")
    
    args = parser.parse_args()
    pconfig = load_pgm_config(args.pgm_config)
    pgm = pconfig['pgm_name']
    n_iter = int(args.n_iter)
    n_parallel = int(args.n_parallel)
    trial = args.trial
    total_time = int(args.total_time)
    init_time = args.init_time
    a_budget = args.a_budget
    ith_trial= args.ith_trial
    run_find(pconfig, pgm, n_iter, n_parallel, trial, total_time, init_time, a_budget, ith_trial)
