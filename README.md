# BlockQuicksort
Code of BlockQuicksort and other algorithms tested in the paper
"BlockQuicksort: How Branch Mispredictions don't affect Quicksort"
by Stefan Edelkamp and Armin Weiß (available at http://arxiv.org/abs/1604.06697).

This package consists of the following files:
- makefile
- driver.cpp : generates test cases and runs the sorting algorithms

- quicksort.h : contains different Quicksort main loops
- partition.h : contains different versions of the block partitioner and other partitioners
- median.h : functions for choosing pivot elements
- insertionsort.h : copy of insertion_sort from the GCC 4.7.2 implementation of std::sort

- interfaces for BlockQuicksort with different pivot selection methods / partitioners: blocked.h++, blocked_double_pivot_check.h++, blocked_double_pivot_check_mosqrt.h++, blocked_hoare_finish.h++, blocked_mo3_mo3.h++, blocked_mo3_mo5.h++, blocked_mo5_mo5.h++, blocked_Mo5.h++, blocked_mo23.h++, blocked_mosqrt.h++, blocked_simple.h++, hoare.h++

- lomuto_katajainen.h++ : interface for Tuned Quicksort by Elmasry, Katajainen and Stenmark http://www.diku.dk/~jyrki/Myris/Kat2014S.html
- ssssort.h++ : Super Scalar Sample Sort implemented by Timo Bingmann and Lorenz Hübschle-Schneider https://github.com/lorenzhs/ssssort/blob/b931c024cef3e6d7b7e7fd3ee3e67491d875e021/ssssort.h
- stl_gcc : copy of std::sort from the GCC 4.7.2 implementation
- qsort3_aumueller.h++, rotations.h, inssort.h : three pivot Quicksort implemented by Timo Bingmann and Martin Aumüller http://eiche.theoinf.tu-ilmenau.de/quicksort-experiments/
- Yaroslavskiy.h++ : dual pivot Quicksort by Vladimir Yaroslavkiy http://codeblab.com/wp-content/uploads/2009/09/DualPivotQuicksort.pdf


- For running time experiments with one single algorithm:
	make <algorithm>.time
- For running time tests with different sets of algorithms and test cases (output  written to .csv file):
	make timetest
	make allAlgtimetest
	make pivotmethodtest
	make time-tests-data-all
	make blocksizetest-data

- For comparison and move experiments with one single algorithm:
	make <algorithm>.comp
	make <algorithm>.move
  These tests are not implemented for all algorithms.
