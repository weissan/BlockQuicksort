import sys

#print(sys.argv)


def P(i, j, f):
#sort_pair(sek1, sek2, less);
 #   print("["+str(i)+", " + str(j) +"]")
    f.write("\tsort_pair(sek"+str(i)+", sek"+str(j)+", less);\n")

def Pbracket(i, x, j, y, f):
    a = 0
    b = 0
    if x == 1 and y == 1:
        P(i, j, f)
    elif x == 1 and y == 2:
        P(i, (j+1), f)
        P(i, j, f)
    elif x == 2 and y == 1:
        P(i, j, f)
        P((i+1), j, f)
    else:
        a = int((x/2))
        b = int((y / 2)) if (x & 1) else int( ((y + 1)/2))
        Pbracket(i, a, j, b, f)
        Pbracket((i + a), (x - a), (j + b), (y - b), f)
        Pbracket((i + a), (x - a), j, b, f)

def Pstar(i, m, f):
    a = 0
    if m > 1:
        a = int(m/2)
        Pstar(i, a, f)
        Pstar((i + a), (m - a), f)
        Pbracket(i, a, (i + a), (m - a), f)


def bose(n, f):
    Pstar(0, n, f)


def strategyGenerator(pivots, sampleSize, filename, writingType, strategy, sumStrategy):
    if pivots > sampleSize:
        raise Exception("More pivots than sample size selected!")

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
        f.write("namespace median {\n\n")

        f.write("template<typename iter, typename Compare>\n")
        f.write("inline void sort_pair(iter i1, iter i2, Compare less) {\n")
        f.write("\ttypedef typename std::iterator_traits<iter>::value_type T;\n")
        f.write("\tbool smaller = less(*i2, *i1);\n")
        f.write("\tT temp = std::move(smaller ? *i1 : temp);\n")
        f.write("\t*i1 = std::move(smaller ? *i2 : *i1);\n")
        f.write("\t*i2 = std::move(smaller ? temp : *i2);\n")
        f.write("}\n\n")

        f.write("template<typename iter, typename Compare>\n")
        f.write("inline iter* "+filename[:-2]+"(iter begin, iter end, Compare less){\n")
        f.write("\tint tmp = (end-begin)/"+ str((sampleSize-1)) + ";\n")
        for i in range(0, sampleSize):
            if i == 0:
                f.write("\titer sek"+str(i)+" = begin;\n")
            elif i == 1:
                f.write("\titer sek"+str(i)+" = (begin+tmp);\n")
            elif i == sampleSize-1:
                f.write("\titer sek"+str(i)+" = end-1;\n")
            else:
                f.write("\titer sek"+str(i)+" = begin+(tmp*"+str(i)+");\n")
        bose(sampleSize, f)
        f.write("\titer* ret = new iter[" + str(pivots) +"];\n")
        counter = 0
        if sumStrategy == 0:
            for i in range(0, pivots):
                f.write("\tret[" + str(i) + "] = sek" + str(i)+";\n")
        else:
            for i in range(0, pivots):
                counter += strategy[i] + 1
                f.write("\tret[" + str(i) + "] = sek" + str(counter) + ";\n")
        f.write("\treturn ret;\n")
        f.write("}\n")
        f.write("}")
#		iter* ret = new iter[2];
#		ret[0] = sek0;
#		ret[1] = sek3; 
#		return ret;
#	}




if __name__ == '__main__':
    pivots = int(sys.argv[1])
    sampleSize = int(sys.argv[2])
    filename = sys.argv[3]
    writingType = sys.argv[4]
    tmpStrat = sys.argv[5:]
    strategy = []
    total = 0
    for i in tmpStrat:
        newI = int(i)
        strategy.append(newI)
        total += newI
    
    if sampleSize-1 < total:
        raise Exception("Sample size is too small compared to your strategy")
    strategyGenerator(pivots, sampleSize, filename, writingType, strategy, total)
	#bose(int(sys.argv[1]))