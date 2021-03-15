import os.path
import gzip

folder = ''

treatment_postfixes = ['0.002', '0.02', '0.04', '0.06', '0.08', '0.1', '0.12', '0.14', '0.16', '0.18', '0.2', '0.3', '0.4', '0.5', '1.0']
reps = range(10, 41)
header = "uid treatment rep update coop count hist_0\n"

outputFileName = "munged_basic.dat"

outFile = open(outputFileName, 'w')
outFile.write(header)

for t in treatment_postfixes:
    for r in reps:
        fname = folder +"SymVals_Seed" + str(r) + "_" + t + ".data"
        uid = t + "_" + str(r)
        curFile = open(fname, 'r')
        for line in curFile:
            if (line[0] != "u"):
                splitline = line.split(',')
                outstring1 = "{} {} {} {} {} {} {}\n".format(uid, t, r, splitline[0], splitline[1], splitline[2], \
                splitline[3])
                outFile.write(outstring1)
        curFile.close()
outFile.close()