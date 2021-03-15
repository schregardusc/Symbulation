
#a script to run several replicates of several treatments locally

directory = "results-0.6/"
seeds = range(10, 41)
horiz_mut_rate = [0.002,0.02,0.04,0.06,0.08,0.1,0.12,0.14,0.16,0.18,0.2,0.3,0.4,0.5,1.0]


import subprocess

def cmd(command):
    '''This wait causes all executions to run in sieries.                          
    For parralelization, remove .wait() and instead delay the                      
    R script calls unitl all neccesary data is created.'''
    return subprocess.Popen(command, shell=True).wait()

def silent_cmd(command):
    '''This wait causes all executions to run in sieries.                          
    For parralelization, remove .wait() and instead delay the                      
    R script calls unitl all neccesary data is created.'''
    return subprocess.Popen(command, shell=True, stdout=subprocess.PIPE).wait()

print("Copying SymSettings.cfg to "+directory)
silent_cmd("cp SymSettings.cfg "+directory)

for a in seeds:
    for b in horiz_mut_rate:
#command_str = './symbulation -SEED '+str(a)+' -START_MOI '+str(b)+' -FILE_PATH '+directory+' -FILE_NAME SM'+str(b)+'_Seed'+str(a)+'_SLR'+str(c)+' -SYM_LYSIS_RES '+str(c)
        #command_str = './symbulation -SEED '+str(a)+' -HORIZ_MUTATION_RATE '+str(b)+' -FILE_PATH '+directory+' -FILE_NAME SM'+str(b)+'_Seed'+str(a)+'_HMR'+str(b)+' -HORIZ_MUTATION_RATE '+str(b)
        command_str = './symbulation -SEED '+str(a)+' -HORIZ_MUTATION_RATE '+str(b)+' -FILE_NAME _Seed'+str(a)+'_'+str(b) + " -FILE_PATH "+directory
            
        print(command_str)
        silent_cmd(command_str)
