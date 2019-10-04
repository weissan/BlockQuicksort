#
# Copyright (C) 2016 Stefan Edelkamp <edelkamp@tzi.de>
# Copyright (C) 2016 Armin Weiß <armin.weiss@fmi.uni-stuttgart.de>
#
# based on the makefile from http://www.diku.dk/~jyrki/Myris/Kat2014S.html
# Copyright (C) 2014 Amr Elmasry
# Copyright (C) 2014 Jyrki Katajainen
# Copyright (C) 2014 Max Stenmark
#
#
# Note: move and comparison tests are NOT implemented for all algorithms.
#
CXX=g++
CXXFLAGS=-O3 -std=c++11 -Wall -march=native -DNDEBUG 
CXXFLAGS_un=-O3 -std=c++11 -Wall -march=native -funroll-loops -DNDEBUG 
CXXFLAGS_O1=-O1 -std=c++11 -Wall -march=native -DNDEBUG 
CXXFLAGS_DEBUG=-O3 -std=c++11 -Wall -march=native -DNDEBUG -funroll-loops


header-files:= $(wildcard *.h++)
versions:= $(basename $(header-files))
time-tests:= $(addsuffix .time, $(versions)) 
comp-tests:= $(addsuffix .comp, $(versions)) 
move-tests:= $(addsuffix .move, $(versions)) 
time-tests-data:= $(addsuffix .datatime, $(versions)) 
comp-tests-data:= $(addsuffix .datacomp, $(versions)) 
move-tests-data:= $(addsuffix .datamove, $(versions)) 


# N = 512 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608 16777216 33554432 67108864 134217728 268435456  536870912 1073741824

N_allcachegrind =  1048576 16777216

N_blocksize = 1048576 16777216 134217728
N = 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608 16777216 33554432 67108864 134217728 268435456 

smallN = 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608 16777216
N_bc = 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608 16777216 33554432

N_few =  1024 16384 131072 1048576 16777216 134217728
seeds = 123456 234567 345678 456789 567890 678901 789012 890123 901234 1012345 1234567 2345678 3456789 4567890 5678901 6789012 7890123 8901234 9012345 100123456 



algorithms = multi_pivot_2_blocked_mo_5_equal_thousand multi_pivot_2_blocked_equal_thousand multi_pivot_2_blocked_mo_5_thousand multi_pivot_2_blocked_thousand multi_pivot_2_blocked_mo_5_equal multi_pivot_2_blocked_equal multi_pivot_2_blocked_mo_5 multi_pivot_2_blocked blocked_double_pivot_check_mosqrt blocked_mosqrt blocked_mo5_mo5 blocked blocked_simple ssssort stl stl_gcc Yaroslavskiy

blockalgorithms = multi_pivot_2_blocked blocked

allalgorithms = multi_pivot_2_blocked_mo_5 multi_pivot_2_blocked blocked_double_pivot_check_mosqrt blocked_mosqrt blocked_mo5_mo5 blocked_simple blocked ssssort ssssort4 stl stl_gcc Yaroslavskiy qsort3_aumueller lomuto_katajainen blocked_stl_loop blocked_hoare_finish lomuto_katajainen

branchAlgs = multi_pivot_2_blocked_mo_5 multi_pivot_2_blocked blocked_double_pivot_check_mosqrt blocked_mo5_mo5 lomuto_katajainen ssssort stl_gcc Yaroslavskiy blocked blocked_no_is lomuto_katajainen_no_is

pivotTestAlgs = multi_pivot_2_blocked_mo_5 multi_pivot_2_blocked blocked_mosqrt blocked_mo5_mo5 blocked_mo23 blocked_mo3_mo5 blocked_mo3_mo3 blocked_mo5 blocked

blocksizes = 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608 16777216
data = b c d i m o p q r s t u v w z
smalldata = i m w

type = int Vector Record
# double Rational


unit: 	$(unit-tests) 
 
all: 	$(time-tests) $(comp-tests) $(move-tests) 
#$(instruction-tests) $(cache-tests) $(branch-tests)

data: 	$(move-tests-data)  $(comp-tests-data) $(time-tests-data) 
# $(instruction-tests-data) $(cache-tests-data) $(branch-tests-data)

newsmalldata = c s v
existingsmalldata = i m w c s v

perftimetest:
	@for alg in $(algorithms); do \
			echo $$alg ; \
			cp $$alg.h++ algorithm.h++ ; \
			$(CXX) $(CXXFLAGS) -DNAME=$$alg -DTYPE=int driver.cpp ; \
				for d in $(newsmalldata) ; do \
				  echo $$d ; \
				  for n in $(smallN) ; do \
					perf stat ./a.out $$n $$d 123456; \
				  done ; \
				done; \
			rm -f ./a.out ; \
	done


