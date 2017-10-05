import sys

print(sys.argv)


def generateParts(pivots, f, makeSmaller, startRange, endRange, step, skipStart=-1, skipEnd=-1, extraIndent = ""):
	for p in range(startRange, endRange, step):
		if p == 1:
			f.write(extraIndent + "\t\t\tint part"+ str(p) + " = p"+str(p) + " - begin;\n")
		elif p == pivots+1:
			if makeSmaller:
				f.write(extraIndent + "\t\t\tint part"+ str(p) + " = end - tmp"+str(p-1) +";\n")
			else:
				f.write(extraIndent + "\t\t\tint part"+ str(p) + " = end - p"+str(p-1) +";\n")
		else:
			if p > skipStart and p <= skipEnd:
				continue
			if makeSmaller:
				f.write(extraIndent + "\t\t\tint part"+ str(p) + " = p"+str(p) + " - tmp"+str(p-1)+";\n")
			else:
				f.write(extraIndent + "\t\t\tint part"+ str(p) + " = p"+str(p) + " - p"+str(p-1)+";\n")

def makeSmallerGenerator(pivots, f, startRange, endRange, step, skipStart=-1, skipEnd=-1, extraIndent=""):
	for i in range(startRange, endRange, step):
		if i >= skipStart and i < skipEnd:
			continue
		if i == 0:
			f.write(extraIndent + "\t\t\titer tmpBegin = begin;\n")
			f.write(extraIndent + "\t\t\twhile(*begin == *(tmpBegin+1) && tmpBegin <= p"+str(i+1)+")\n")
			f.write(extraIndent + "\t\t\t\ttmpBegin++;\n")	
			f.write(extraIndent + "\t\t\tbegin = tmpBegin;\n")
		elif i == pivots:
			f.write(extraIndent + "\t\t\titer tmp" + str(i) + " = p"+str(i) + ";\n")
			f.write(extraIndent + "\t\t\twhile(*p"+str(i)+" == *(tmp"+str(i)+"+1) && tmp"+str(i) + " <= end)\n")
			f.write(extraIndent + "\t\t\t\ttmp"+str(i)+"++;\n")
		else:
			f.write(extraIndent + "\t\t\titer tmp" + str(i) + " = p"+str(i) + ";\n")
			f.write(extraIndent + "\t\t\twhile(*p"+str(i)+" == *(tmp"+str(i)+"+1) && tmp"+str(i) + " <= p"+str(i+1)+")\n")
			f.write(extraIndent + "\t\t\t\ttmp"+str(i)+"++;\n")

