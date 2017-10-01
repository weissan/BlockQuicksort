import sys
import medianer
import quicksorter 
import partitioner
print(sys.argv)



def generateOverallLogic(name, quicksortname, partitionname, medianname, pivots, quicksortalgname):
    partitionalgname = "Lomuto_Block_"+str(pivots)+"_partition"
    with open(name+".h++", 'w') as f:
        f.write("/******************************************************************************\n")
        f.write("* "+name+"\n")
        f.write("* \n")
        f.write("* \n")
        f.write("* \n")
        f.write("/******************************************************************************\n")
        f.write("* Copyright (C) 2017 Nikolaj Hass <niah@itu.dk>")
        f.write("* \n")
        f.write("* This program is free software: you can redistribute it and/or modify it\n")
        f.write("* under the terms of the GNU General Public License as published by the Free\n")
        f.write("* Software Foundation, either version 3 of the License, or (at your option)\n")
        f.write("* any later version.\n")
        f.write("* \n")
        f.write("* This program is distributed in the hope that it will be useful, but WITHOUT\n")
        f.write("* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or\n")
        f.write("* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for\n")
        f.write("* more details.\n")
        f.write("* \n")
        f.write("* You should have received a copy of the GNU General Public License along with\n")
        f.write("* this program.  If not, see <http://www.gnu.org/licenses/>.\n")
        f.write("******************************************************************************/\n")

        f.write("#pragma once\n")
        f.write("#include <iostream>\n")
        f.write("#include <fstream>\n")
        f.write("#include <vector>\n")
        f.write("#include <queue>\n")
        f.write("#include <algorithm>\n")
        f.write("#include <string>\n")
        f.write("#include <stdlib.h>\n")
        f.write("#include <random>\n")
        f.write("#include <ctime>\n")
        f.write("#include <cmath>\n")
        f.write("#include <assert.h>\n")

        f.write("#include \"insertionsort.h\"\n")
        f.write("#include \""+medianname +"\"\n")
        f.write("#include \""+partitionname+"\"\n")
        f.write("#include \""+quicksortname+"\"\n")
        f.write("namespace " + name + " { \n")

        f.write("\ttemplate<typename iter, typename Compare>\n")
        f.write("\tvoid sort(iter begin, iter end, Compare less) {\n")
        f.write("\t\tquicksort::"+quicksortalgname+"<partition::"+partitionalgname+">(begin, end, less);\n")
        f.write("\t}\n\n")

        f.write("\ttemplate<typename T>\n") 
        f.write("\tvoid sort(std::vector<T> &v) {\n")
        f.write("\t\ttypename std::vector<T>::iterator begin = v.begin();\n")
        f.write("\t\ttypename std::vector<T>::iterator end = v.end();\n")
        f.write("\t\tquicksort::"+quicksortalgname+"<partition::"+partitionalgname+">(begin, end, std::less<T>());\n")
        f.write("\t}\n")
        f.write("}\n")

if __name__ == '__main__':
    pivots = int(sys.argv[1])
    threshold = int(sys.argv[2])
    blocksize = int(sys.argv[3])
    useInsertionSort = False
    if sys.argv[4] == "y" or sys.argv[4] == "yes":
        useInsertionSort = True
    checkEqual = False
    if sys.argv[5] == "y" or sys.argv[5] == "yes":
        checkEqual = True
    makeSmaller = False
    if sys.argv[6] == "y" or sys.argv[6] == "yes":
        makeSmaller = True
    usePapi = False    
    if sys.argv[7] == "y" or sys.argv[7] == "yes":
        usePapi = True
    
    filename = sys.argv[8]

    #writingType = sys.argv[8]
    sampleSize = int(sys.argv[9])
    tmpStrat = sys.argv[10:]
    strategy = []
    total = 0
    for i in tmpStrat:
        newI = int(i)
        strategy.append(newI)
        total += newI
    if sampleSize < total:
        raise Exception("Sample size is too small compared to your strategy")
    if makeSmaller and not checkEqual:
        makeSmaller = False
    if pivots < 2 and checkEqual:
        raise Exception("You can not check for equal pivots with only 1 pivot!")
    quicksortname = "quicksort_logic_"+str(pivots) + "_"+str(threshold)
    quicksortalgname = "qsort_"+str(pivots)+"_pivot"
    if checkEqual:
        quicksortalgname += "_equal_elements"
        quicksortname += "_equal_elements"
    if makeSmaller:
        quicksortalgname += "_make_smaller"
        quicksortname += "_make_smaller"
    quicksortname += ".h"

    stragetyNaming = ""
    for i in strategy:
        stragetyNaming += "_"+str(i)
    medianname = "median_"+str(pivots) + "_"+str(sampleSize)+stragetyNaming+".h"
    partitionname = "lomuto_partition_"+str(pivots)+"_"+str(sampleSize)+stragetyNaming+".h"

    median_function = "pivot_selection_"+str(pivots)+"_"+str(sampleSize)+"_"+str(total)
    quicksorter.generateQSLogic(threshold, useInsertionSort, checkEqual, makeSmaller, pivots, quicksortname, usePapi)
    medianer.strategyGenerator(pivots, sampleSize, medianname, 'w', strategy, total)
    partitioner.generatePartition(pivots, blocksize, medianname[:-2], partitionname)
    generateOverallLogic(filename, quicksortname, partitionname, medianname, pivots, quicksortalgname)

	#bose(int(sys.argv[1]))