newtimetest:
	@for seed in $(seeds) ; do \
		echo "Now running seed: $$seed"; \
		for alg in $(algorithms); do \
			echo $$alg ; \
			cp $$alg.h++ algorithm.h++ ; \
			$(CXX) $(CXXFLAGS) -DNAME=$$alg -DTYPE=int driver.cpp ; \
				for d in $(newsmalldata) ; do \
				  echo $$d ; \
				  for n in $(smallN) ; do \
					./a.out $$n $$d $$seed >> results.time-newtimetest.csv; \
				  done ; \
				done; \
			rm -f ./a.out ; \
		done ;\
	done

timetest:
	@for seed in $(seeds) ; do \
		echo "Now running seed: $$seed"; \
		for alg in $(algorithms); do \
			echo $$alg ; \
			cp $$alg.h++ algorithm.h++ ; \
			for t in $(type) ; do \
				echo $$t ; \
				$(CXX) $(CXXFLAGS) -DNAME=$$alg -DTYPE=$$t driver.cpp ; \
				for n in $(N) ; do \
					./a.out $$n r $$seed >> results.time.csv; \
				done ; \
				rm -f ./a.out ; \
			done ; \
			$(CXX) $(CXXFLAGS) -DNAME=$$alg -DTYPE=int driver.cpp ; \
				for d in $(smalldata) ; do \
				  echo $$d ; \
				  for n in $(smallN) ; do \
					./a.out $$n $$d $$seed >> results.time.csv; \
				  done ; \
				done; \
			rm -f ./a.out ; \
			$(CXX) $(CXXFLAGS_un) -DNAME=$$alg -DTYPE=int driver.cpp ; \
				  for n in $(smallN) ; do \
					./a.out $$n r $$seed >> results.unroll.time.csv; \
				  done ; \
			rm -f ./a.out ; \
			$(CXX) $(CXXFLAGS_O1) -DNAME=$$alg -DTYPE=int driver.cpp ; \
				  for n in $(smallN) ; do \
					./a.out $$n r $$seed >> results.O1.time.csv; \
				  done ; \
			rm -f ./a.out ; \
		done ;\
	done
	

allAlgtimetest:
	@for seed in $(seeds) ; do \
		for alg in $(allalgorithms); do \
			echo $$alg ; \
			cp $$alg.h++ algorithm.h++ ; \
			$(CXX) $(CXXFLAGS) -DNAME=$$alg -DTYPE=int driver.cpp ; \
			for n in $(N) ; do \
				./a.out $$n r $$seed >> results.allAlg.time.csv; \
				./a.out $$n m $$seed >> results.allAlg.time.csv; \
			done ; \
			rm -f ./a.out ; \
		done ;\
	done
	
	 
pivotmethodtest:
	@for seed in $(seeds) ; do \
		for t in $(type) ; do \
			for alg in $(pivotTestAlgs); do \
				cp $$alg.h++ algorithm.h++ ; \
				$(CXX) $(CXXFLAGS) -DNAME=$$alg -DTYPE=$$t driver.cpp ; \
				for n in $(N) ; do \
					./a.out $$n r $$seed >> results.pivotmethod.time.csv; \
				 done ;\
			done ;\
		done ;\
	 done

newblocksizetest:
	@for alg in $(blockalgorithms); do \
			echo $$alg ; \
			cp $$alg.h++ algorithm.h++ ; \
			for b in $(blocksizes) ; do \
				echo $$b ; \
				$(CXX) $(CXXFLAGS) -DNAME=$$alg -DBLOCKSIZE=$$b -DTYPE=int -DBLOCKSIZETEST=1 driver.cpp  ; \
				for n in $(N_blocksize) ; do \
					echo "doing $$n" ; \
					./a.out $$n r $$seed >> results.blocksizes.time.csv; \
				 done ;\
		  done ; \
		done 

blocksizetest: 
	@cp blocked.h++ algorithm.h++ ; \
	for seed in $(seeds) ; do \
		for t in $(type) ; do \
			echo $$t ; \
			for b in $(blocksizes) ; do \
				echo $$b ; \
				$(CXX) $(CXXFLAGS) -DNAME=blocked -DBLOCKSIZE=$$b -DTYPE=$$t -DBLOCKSIZETEST=1 driver.cpp  ; \
				for n in $(N_blocksize) ; do \
					./a.out $$n r $$seed ; \
				 done ;\
			done ;\
		done ;\
	 done
	 
