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

def run_refine(pgm, n_iter, trial, check_times, ith_trial):
    top_dir = "/".join([configs['top_dir'], ith_trial+"__check"+str(trial), pgm])
    log_dir = top_dir + "/" + "__".join([pgm,"check"+str(trial), "logs"])
    os.chdir(log_dir)
    cat_cmd = " ".join(["cat", pgm+"*__check*", "> check_result"+str(trial)])
    os.system(cat_cmd)
	
    w_coverage = {}
    with open("check_result"+str(trial), "r") as tf:
        lines = tf.readlines()
        for l in lines:
            weight = l.split(": ")[0]
            cov = (l.split(": ")[1]).split('\n')[0]
            if weight in w_coverage.keys():
                w_coverage[weight].append(float(cov)) 
            else:
                w_coverage[weight] = [float(cov),]
    avr_list =[];	
    for key in w_coverage.keys():
        if w_coverage[key]==0 or len(w_coverage[key])<(check_times-1):
            return None
        else:				
            tup = (key, (sum(w_coverage[key], 0.0)/len(w_coverage[key])));			
            avr_list.append(tup) 
	
    sort_list = sorted(avr_list, key=lambda tup: tup[1]) #coverage sort
    scr_dir= configs['script_path'] 
    os.chdir(scr_dir)
    with open(pgm+"_topcheck_log", 'a') as covf:
        covf.writelines(["%s \n" % sort_list[(len(sort_list)-1)][1] ])
    
    top1_w = (sort_list[len(sort_list)-1])[0] + ".weight"	
    top2_w = (sort_list[len(sort_list)-2])[0] + ".weight"	
	
    with open("top2w_"+pgm+"_log", 'a') as topwf:
        topwf.write("top 1: "+top1_w + " Top 2: " + top2_w +"\n" )
		
    print ("Top 1: ", top1_w, "Top 2: ", top2_w)  

    refine_cmd = " ".join(["python", "subscripts/refine_w.py", args.trial+"_weights/"+top1_w, args.trial+"_weights/"+top2_w, args.n_iter, args.trial])
    os.system(refine_cmd)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    
    parser.add_argument("pgm")
    parser.add_argument("n_iter")
    parser.add_argument("trial")
    parser.add_argument("check_times")
    parser.add_argument("ith_trial")
    
    args = parser.parse_args()
    pgm = args.pgm
    n_iter = int(args.n_iter)
    trial = int(args.trial)
    check_times = int(args.check_times)
    ith_trial= args.ith_trial
    
    run_refine(pgm, n_iter, trial, check_times, ith_trial)
