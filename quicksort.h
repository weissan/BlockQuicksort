/******************************************************************************
* quicksort.h
*
* Different Quicksort main loops
*
******************************************************************************
* Copyright (C) 2016 Stefan Edelkamp <edelkamp@tzi.de>
* Copyright (C) 2016 Armin Wei� <armin.weiss@fmi.uni-stuttgart.de>
*
* This program is free software: you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#pragma once
#include <future>
#include <cstdlib>
#ifndef IS_THRESH
#define IS_THRESH 20 //(1 << 4)
#endif // !IS_THRESH


namespace quicksort {

	// main Quicksort loop from std::sort (file stl_algo.h) from GCC Version 4.7.2
	enum { _S_threshold = 16 };
	/// This is a helper function for the sort routine.
	template<template<class, class> class Partitioner, typename _RandomAccessIterator, typename _Size, typename _Compare>
	void
		introsort_loop(_RandomAccessIterator first,
			_RandomAccessIterator last,
			_Size depth_limit, _Compare comp)
	{
		while (last - first > int(_S_threshold))
		{
			if (depth_limit == 0)
			{
				partial_sort_count++;
				std::partial_sort(first, last, last, comp);
				return;
			}
			--depth_limit;
			_RandomAccessIterator cut =
				Partitioner<_RandomAccessIterator, _Compare>::partition(first, last, comp);
			introsort_loop<Partitioner>(cut, last, depth_limit, comp);
			last = cut;
		}
	}


	//main Quicksort loop supporting Partitioner with check for duplicate elements
	//Implementation based on Tuned Quicksort (Elmasry, Katajainen, Stenmark)
	//available at http://www.diku.dk/~jyrki/Myris/Kat2014S.html
	template<template<class, class> class Partitioner, typename iter, typename Compare>
	inline void qsort_double_pivot_check(iter begin, iter end, Compare less) {
		const int depth_limit = 2 * ilogb((double)(end - begin)) + 3;
		iter stack[80];
		iter* s = stack;
		int depth_stack[40];
		int depth = 0;
		int* d_s_top = depth_stack;
		*s = begin;
		*(s + 1) = end;
		s += 2;
		*d_s_top = 0;
		++d_s_top;
		do {
			if (depth < depth_limit && end - begin > IS_THRESH) {
				iter pivot;
				int pivot_length = 1;
				pivot = Partitioner< iter, Compare>::partition(begin, end, less, pivot_length);
				if (pivot - begin > end - pivot) {
					*s = begin;
					*(s + 1) = pivot;
					begin = pivot + pivot_length;
				}
				else {
					*s = pivot + pivot_length;
					*(s + 1) = end;
					end = pivot;
				}
				s += 2;
				depth++;
				*d_s_top = depth;
				++d_s_top;
			}
			else {
				if (end - begin > IS_THRESH) {
#ifdef PARTIAL_SORT_COUNT
					partial_sort_count++;
#endif
					std::partial_sort(begin, end, end);
				}
				else
					insertionsort::insertion_sort(begin, end, less); // copy of std::__insertion_sort (GCC 4.7.2)

				s -= 2;
				begin = *s;
				end = *(s + 1);
				--d_s_top;
				depth = *d_s_top;
			}
		} while (s != stack);
	}



	//main Quicksort loop NOT supporting Partitioner with check for duplicate elements
	//Implementation based on Tuned Quicksort (Elmasry, Katajainen, Stenmark)
	//available at http://www.diku.dk/~jyrki/Myris/Kat2014S.html
	template<template<class , class> class Partitioner, typename iter, typename Compare>
	inline void qsort(iter begin, iter end, Compare less) {
		const int depth_limit = 2 * ilogb((double)(end - begin)) + 3;
		iter stack[80];
		iter* s = stack;
		int depth_stack[40];
		int depth = 0;
		int* d_s_top = depth_stack;
		*s = begin;
		*(s + 1) = end;
		s += 2;
		*d_s_top = 0;
		++d_s_top;
		do {
			if (depth < depth_limit && end - begin > IS_THRESH) {
				iter pivot;
				pivot = Partitioner< iter, Compare>::partition(begin, end, less);
				if (pivot - begin > end - pivot) {
					*s = begin;
					*(s + 1) = pivot;
					begin = pivot + 1;
				}
				else {
					*s = pivot + 1;
					*(s + 1) = end;
					end = pivot;
				}
				s += 2;
				depth++;
				*d_s_top = depth;
				++d_s_top;
			}
			else {
				if (end - begin > IS_THRESH) { // if recursion depth limit exceeded
#ifdef PARTIAL_SORT_COUNT
					partial_sort_count++;
#endif
					std::partial_sort(begin, end, end);
				}
#ifndef NOINSERTIONSORT
				else
					insertionsort::insertion_sort(begin, end, less); // copy of std::__insertion_sort (GCC 4.7.2)
#endif 					
				//pop new subarray from stack
				s -= 2;
				begin = *s;
				end = *(s + 1);
				--d_s_top;
				depth = *d_s_top;
			}
		} while (s != stack);
	}

	//main Multi-Pivot Quicksort loop NOT supporting Partitioner with check for duplicate elements
	//Implementation based on Tuned Quicksort (Elmasry, Katajainen, Stenmark)
	//available at http://www.diku.dk/~jyrki/Myris/Kat2014S.html
	template<template<class , class> class Partitioner, typename iter, typename Compare>
	inline void qsort_dual_pivot(iter begin, iter end, Compare less) {
		const int depth_limit = 2 * ilogb((double)(end - begin)) + 3;
		iter stack[80];
		iter* s = stack;
		int depth_stack[80];
		int depth = 0;
		int* d_s_top = depth_stack;
		*s = begin;
		*(s + 1) = end;
		s += 2;
		*d_s_top = 0;
		++d_s_top;
		do {
			assert((end >= begin));
			if (depth < depth_limit && (end - begin > IS_THRESH)) {
				iter p1;
				iter p2;
				
				Partitioner<iter, Compare>::partition(begin, end, &p1, &p2, less);
				
				assert((p1 < p2));
				assert(p2 < end);
				int part1 = p1 - begin;
				int part2 = p2 - p1;
				int part3 = end - p2;
				if (part2 > part1 && part3 > part1) { //Partition 1 is the smallest
					*s = p1 + 1;
					*(s + 1) = p2;
					*(s + 2) = p2 + 1;
					*(s + 3) = end;
					end = p1;
				}
				else if(part1 > part2 && part3 > part2) { //Partition 2 is the smallest
					*s = begin;
					*(s + 1) = p1;
					*(s + 2) = p2 + 1;
					*(s + 3) = end;
					begin = p1 + 1;
					end = p2;
				}
				else {
					*s = begin;
					*(s + 1) = p1;
					*(s + 2) = p1 + 1;
					*(s + 3) = p2;
					begin = p2 + 1;
				}
				assert(begin <= end);
				s += 4;
				*d_s_top = ++depth;
				*(d_s_top+1) = ++depth;
				d_s_top += 2;

			}
			else {
				
				if (end - begin > IS_THRESH) { // if recursion depth limit exceeded
#ifdef PARTIAL_SORT_COUNT
					partial_sort_count++;
#endif
					std::partial_sort(begin, end, end);
				}
#ifndef NOINSERTIONSORT
				else {
					insertionsort::insertion_sort(begin, end, less); // copy of std::__insertion_sort (GCC 4.7.2)
				}
#endif 			
				//pop new subarray from stack
				s -= 2;
				begin = *s;
				end = *(s + 1);
				--d_s_top;
				assert(d_s_top != (depth_stack-1));
				depth = *d_s_top;
				assert(begin <= end);
			}
		} while (s != stack);
	}


	template<template<class , class> class Partitioner, typename iter, typename Compare>
	inline void qsort_dual_pivot_equal_elements(iter begin, iter end, Compare less) {
		const int depth_limit = 2 * ilogb((double)(end - begin)) + 3;
		iter stack[80];
		iter* s = stack;
		int depth_stack[80];
		int depth = 0;
		int* d_s_top = depth_stack;
		*s = begin;
		*(s + 1) = end;
		s += 2;
		*d_s_top = 0;
		++d_s_top;
		do {
			assert((end >= begin));
			if (depth < depth_limit && (end - begin > IS_THRESH)) {
				iter p1;
				iter p2;
				
				Partitioner<iter, Compare>::partition(begin, end, &p1, &p2, less);
				
				if(*p1 == *p2){
					//Disregard the middle partition
					//search, we remove all the elements equal to the pivot
					int part1 = p1 - begin;
					//int part2 = p2 - p1;
					int part3 = end - p2;
					if (part1 > part3) {
						*s = begin;
						*(s + 1) = p1;
						begin = p2 + 1;
					}
					else {
						*s = p2 + 1; 
						*(s + 1) = end;
						end = p1;
					}

					s += 2;
					depth++;
					*d_s_top = depth;
					++d_s_top;

				}
				else{
					//Attempt at making things smaller
					//Search, we remove all the elements equal to the pivot
					iter tmp2 = p2;
					while(*p2 == *(tmp2-1) && less(*p1, *(tmp2-1))) 
						tmp2--;

					//Search in the middle
					iter tmp1 = p1;
					while(*p1 == *(tmp1+1) && less(*(tmp1+1), *end))
						tmp1++;
					
					int part1 = p1 - begin;
					int part2 = tmp2 - tmp1;
					int part3 = end - p2;

					if (part2 > part1 && part3 > part1) { //Partition 1 is the smallest
						*s = tmp1 + 1;
						*(s + 1) = tmp2;
						*(s + 2) = p2 + 1;
						*(s + 3) = end;
						end = p1;
					}
					else if(part1 > part2 && part3 > part2) { //Partition 2 is the smallest
						*s = begin;
						*(s + 1) = p1;
						*(s + 2) = p2 + 1;
						*(s + 3) = end;
						begin = tmp1 + 1;
						end = tmp2;
					}
					else {
						*s = begin;
						*(s + 1) = p1;
						*(s + 2) = tmp1 + 1;
						*(s + 3) = tmp2;
						begin = p2 + 1;
					}
					assert(begin <= end);
					s += 4;
					*d_s_top = ++depth;
					*(d_s_top+1) = ++depth;
					d_s_top += 2;
				}
				

			}
			else {
				
				if (end - begin > IS_THRESH) { // if recursion depth limit exceeded
#ifdef PARTIAL_SORT_COUNT
					partial_sort_count++;
#endif
					std::partial_sort(begin, end, end);
				}
#ifndef NOINSERTIONSORT
				else {
					insertionsort::insertion_sort(begin, end, less); // copy of std::__insertion_sort (GCC 4.7.2)
				}
#endif 			
				//pop new subarray from stack
				s -= 2;
				begin = *s;
				end = *(s + 1);
				--d_s_top;
				assert(d_s_top != (depth_stack-1));
				depth = *d_s_top;
				assert(begin <= end);
			}
		} while (s != stack);
	}


	//main Multi-Pivot Quicksort loop NOT supporting Partitioner with check for duplicate elements
	//Implementation based on Tuned Quicksort (Elmasry, Katajainen, Stenmark)
	//available at http://www.diku.dk/~jyrki/Myris/Kat2014S.html
	template<template<class , class> class Partitioner, typename iter, typename Compare>
	inline void qsort_dual_pivot_thousand(iter begin, iter end, Compare less) {
		const int depth_limit = 2 * ilogb((double)(end - begin)) + 3;
		iter stack[80];
		iter* s = stack;
		int depth_stack[80];
		int depth = 0;
		int* d_s_top = depth_stack;
		*s = begin;
		*(s + 1) = end;
		s += 2;
		*d_s_top = 0;
		++d_s_top;
		do {
			assert((end >= begin));
			if (depth < depth_limit && (end - begin > 1000)) {
				iter p1;
				iter p2;
				
				Partitioner<iter, Compare>::partition(begin, end, &p1, &p2, less);
				
				assert((p1 < p2));
				assert(p2 < end);
				int part1 = p1 - begin;
				int part2 = p2 - p1;
				int part3 = end - p2;
				if (part2 > part1 && part3 > part1) { //Partition 1 is the smallest
					*s = p1 + 1;
					*(s + 1) = p2;
					*(s + 2) = p2 + 1;
					*(s + 3) = end;
					end = p1;
				}
				else if(part1 > part2 && part3 > part2) { //Partition 2 is the smallest
					*s = begin;
					*(s + 1) = p1;
					*(s + 2) = p2 + 1;
					*(s + 3) = end;
					begin = p1 + 1;
					end = p2;
				}
				else {
					*s = begin;
					*(s + 1) = p1;
					*(s + 2) = p1 + 1;
					*(s + 3) = p2;
					begin = p2 + 1;
				}
				assert(begin <= end);
				s += 4;
				*d_s_top = ++depth;
				*(d_s_top+1) = ++depth;
				d_s_top += 2;

			}
			else {
				
				if (end - begin > 1000) { // if recursion depth limit exceeded
#ifdef PARTIAL_SORT_COUNT
					partial_sort_count++;
#endif
					std::partial_sort(begin, end, end);
				}
#ifndef NOINSERTIONSORT
				else {
					quicksort::qsort<partition::Hoare_block_partition_mosqrt>(begin, end, less);
				}
#endif 			
				//pop new subarray from stack
				s -= 2;
				begin = *s;
				end = *(s + 1);
				--d_s_top;
				assert(d_s_top != (depth_stack-1));
				depth = *d_s_top;
				assert(begin <= end);
			}
		} while (s != stack);
	}


	template<template<class , class> class Partitioner, typename iter, typename Compare>
	inline void qsort_dual_pivot_equal_elements_thousand(iter begin, iter end, Compare less) {
		const int depth_limit = 2 * ilogb((double)(end - begin)) + 3;
		iter stack[80];
		iter* s = stack;
		int depth_stack[80];
		int depth = 0;
		int* d_s_top = depth_stack;
		*s = begin;
		*(s + 1) = end;
		s += 2;
		*d_s_top = 0;
		++d_s_top;
		do {
			assert((end >= begin));
			if (depth < depth_limit && (end - begin > 1000)) {
				iter p1;
				iter p2;
				
				Partitioner<iter, Compare>::partition(begin, end, &p1, &p2, less);
				
				if(*p1 == *p2){
					//Disregard the middle partition
					int part1 = p1 - begin;
					int part3 = end - p2;
					if (part1 > part3) {
						*s = begin;
						*(s + 1) = p1;
						begin = p2 + 1;
					}
					else {
						*s = p2 + 1; 
						*(s + 1) = end;
						end = p1;
					}

					s += 2;
					depth++;
					*d_s_top = depth;
					++d_s_top;

				}
				else{
					//Attempt at making things smaller
					//Search, we remove all the elements equal to the pivot
					iter tmp2 = p2;
					while(*p2 == *(tmp2-1) && less(*p1, *(tmp2-1))) 
						tmp2--;

					//Search in the middle
					iter tmp1 = p1;
					while(*p1 == *(tmp1+1) && less(*(tmp1+1), *end))
						tmp1++;
					
					int part1 = p1 - begin;
					int part2 = tmp2 - tmp1;
					int part3 = end - p2;

					if (part2 > part1 && part3 > part1) { //Partition 1 is the smallest
						*s = tmp1 + 1;
						*(s + 1) = tmp2;
						*(s + 2) = p2 + 1;
						*(s + 3) = end;
						end = p1;
					}
					else if(part1 > part2 && part3 > part2) { //Partition 2 is the smallest
						*s = begin;
						*(s + 1) = p1;
						*(s + 2) = p2 + 1;
						*(s + 3) = end;
						begin = tmp1 + 1;
						end = tmp2;
					}
					else {
						*s = begin;
						*(s + 1) = p1;
						*(s + 2) = tmp1 + 1;
						*(s + 3) = tmp2;
						begin = p2 + 1;
					}
					assert(begin <= end);
					s += 4;
					*d_s_top = ++depth;
					*(d_s_top+1) = ++depth;
					d_s_top += 2;
				}
				

			}
			else {
				
				if (end - begin > 1000) { // if recursion depth limit exceeded
#ifdef PARTIAL_SORT_COUNT
					partial_sort_count++;
#endif
					std::partial_sort(begin, end, end);
				}
#ifndef NOINSERTIONSORT
				else {
					quicksort::qsort<partition::Hoare_block_partition_mosqrt>(begin, end, less);
				}
#endif 			
				//pop new subarray from stack
				s -= 2;
				begin = *s;
				end = *(s + 1);
				--d_s_top;
				assert(d_s_top != (depth_stack-1));
				depth = *d_s_top;
				assert(begin <= end);
			}
		} while (s != stack);
	}



}