def generateStack(pivots, f, makeSmaller, skipStart=-1, skipEnd=-1, extraIndent="", lessStackElements=0):
	
	for p in range(1, pivots+2):
		if p > skipStart and p < skipEnd+1:
			continue
		
		stackRange = ((pivots*2)-1) - ((skipEnd - skipStart) *2)
		otherVals = list(range(1, pivots+2))
		otherVals.remove(p)
		for removeMe in range(skipEnd, skipStart, -1):
			otherVals.remove(removeMe)

		toWrite = "\t\t\t" + extraIndent

		if p == pivots+1: 
			toWrite += "else {\n"
		elif p == 1:
			toWrite += "if("
		else:
			toWrite += "else if("
			
		if p != pivots+1:
			for i in otherVals:
				toWrite += "part"+str(i)+" > part"+str(p) +" && "
			toWrite = toWrite[:-3]
			toWrite += ") {\n"
		f.write(toWrite)
		sCounter = 1			


		#Logic is to throw the smallest item first on the stack. 
		#We then have edge cases 
		#If I am in here, the part I am on is the smallest partition
		if p == 1: 
			numberToWriteFirst = 1
			if 1 > skipStart-1 and numberToWriteFirst < skipEnd:
				numberToWriteFirst = skipEnd
			if makeSmaller:
				f.write(extraIndent + "\t\t\t\t*s = tmp"+str(numberToWriteFirst)+" + 1;\n")
			else:
				f.write(extraIndent + "\t\t\t\t*s = p"+str(numberToWriteFirst)+" + 1;\n")

			sCounter = 2
			for i in range(1, stackRange):
				if i % 2 == 0:
					if sCounter > skipStart-1 and sCounter < skipEnd+1:
						if i == stackRange-1:
							sCounter = skipEnd+1
						else:
							sCounter = skipEnd
						#sCounter = skipEnd+1
						if sCounter+1 == p:
							sCounter += 1
					if sCounter > pivots:
						sCounter = pivots
					if makeSmaller:
						f.write(extraIndent + "\t\t\t\t*(s+" + str(i) + ") = tmp"+str(sCounter) +"+1;\n")
					else:
						f.write(extraIndent + "\t\t\t\t*(s+" + str(i) + ") = p"+str(sCounter) +"+1;\n")
					sCounter += 1
				else:
					if sCounter > skipStart and sCounter < skipEnd+1:
						sCounter = skipEnd+1
						if sCounter == p:
							sCounter += 1
					if sCounter > pivots:
						sCounter = pivots
					f.write(extraIndent + "\t\t\t\t*(s+" + str(i) + ") = p"+str(sCounter) +";\n")

			f.write(extraIndent + "\t\t\t\t*(s+"+str(stackRange)+ ") = end;\n")
			f.write(extraIndent + "\t\t\t\tend = p1;\n")
		#last part
		elif p == pivots+1:
			f.write(extraIndent + "\t\t\t\t*s = begin;\n")
			sCounter = 1
			for i in range(1, stackRange):
				if i % 2 == 0:
					#if p == sCounter+1:
					#	sCounter += 1
					if sCounter > skipStart-1 and sCounter < skipEnd:
						sCounter = skipEnd
					if makeSmaller:
						f.write(extraIndent + "\t\t\t\t*(s+" + str(i) + ") = tmp"+str(sCounter) +"+1;\n")
					else:
						f.write(extraIndent + "\t\t\t\t*(s+" + str(i) + ") = p"+str(sCounter) +"+1;\n")
					sCounter += 1
				else:
					#if p == sCounter:
					#	sCounter += 1
					if sCounter > skipStart and sCounter < skipEnd:
						sCounter += skipEnd
					f.write(extraIndent + "\t\t\t\t*(s+" + str(i) + ") = p"+str(sCounter) +";\n")
			
			lastP = pivots
			if pivots > skipStart and pivots < skipEnd+1:
				lastP = skipStart

			f.write(extraIndent + "\t\t\t\t*(s+"+str(stackRange)+") = p"+str(lastP)+";\n")
			if makeSmaller:
				f.write(extraIndent + "\t\t\t\tbegin = tmp"+str(pivots) + "+1;\n")
			else:
				f.write(extraIndent + "\t\t\t\tbegin = p"+str(pivots) + "+1;\n")
		else:
			f.write(extraIndent + "\t\t\t\t*s = begin;\n")
			#Have to skip once in a while
			sCounter = 1
			for i in range(1, stackRange):
				if i % 2 == 0:
					if p == sCounter+1:
						sCounter += 1
					if sCounter > skipStart-1 and sCounter < skipEnd+1:
						if i == stackRange-1:
							sCounter = skipEnd+1
						else:
							sCounter = skipEnd
							if sCounter+1 == p:
								sCounter += 1
					if sCounter > pivots:
						sCounter = pivots
					if makeSmaller:
						f.write(extraIndent + "\t\t\t\t*(s+" + str(i) + ") = tmp"+str(sCounter) +"+1;\n")
					else:
						f.write(extraIndent + "\t\t\t\t*(s+" + str(i) + ") = p"+str(sCounter) +"+1;\n")
					sCounter += 1
				else:
					if p == sCounter:
						sCounter += 1
					if sCounter > skipStart and sCounter < skipEnd+1:
						if i == stackRange-1:
							sCounter = skipEnd+1
						else:
							sCounter = skipEnd
						if p == sCounter:
							sCounter += 1
					if sCounter > pivots:
						sCounter = pivots
					f.write(extraIndent + "\t\t\t\t*(s+" + str(i) + ") = p"+str(sCounter) +";\n")	
			f.write(extraIndent + "\t\t\t\t*(s+"+str(stackRange)+ ") = end;\n")
			if makeSmaller:
				f.write(extraIndent + "\t\t\t\tbegin = tmp" + str(p-1) + "+1;\n" )
			else:
				f.write(extraIndent + "\t\t\t\tbegin = p" + str(p-1) + "+1;\n" )
				
			f.write(extraIndent + "\t\t\t\tend = p" + str(p)+";\n")
		f.write(extraIndent + "\t\t\t}\n")
	f.write(extraIndent + "\t\t\ts += "+str(stackRange+1)+";\n")
	f.write(extraIndent + "\t\t\t*d_s_top = ++depth;\n")
	#Are we sure this should be pivots, when we remove partitions?
	stackAdditions = pivots - (skipEnd - skipStart)
	for i in range(1,  stackAdditions):
		f.write(extraIndent + "\t\t\t*(d_s_top+"+str(i)+") = ++depth;\n")
	f.write(extraIndent + "\t\t\td_s_top += "+str(stackAdditions)+";\n")


