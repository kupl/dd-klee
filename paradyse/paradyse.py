from multiprocessing import Process
from subprocess import Popen, PIPE

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
import time
from threading import Timer

# hyper-parameters
a_budget="1000" #  (i.e., N = 1000s in Algorithm 2) 
check_times="5" # (i.g., T = 5 at line 16 in Algorithm 3)

start_time = datetime.datetime.now()
date = start_time.strftime('%m')
ith_trial="1" # ith-trial 

configs = {
    'script_path': os.path.abspath(os.getcwd()),
    'date': date,
    'top_dir': os.path.abspath('../experiments/')
}


def load_pgm_config(config_file):
    with open(config_file, 'r') as f:
        parsed = json.load(f)
    
    return parsed

	
def clean_dir(pconfig, pgm, iters, n_parameter, n_parallel, ith_trial):
    core_num = int(n_parallel)
    count = int(n_parameter)/core_num
    exec_dir=pconfig['exec_dir']   
    
    #find_log 
    for i in range(1,int(iters)+1):
        top_dir = "/".join([configs['top_dir'], ith_trial+"__find"+str(i), pgm])
        dir_name = top_dir+"/"+pgm+"__find"+str(i)+"__logs/"
        os.chdir(top_dir)
        for j in range(1, core_num+1):
            os.chdir(str(j)+exec_dir+"/")
            rm_cmd = " ".join(["rm", "klee-out-*/final.bc", "klee-out-*/messages.txt", "klee-out-*/run.*", "klee-out-*/assembly.ll", "klee-out-*/warnings.txt"])
            os.system(rm_cmd)
            for k in range(0,count):
                klee_dir = "klee-out-"+str(k)
                cp_cmd = " ".join(["cp -r", klee_dir, dir_name+"/"+str(j)+"__"+klee_dir])
                os.system(cp_cmd)
            
            os.chdir(top_dir)
            rm_dir = " ".join(["rm", "-rf", str(j)])                 
            os.system(rm_dir)        
        
        cp_cmd = " ".join(["cp -r", dir_name, configs['top_dir']+"/"+ith_trial+pgm+"__all__logs"])
        os.system(cp_cmd)
    
    #check_log 
    for i in range(1,int(iters)+1):
        top_dir = "/".join([configs['top_dir'], ith_trial+"__check"+str(i), pgm])
        dir_name = top_dir+"/"+pgm+"__check"+str(i)+"__logs/"
        os.chdir(top_dir) 
        os.system("ls >> "+pgm+"_result")
        dir_list = set([])
        with open(pgm+"_result") as f:
            lines = f.readlines()
            for l in lines:
                if l.find(pgm) < 0:
                    dir_list.add(l.split('\n')[0])
        for dir_num in dir_list:
            os.chdir(dir_num+exec_dir+"/")
            rm_cmd = " ".join(["rm", "klee-out-*/final.bc", "klee-out-*/messages.txt", "klee-out-*/run.*", "klee-out-*/assembly.ll", "klee-out-*/warnings.txt"])
            os.system(rm_cmd)
            for k in range(0,10):
                klee_dir = "klee-out-"+str(k)
                cp_cmd = " ".join(["cp -r", klee_dir, dir_name+"/"+dir_num+"__"+klee_dir])
                os.system(cp_cmd)
            
            os.chdir(top_dir)
            rm_dir = " ".join(["rm", "-rf", dir_num])                 
            os.system(rm_dir)        

        cp_cmd = " ".join(["cp -r", dir_name, configs['top_dir']+"/"+ith_trial+pgm+"__all__logs"])
        os.system(cp_cmd)
	