blocksizetest-data: 
	@cp blocked.h++ algorithm.h++ ; \
	for seed in $(seeds) ; do \
		for t in $(type) ; do \
			echo $$t ; \
			for b in $(blocksizes) ; do \
				echo $$b ; \
				$(CXX) $(CXXFLAGS) -DNAME=blocked -DBLOCKSIZE=$$b -DTYPE=$$t -DBLOCKSIZETEST=1 driver.cpp  ; \
				for n in $(N_blocksize) ; do \
					./a.out $$n r $$seed >> results.blocksizes.time.csv; \
				 done ;
			done ;\
		done ;\
	 done
	
#this will take very long
time-tests-data-all:
	@for seed in $(seeds) ; do \
		for alg in $(algorithms); do \
			echo $$alg ; \
			cp $$alg.h++ algorithm.h++ ; \
			for t in $(type) ; do \
				echo $$t ; \
				$(CXX) $(CXXFLAGS) -DNAME=$$alg -DTYPE=$$t driver.cpp ; \
				for d in $(data) ; do \
				  echo $$d ; \
				  for n in $(N) ; do \
					./a.out $$n $$d $$seed >> results.allmodes.time.csv; \
				  done ; \
				done; \
				rm -f ./a.out ; \
			done ;\
		done ;\
	done


$(time-tests): %.time : %.h++
	@cp $*.h++ algorithm.h++ 
	@for t in $(type) ; do \
		echo $$t ; \
		$(CXX) $(CXXFLAGS) -DNAME=$* -DTYPE=$$t driver.cpp ; \
		for d in $(data) ; do \
		  for n in $(N) ; do \
			./a.out $$n $$d ; \
		  done ; \
		done; \
		rm -f ./a.out ; \
	done

$(move-tests): %.move : %.h++
	@cp $*.h++ algorithm.h++
	$(CXX) $(CXXFLAGS) -DMEASURE_MOVES -DNAME=$* driver.cpp
	@for d in $(data) ; do \
	  echo $$d ; \
	  for n in $(N) ; do \
	    ./a.out $$n $$d ; \
	  done \
	done; \
	rm -f ./a.out 

$(comp-tests): %.comp : %.h++
	@cp $*.h++ algorithm.h++
	$(CXX) $(CXXFLAGS) -DMEASURE_COMPARISONS -DNAME=$* driver.cpp
	@for d in $(data) ; do \
	  echo $$d ; \
	  for n in $(N) ; do \
	    ./a.out $$n $$d ; \
	  done \
	done; \
	rm -f ./a.out 

$(time-tests-data): %.datatime : %.h++
	@cp $*.h++ algorithm.h++ 
	@for seed in $(seeds) ; do \
		for t in $(type) ; do \
			echo $$t ; \
			$(CXX) $(CXXFLAGS) -DNAME=$* -DTYPE=$$t driver.cpp ; \
			for d in $(data) ; do \
			  echo $$d ; \
			  for n in $(N) ; do \
				./a.out $$n $$d $$seed >> results.time.csv; \
			  done ; \
			done; \
			rm -f ./a.out ; \
		done ;\
	done

$(move-tests-data): %.datamove : %.h++
	@cp $*.h++ algorithm.h++
	$(CXX) $(CXXFLAGS) -DMEASURE_MOVES -DNAME=$* driver.cpp
	@for d in $(data) ; do \
	  echo $$d ; \
	  for n in $(N) ; do \
	    ./a.out $$n $$d >> $*.$$d.move.csv; \
	  done \
	done ; \
	rm -f algorithm.h++ ./a.out 

$(comp-tests-data): %.datacomp : %.h++
	@cp $*.h++ algorithm.h++
	$(CXX) $(CXXFLAGS) -DMEASURE_COMPARISONS -DNAME=$* driver.cpp
	@for d in $(data) ; do \
	  echo $$d ; \
	  for n in $(N) ; do \
	    ./a.out $$n $$d >> $*.$$d.comp.csv; \
	  done \
	done ; \
	rm -f algorithm.h++ ./a.out 

build: 
	cp $(ALGNAME).h++ algorithm.h++ ; \
	$(CXX) $(CXXFLAGS_DEBUG) -DNAME=$(ALGNAME) -DTYPE=int driver.cpp ;

buildtype: 
	cp $(ALGNAME).h++ algorithm.h++ ; \
	$(CXX) $(CXXFLAGS_DEBUG) -DNAME=$(ALGNAME) -DTYPE=$(TYPE) driver.cpp ;

clean:
	- rm  -f a.out temp algorithm.h++ *.csv 2>/dev/null

veryclean: clean
	- rm -f *~ */*~ 2>/dev/null

find:
	find . -type f -print -exec grep $(word) {} \; | less # or -name '*.cc'
