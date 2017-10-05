#!/usr/bin/env python

# The MIT License (MIT)
# 
# Copyright (c) 2015 Brian Pursley
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import sys, argparse

class ComparisonNetwork(list):

	def __str__(self):
		result = ""
		usedInputs = []
		current = []
		for c in self:
			if c[0] in usedInputs or c[1] in usedInputs:
				result += str(current).replace(" ", "") + "\n"
				del usedInputs[:]
				del current[:]
			current.append(c)
			usedInputs.append(c[0])
			usedInputs.append(c[1])
		result += str(current).replace(" ", "")
		return result
		
	def sortBinarySequence(self, sequence):
		result = sequence
		for c in self:
			if (result >> c[0]) & 1 < (result >> c[1]) & 1:
				result = (result - 2**c[1]) | 2**c[0]
		return result
		
	def sortSequence(self, sequence):
		result = list(sequence)
		for c in self:
			if result[c[0]] > result[c[1]]:
				result[c[0]], result[c[1]] = result[c[1]], result[c[0]]
		return result
		
	def getMaxInput(self):
		max = 0
		for c in self:
			if c[0] > max:
				max= c[0]
			if c[1] > max:
				max = c[1]
		return max
		
	def svg(self):
		scale = 1
		xscale = scale * 35
		yscale = scale * 20
		
		innerResult = ''
		x = xscale
		usedInputs = []
		for c in self:
			if c[0] in usedInputs or c[1] in usedInputs:
				x += xscale
				del usedInputs[:]
			for ui in usedInputs:
				if (ui > c[0] and ui < c[1]) or (ui > c[1] and ui < c[0]):
					x += xscale / 3
					break
			y0 = yscale + c[0] * yscale
			y1 = yscale + c[1] * yscale
			innerResult += "<circle cx='%s' cy='%s' r='%s' style='stroke:black;stroke-width:1;fill=yellow' />"%(x, y0, 3)
			innerResult += "<line x1='%s' y1='%s' x2='%s' y2='%s' style='stroke:black;stroke-width:%s' />"%(x, y0, x, y1, 1)
			innerResult += "<circle cx='%s' cy='%s' r='%s' style='stroke:black;stroke-width:1;fill=yellow' />"%(x, y1, 3)
			usedInputs.append(c[0])
			usedInputs.append(c[1])
		
		w = x + xscale
		n = self.getMaxInput() + 1
		h = (n + 1) * yscale
		result = "<?xml version='1.0' encoding='utf-8'?>"
		result += "<!DOCTYPE svg>"
		result += "<svg width='%spx' height='%spx' xmlns='http://www.w3.org/2000/svg'>"%(w, h)
		for i in range(0, n):
			y = yscale + i * yscale
			result += "<line x1='%s' y1='%s' x2='%s' y2='%s' style='stroke:black;stroke-width:%s' />"%(0, y, w, y, 1)
		result += innerResult
		result += "</svg>"
		return result
		
class SortingNetworkChecker:
	def __init__(self, numberOfInputs):
		self.numberOfInputs = numberOfInputs
		self.sortedBinarySequences = []
		self.maxSequenceToCheck = 2**numberOfInputs
		for i in range(0, numberOfInputs + 1):
			bits = "0" * i + "1" * (numberOfInputs - i)
			self.sortedBinarySequences.append(int(bits, 2))
			
	def isSortingNetwork(self, cn):
		for i in range(1, self.maxSequenceToCheck):
			if cn.sortBinarySequence(i) not in self.sortedBinarySequences:
				return False 
		return True
		
def readComparisonNetwork(filename):
	cn = ComparisonNetwork()
	if filename:
		with open(filename, 'r') as f:
			for line in f:
				cn += eval(line)
	else:
		for line in sys.stdin:
			cn += eval(line)
	return cn
		
def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("-i", "--input", metavar="inputfile", help="specify a file containing comparison network definition")
	parser.add_argument("-o", "--output", metavar="outputfile", nargs='?', const='', help="specify a file for saving the comparison network definition")
	parser.add_argument("-c", "--check", action="store_true", help="check whether it is a sorting network")
	parser.add_argument("-s", "--sort", metavar="list", nargs='?', const='', help="sorts the list using the input comparison network")
	parser.add_argument("--svg", metavar="outputfile", nargs='?', const='', help="generate SVG")
	args = parser.parse_args()

	if args.check:
		cn = readComparisonNetwork(args.input)
		checker = SortingNetworkChecker(cn.getMaxInput() + 1)
		print checker.isSortingNetwork(cn)

	if args.svg or args.svg == "":
		cn = readComparisonNetwork(args.input)
		if args.svg == "":
			print cn.svg()
		else:
			with open(args.svg, "w") as f:
				f.write(cn.svg())

	if args.output or args.output == "":
		cn = readComparisonNetwork(args.input)
		if args.output == "":
			print str(cn)
		else:
			with open(args.output, "w") as f:
				f.write(str(cn))

	if args.sort or args.sort == "":
		cn = readComparisonNetwork(args.input)
		if args.sort == "":
			inputSequence = eval(sys.stdin.readline())
		else:
			inputSequence = eval(args.sort)
		print cn.sortSequence(inputSequence)
		
if __name__ == "__main__":
    main()