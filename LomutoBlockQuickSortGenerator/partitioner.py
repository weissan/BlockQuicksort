#!/usr/bin/python
import sys

print(sys.argv)

def generateBlockResolving(f, pivots, condition, setNumsZero = True):
	f.write("\t\tfor(index j = 0;" + condition + "; j++) {\n")
	for i in range(1, pivots+2):
		f.write("\t\t\tblock"+str(i)+"[num" + str(i) +"] = *(counter+j);\n")
		#needs to be fixed so we do not increment too many things
		if i != (pivots+1):
			f.write("\t\t\tint o"+str(i)+" = less(counter[j], p"+str(i)+");\n")
		if i == (pivots+1):
			f.write("\t\t\tnum" + str(i) + " += 1 - o"+ str(i-1) + ";\n")
		elif i != 1:
			f.write("\t\t\tnum" + str(i) + " += o"+ str(i) +" - o"+ str((i-1)) + ";\n")
		else:
			f.write("\t\t\tnum" + str(i) + "+= o"+ str(i)+";\n")
	f.write("\t\t}\n")
	
	for i in range (1, pivots+2):
		f.write("\t\tfor(index p = 0; p < num"+ str(i) +"; p++){\n")
		f.write("\t\t\t*counter = block"+str(i)+"[p];\n")
		remain = (pivots+2)-i			
		if remain == 2:
			f.write("\t\t\tstd::iter_swap(offset"+str(pivots)+", counter);\n")	
			f.write("\t\t\toffset"+str(pivots)+"++;\n")
		elif remain > 2:
			rotateLine = "\t\t\trotations::rotate"+str(remain)+"(*counter, "
			for p in range(pivots, i-1, -1):
				rotateLine += "*offset"+str(p)+", "
			rotateLine = rotateLine[:-2] + ");\n"
			f.write(rotateLine)
			for p in range(pivots, i-1, -1):
				f.write("\t\t\toffset"+str(p)+"++;\n")

		f.write("\t\t\tcounter++;\n")

		f.write("\t\t}\n")
		if setNumsZero:
			f.write("\t\tnum"+str(i)+" = 0;\n")

def generatePartition(pivots, blocksize, medianname, filename):
	with open(filename, 'w') as f:
		f.write("/******************************************************************************\n")
		f.write("* "+filename+"\n")
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
		f.write("#include <ctime>\n")
		f.write("#include <cmath>\n")
		f.write("#include <assert.h>\n")
		f.write("#include <functional>\n")
		f.write("#include \"rotations.h\"\n")
		f.write("#ifndef BLOCKSIZE\n")
		f.write("#define BLOCKSIZE 128\n")
		f.write("#endif\n\n")
		f.write("namespace partition {\n\n")
		f.write("template<typename iter, typename Compare>\n")
		retLine = ""
		for i in range(1, pivots+1):
			retLine += "iter* ret"+str(i)+", "
		retLine = retLine[:-2] + "){\n"
		f.write("inline void lomuto_"+str(pivots)+"_partition(iter begin, iter end, iter* pivot_positions, Compare less," + retLine)
		f.write("\ttypedef typename std::iterator_traits<iter>::difference_type index;\n")
		f.write("\ttypedef typename std::iterator_traits<iter>::value_type val;\n")
		for i in range(1, pivots+2):
			f.write("\tval block"+str(i)+"[BLOCKSIZE];\n")
			f.write("\tint num"+str(i) +" = 0;\n")
		f.write("\titer last = end-1;\n")
		for i in range(pivots-1, -1, -1):
			f.write("\tstd::iter_swap(pivot_positions["+str(i)+"], last);\n")
			f.write("\tconst typename std::iterator_traits<iter>::value_type & p"+str(i+1)+" = *last;\n")
			f.write("\tpivot_positions["+str(i)+"] = last;\n")
			f.write("\tlast--;\n")
			f.write("\titer offset"+str(i+1)+" = begin;\n")
		

		f.write("\titer counter = begin;\n")
		f.write("\twhile (last - counter + 1 > BLOCKSIZE) {\n")
		generateBlockResolving(f, pivots, "j < BLOCKSIZE")	
		
		f.write("\t}\n")

		generateBlockResolving(f, pivots, "j < (last-counter+1)", False)
		#resolve pivots need to resolve the lowest first
		for i in range(1, pivots+1):
			pivotsLeft = pivots-i
			if pivotsLeft == 0:
				f.write("\tstd::iter_swap(end-1, offset"+str(i)+");\n")			
			else:
				rotationLine = "\trotations::rotate"+str(pivotsLeft+2) + "("
				rotationLine += "*(end-"+str((pivots+1)-i)+"), "
				for p in range(pivots, i-1, -1):
					rotationLine += "*offset"+str(p)+", "
				rotationLine = rotationLine[:-2] + ");\n"
				f.write(rotationLine)
				for p in range(pivots, i, -1):
					f.write("\toffset"+str(p)+"++;\n")

		for i in range(1, pivots+1):
			f.write("\t*ret"+str(i)+ " = offset"+str(i)+";\n")
		
		f.write("}\n")

		f.write("template< typename iter, typename Compare>\n")
		f.write("struct Lomuto_Block_"+str(pivots)+"_partition {\n")
		retLine = ""
		for i in range(1, pivots+1):
			retLine += "iter* p"+str(i)+", "
		#retLine = retLine[:-2] + "){\n"
		f.write("\tstatic inline void partition(iter begin, iter end, "+ retLine + "Compare less) {\n")
		f.write("\t\titer* pivots = median::"+medianname+"(begin, end-1, less);\n")
		pivotLine = ""
		for i in range(1, pivots+1):
			pivotLine += "p"+str(i)+", "
		pivotLine = pivotLine[:-2] + ");\n"
		f.write("\t\tlomuto_"+str(pivots)+"_partition(begin, end, pivots, less, " + pivotLine)
		f.write("\t}\n")
		f.write("};\n")

		#namespace
		f.write("}")


if __name__ == '__main__':
	generatePartition(int(sys.argv[1]), 128,  "test", sys.argv[2])
    