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
date = start_time.strftime('%m')

configs = {
	'script_path': os.path.abspath(os.getcwd()),
	'date': date,
	'top_dir': os.path.abspath('../experiments/')
}

check_times="5"

def load_pgm_config(config_file):
    with open(config_file, 'r') as f:
        parsed = json.load(f)
    
    return parsed
	
def check_10(pconfig, pgm, n_parallel, trial, total_time, init_time, ith_trial):
    top_dir = "/".join([configs['top_dir'], ith_trial+configs['date']+"__find"+trial, pgm])
	
    log_dir = top_dir + "/" + "__".join([pgm,"find"+trial, "logs"])
    os.chdir(log_dir)
    top10_log = pgm+"__find"+trial+".log"
    cat_cmd = " ".join(["cat", pgm+"__tfind*", "> "+top10_log])
    os.system(cat_cmd)
    cov_dic={}
    with open(top10_log, "r") as tf:
        lines = tf.readlines()
        for l in lines:
            weight = l.split(": ")[0]
            cov = (l.split(": ")[1]).split('\n')[0]
            cov_dic[weight]=float(cov)
        sorted_list = sorted(cov_dic.items(), key=lambda kv: kv[1], reverse = True)

    top_list = []
    for w in sorted_list[:10]:
        top_list.append(w[0])
	
    scr_dir = configs['script_path'] 
    os.chdir(scr_dir)
    topw_list = ", ".join(top_list)
    check_cmd = " ".join(["python", "subscripts/check_w.py", args.pgm_config, n_parallel, check_times, "-l", '"'+ topw_list +'"', trial, total_time, init_time, ith_trial])
    os.system(check_cmd)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    
    parser.add_argument("pgm_config")
    parser.add_argument("n_parallel")
    parser.add_argument("trial")
    parser.add_argument("total_time")
    parser.add_argument("init_time")
    parser.add_argument("ith_trial")
    
    args = parser.parse_args()
    pconfig = load_pgm_config(args.pgm_config)
    pgm = pconfig['pgm_name']
    n_parallel = args.n_parallel
    trial = args.trial
    total_time = args.total_time
    init_time = args.init_time
    ith_trial= args.ith_trial
    
    check_10(pconfig, pgm, n_parallel, trial, total_time, "\""+init_time+"\"", ith_trial)