def generateQSLogic(threshold, useInsertionSort, checkEqual, makeSmaller, pivots, filename = "quicksort.cpp", enablePAPI = False, papivars = None):
	papiEventsCount = 8
	partionString = "\t\t\tPartitioner<iter, Compare>::partition(begin, end,"
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
		f.write("#include <future>\n")
		f.write("#include <cstdlib>\n\n")
		f.write("#if defined(PAPI)\n")
		f.write("#include <papi.h>\n")
		f.write("#define NUM_EVENTS "+str(papiEventsCount)+ "\n")
		f.write("#endif\n")
		f.write("namespace quicksort {\n")
		f.write("template<template<class , class> class Partitioner, typename iter, typename Compare>\n")
		if checkEqual and not makeSmaller:
			f.write("inline void qsort_"+str(pivots)+"_pivot_equal_elements(iter begin, iter end, Compare less) { \n")
		elif checkEqual and makeSmaller:
			f.write("inline void qsort_"+str(pivots)+"_pivot_equal_elements_make_smaller(iter begin, iter end, Compare less) { \n")
		else:
			f.write("inline void qsort_"+str(pivots)+"_pivot(iter begin, iter end, Compare less) { \n")

		if enablePAPI:
			f.write("#if defined(PAPI)\n")
			f.write("\tint event[NUM_EVENTS] = { \n")
			papiEventIndent = "\t\t\t\t\t"
			f.write(papiEventIndent+"PAPI_L1_TCM,\n")
			f.write(papiEventIndent+"PAPI_L2_TCM,\n")	
			f.write(papiEventIndent+"PAPI_TOT_CYC,\n")	
			f.write(papiEventIndent+"PAPI_TOT_INS, \n")
			f.write(papiEventIndent+"PAPI_BR_CN,\n")	 
			f.write(papiEventIndent+"PAPI_BR_INS,\n")	
			f.write(papiEventIndent+"PAPI_BR_MSP,\n")   	
			f.write(papiEventIndent+"PAPI_BR_NTK,\n")
			f.write("\t};\n")
			f.write("\tlong long values[NUM_EVENTS];\n")
			f.write("\t")
			f.write("\tif (PAPI_start_counters(event, NUM_EVENTS) != PAPI_OK) {\n")
			f.write("\t\tfprintf(stderr, \"PAPI_start_counters - FAILED\");\n")
			f.write("\t\texit(1);\n")
			f.write("\t}\n")
			f.write("#endif\n")

		f.write("\tconst int depth_limit = 2 * ilogb((double)(end - begin)) + 3;\n")
		stackSize = 40 * pivots
		if stackSize < 80:
			stackSize = 80
		f.write("\titer stack["+str(stackSize)+"];\n")
		f.write("\titer* s = stack;\n")
		f.write("\tint depth_stack["+str(stackSize)+"];\n")
		f.write("\tint depth = 0;\n")
		f.write("\tint* d_s_top = depth_stack;\n")
		f.write("\t*s = begin;\n")
		f.write("\t*(s + 1) = end;\n")
		f.write("\ts += 2;\n")
		f.write("\t*d_s_top = 0;\n")
		f.write("\t++d_s_top;\n")
		f.write("\tdo {\n")
		f.write("\t\tif (depth < depth_limit && (end - begin > " + str(threshold) +")) {\n")

		for p in range(1, pivots+1):
			f.write("\t\t\titer p"+str(p)+";\n")
			partionString += "&p"+str(p)+", "
		f.write(partionString +"less);\n")

		##generate equal elements
		if checkEqual:
			#Should start with the largest first
			l = list(range(1, pivots+1))
			
			for i in range(1, pivots):
				l = list(range(pivots, i, -1))
				for j in l:
					statementToWrite = ""
					if i == 1 and j == pivots:
						statementToWrite += "\t\t\tif("
					else:
						statementToWrite += "\t\t\telse if("
					statementToWrite += "*p"+str(i) + " == *p"+str(j)+"){\n"
					f.write(statementToWrite)	
					#I need to skip everything between i and j now
					#Make a method of previous method. Should just have to increase 
					#the starting point of generating??
					if makeSmaller:
						makeSmallerGenerator(pivots, f, 1, pivots+1, 1, i, j, "\t")
					generateParts(pivots, f, makeSmaller, 1, pivots+2, 1, i, j, "\t")
					generateStack(pivots, f, makeSmaller, i, j, "\t", i-j)	

					f.write("\t\t\t}\n")

			#f.write(statementToWrite)
			f.write("\t\t\telse{\n\n")
		
		howMuchIndent = ""
		if checkEqual:
			howMuchIndent = "\t"

		if makeSmaller:
			makeSmallerGenerator(pivots, f, 1, pivots+1, 1, extraIndent=howMuchIndent)	

		#generate parts
		generateParts(pivots, f, makeSmaller, 1, pivots+2, 1, extraIndent=howMuchIndent)
		generateStack(pivots, f, makeSmaller, extraIndent=howMuchIndent)	
		if checkEqual:
			f.write("\t\t\t}\n")
		f.write("\t\t}\n")
		f.write("\t\telse {\n")

		f.write("\t\t\tif(end-begin > "+str(threshold) + ") { \n")
		f.write("\t\t\t\tstd::partial_sort(begin, end, end);\n")
		f.write("\t\t\t}\n")
		if(useInsertionSort):
			f.write("\t\t\telse\n")
			f.write("\t\t\t\tinsertionsort::insertion_sort(begin, end, less);\n")
		f.write("\t\t\ts -= 2;\n")
		f.write("\t\t\tbegin = *s;\n")
		f.write("\t\t\tend = *(s + 1);\n")
		f.write("\t\t\t--d_s_top;\n")
		f.write("\t\t\tdepth = *d_s_top;\n")


		f.write("\t\t}\n")#end else
		f.write("\t} while (s != stack);\n")
		if enablePAPI:
			f.write("#if defined(PAPI)\n")
			f.write("\tif (PAPI_stop_counters(values, NUM_EVENTS) != PAPI_OK) {\n")
			f.write("\t\tfprintf(stderr, \"PAPI_stopped_counters - FAILED\");\n")
			f.write("\t\texit(1);\n")
			f.write("}\n")
			f.write("\tstd::cout << \"Level 1 cache misses: \" << values[0] << std::endl;\n")
			f.write("\tstd::cout << \"Level 2 cache misses: \" << values[1] << std::endl;\n")
			f.write("\tstd::cout << \"Total cycles: \" << values[2] << std::endl;\n")
			f.write("\tstd::cout << \"Instructions completed: \" << values[3] << std::endl;\n")
			f.write("\tstd::cout << \"Conditional branch instructions: \" << values[4] << std::endl;\n")
			f.write("\tstd::cout << \"Branch instructions: \" << values[5] << std::endl;\n")
			f.write("\tstd::cout << \"Conditional branch instructions mispredicted: \" << values[6] << std::endl;\n")
			f.write("\tstd::cout << \"Conditional branch instructions not taken: \" << values[7] << std::endl;\n")
			f.write("#endif\n")


		f.write("}\n")
		f.write("}")
        #Need last pivot logic

			
if __name__ == '__main__':
	#generateQSLogic(threshold, useInsertionSort, pivots, filename = "quicksort.cpp")
	useInsertionSort = False
	if sys.argv[2] == "y" or sys.argv[2] == "yes":
		useInsertionSort = True
	checkEqual = False
	generateQSLogic(int(sys.argv[1]), useInsertionSort, checkEqual, int(sys.argv[3]))