def run_paradyse(pconfig, pgm, n_parameter, n_groups, total_time, ith_trial):
    final_dir = "/".join([configs['top_dir'], ith_trial+pgm+"__all__logs"])
    if not os.path.exists(final_dir):
        os.makedirs(final_dir)
    w_dir = "/".join([final_dir, "w_"+ pgm])
    if not os.path.exists(w_dir):
        os.makedirs(w_dir)
		
    scr_dir = configs['script_path'] 
    os.chdir(scr_dir)
    for i in range(1, 20):
        rm_cmd = " ".join(["rm -rf", str(i)+"_weights"])
        os.system(rm_cmd)
    
    # initially sample n parameters.
    genw_cmd = " ".join(["python", "subscripts/gen_weights.py", n_parameter])
    os.system(genw_cmd)
    cpw_final_cmd = " ".join(["cp -r", "1_weights", w_dir])
    os.system(cpw_final_cmd)
    
    with open(pgm+"_topcheck_log", 'a') as f:
        f.write("0\n")
    last_iter = 1	
    for i in range(1, 20):
        #find - check - refine	
        find_cmd = ["python", "subscripts/1find.py", 
                             args.pgm_config, n_parameter, args.n_parallel, str(i), total_time, str(start_time), a_budget, ith_trial]	
        print find_cmd 
        x = subprocess.Popen(find_cmd)
        x.communicate()
        if int(x.returncode)==100:
            break
        
        check_cmd = ["python", "subscripts/2check.py", 
                             args.pgm_config, args.n_parallel, str(i), total_time, str(start_time), a_budget, check_times, ith_trial]
         
        x = subprocess.Popen(check_cmd)
        x.communicate()
        print (x.returncode)
        if int(x.returncode)==100:
            break

        refine_cmd = " ".join(["python", "subscripts/3refine.py", pgm, n_parameter, str(i), check_times, ith_trial])
        os.system(refine_cmd)
	
        #check saturation
        os.chdir(scr_dir)
        with open(pgm+"_topcheck_log", 'r') as f:
            lines = f.readlines()
            if float(lines[len(lines)-1])<=float(lines[len(lines)-2]):
                print ("Saturation !!\n")
                break		

        #copy_w
        cpw2_final_cmd=	" ".join(["cp -r", str(i+1)+"_weights", w_dir])
        os.system(cpw2_final_cmd)
        last_iter = i
        
    #mv result
    clean_dir(pconfig, pgm, last_iter, n_parameter, n_groups, ith_trial) 
    os.chdir(scr_dir)
    #the best w's average coverage	
    with open("top2w_"+ pgm+"_log", 'r') as f:
        lines = f.readlines()
        topw = []
        n = len(lines)
        topw = lines[len(lines)-2].split()
	
        cp_topw = " ".join(["cp", str(n-1)+"_weights/" + topw[2], pgm+pconfig['exec_dir']+"/best.w"]) 
        mv_topw = " ".join(["cp", str(n-1)+"_weights/" + topw[2], final_dir+"/best.w"]) 
    os.system(cp_topw)
    os.system(mv_topw)
	
    #cp information  
    for i in range(1, 20):
        rm_cmd = " ".join(["rm -rf", str(i)+"_weights"])
        os.system(rm_cmd)
        
    cp_topcheck = " ".join(["mv", pgm+"_topcheck_log", final_dir])
    os.system(cp_topcheck)
	
    cp_topw = " ".join(["mv", "top2w_"+ pgm+"_log", final_dir]) 
    os.system(cp_topw)
   
    finished_time = datetime.datetime.now() 
    print (start_time, finished_time)
    os.chdir(final_dir)
    with open(pgm+"_time_final_result", 'w') as ff:
        ff.write("start_time: "+str(start_time)+"\n")
        ff.write("finished_time: "+str(finished_time)+"\n")
    
    print ("#############################################")
    print ("Successfully Generate a Search Heuristic!!!!!")
    print ("#############################################")
	
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    
    parser.add_argument("pgm_config")
    parser.add_argument("n_parameter")
    parser.add_argument("n_parallel")
    parser.add_argument("total_budget")
    
    args = parser.parse_args()
    pconfig = load_pgm_config(args.pgm_config)
    pgm = pconfig['pgm_name']
    n_parameter = args.n_parameter
    n_parallel = args.n_parallel
    total_budget= args.total_budget
    
    run_paradyse(pconfig, pgm, n_parameter, n_parallel, total_budget, ith_trial)
