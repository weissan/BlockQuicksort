/******************************************************************************
* partition.h
*
* Different partition algorithms with interfaces for different pivot selection strategies: 
* 4x block partition (simple, loops unrolled, loops unrolled + duplicate check, Hoare finish),
* Lomuto partitioner by Elmasry, Katajainen and Stenmark, and Hoare parititioner
*
******************************************************************************
* Copyright (C) 2016 Stefan Edelkamp <edelkamp@tzi.de>
* Copyright (C) 2016 Armin Weiﬂ <armin.weiss@fmi.uni-stuttgart.de>
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
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <string>
#include <stdlib.h>
#include <ctime>
#include <cmath>
#include <assert.h>
#include <functional>

//#defineMOREPARTITIONERS
#ifndef BLOCKSIZE
#define BLOCKSIZE 128
#endif
#ifndef PIVOTSAMPLESIZE
#define PIVOTSAMPLESIZE 23
#endif
#ifndef MO3_THRESH
#define MO3_THRESH (PIVOTSAMPLESIZE*PIVOTSAMPLESIZE)
#endif


namespace partition {

	//agrees with median::sort_pair
	template<typename iter, typename Compare>
	inline void leanswap(iter i1, iter i2, Compare less) {
		using t = typename std::iterator_traits<iter>::value_type;
		bool smaller = less(*i2, *i1);
		t temp = std::move(smaller ? *i1 : temp);
		*i1 = std::move(smaller ? *i2 : *i1);
		*i2 = std::move(smaller ? temp : *i2);
	}

	//pivot choice for Tuned Quicksort by Elmasry, Katajainen, and Stenmark
	template<typename iter, typename Compare>
	inline iter pivot(iter p, iter r, Compare less) {
		iter last = r - 1;
		iter q = p + (r - p) / 2;
		iter v = less(*q, *p) ? p : q;
		v = less(*v, *last) ? last : v;
		iter i = (v == p) ? q : p;
		iter j = (v == last) ? q : last;
		return less(*j, *i) ? i : j;
	}

	//Tuned Quicksort by Elmasry, Katajainen, and Stenmark
	//Code from http://www.diku.dk/~jyrki/Myris/Kat2014S.html
	template<typename iter, typename Compare>
	struct Lomuto_partition {
		static inline iter partition(iter begin, iter end, Compare less) {
			typedef typename std::iterator_traits<iter>::difference_type index;
			typedef typename std::iterator_traits<iter>::value_type t;
			iter q = pivot(begin, end, less);
			t v = std::move(*q);
			iter first = begin;
			*q = std::move(*first);
			q = first + 1;
			while (q < end) {
				t x = std::move(*q);
				bool smaller = less(x, v);
				begin += smaller; // smaller = 1 ? begin++ : begin
				index delta = smaller * (q - begin);
				iter s = begin + delta; // smaller = 1 => s = q : s = begin
				iter y = q - delta; // smaller = 1 => y = begin : y = q
				*s = std::move(*begin);
				*y = std::move(x);
				++q;
			}
			*first = std::move(*begin);
			*begin = std::move(v);
			return begin;
		}
	};


	template<typename iter, typename Compare>
	inline iter hoare_block_partition_simple(iter begin, iter end, iter pivot_position, Compare less) {
		typedef typename std::iterator_traits<iter>::difference_type index;
		index indexL[BLOCKSIZE], indexR[BLOCKSIZE];
		
		iter last = end - 1;
		std::iter_swap(pivot_position, last);
		const typename std::iterator_traits<iter>::value_type & pivot = *last;
		pivot_position = last;
		last--;

		int num_left = 0;
		int num_right = 0;
		int start_left = 0;
		int start_right = 0;
		int num;
		//main loop
		while (last - begin + 1 > 2 * BLOCKSIZE)
		{
			//Compare and store in buffers
			if (num_left == 0) {
				start_left = 0;
				for (index j = 0; j < BLOCKSIZE; j++) {
					indexL[num_left] = j;
					num_left += (!(less(begin[j], pivot)));				
				}
			}
			if (num_right == 0) {
				start_right = 0;
				for (index j = 0; j < BLOCKSIZE; j++) {
					indexR[num_right] = j;
					num_right += !(less(pivot, *(last - j)));				
				}
			}
			//rearrange elements
			num = std::min(num_left, num_right);
			for (int j = 0; j < num; j++)
				std::iter_swap(begin + indexL[start_left + j], last - indexR[start_right + j]);

			num_left -= num;
			num_right -= num;
			start_left += num;
			start_right += num;
			begin += (num_left == 0) ? BLOCKSIZE : 0;
			last -= (num_right == 0) ? BLOCKSIZE : 0;

		}//end main loop

		 //Compare and store in buffers final iteration
		index shiftR = 0, shiftL = 0;
		if (num_right == 0 && num_left == 0) {	//for small arrays or in the unlikely case that both buffers are empty
			shiftL = ((last - begin) + 1) / 2;
			shiftR = (last - begin) + 1 - shiftL;
			assert(shiftL >= 0); assert(shiftL <= BLOCKSIZE);
			assert(shiftR >= 0); assert(shiftR <= BLOCKSIZE);
			start_left = 0; start_right = 0;
			for (index j = 0; j < shiftL; j++) {
				indexL[num_left] = j;
				num_left += (!less(begin[j], pivot));
				indexR[num_right] = j;
				num_right += !less(pivot, *(last - j));
			}
			if (shiftL < shiftR)
			{
				assert(shiftL + 1 == shiftR);
				indexR[num_right] = shiftR - 1;
				num_right += !less(pivot, *(last - shiftR + 1));
			}
		}
		else if (num_right != 0) {
			shiftL = (last - begin) - BLOCKSIZE + 1;
			shiftR = BLOCKSIZE;
			assert(shiftL >= 0); assert(shiftL <= BLOCKSIZE); assert(num_left == 0);
			start_left = 0;
			for (index j = 0; j < shiftL; j++) {
				indexL[num_left] = j;
				num_left += (!less(begin[j], pivot));
			}
		}
		else {
			shiftL = BLOCKSIZE;
			shiftR = (last - begin) - BLOCKSIZE + 1;
			assert(shiftR >= 0); assert(shiftR <= BLOCKSIZE); assert(num_right == 0);
			start_right = 0;
			for (index j = 0; j < shiftR; j++) {
				indexR[num_right] = j;
				num_right += !(less(pivot, *(last - j)));
			}
		}

		//rearrange final iteration
		num = std::min(num_left, num_right);
		for (int j = 0; j < num; j++)
			std::iter_swap(begin + indexL[start_left + j], last - indexR[start_right + j]);

		num_left -= num;
		num_right -= num;
		start_left += num;
		start_right += num;
		begin += (num_left == 0) ? shiftL : 0;
		last -= (num_right == 0) ? shiftR : 0;			
		//end final iteration


		//rearrange elements remaining in buffer
		if (num_left != 0)
		{
			
			assert(num_right == 0);
			int lowerI = start_left + num_left - 1;
			index upper = last - begin;
			//search first element to be swapped
			while (lowerI >= start_left && indexL[lowerI] == upper) {
				upper--; lowerI--;
			}
			while (lowerI >= start_left)
				std::iter_swap(begin + upper--, begin + indexL[lowerI--]);

			std::iter_swap(pivot_position, begin + upper + 1); // fetch the pivot 
			return begin + upper + 1;
		}
		else if (num_right != 0) {
			assert(num_left == 0);
			int lowerI = start_right + num_right - 1;
			index upper = last - begin;
			//search first element to be swapped
			while (lowerI >= start_right && indexR[lowerI] == upper) {
				upper--; lowerI--;
			}
			
			while (lowerI >= start_right)
				std::iter_swap(last - upper--, last - indexR[lowerI--]);

			std::iter_swap(pivot_position, last - upper);// fetch the pivot 
			return last - upper;
		}
		else { //no remaining elements
			assert(last + 1 == begin);
			std::iter_swap(pivot_position, begin);// fetch the pivot 
			return begin;
		}
	}



	template< typename iter, typename Compare>
	inline iter hoare_block_partition_unroll_loop(iter begin, iter end, iter pivot_pos, Compare less ) {
		using t = typename std::iterator_traits<iter>::value_type;
		iter last = end - 1;
		int indexL[BLOCKSIZE], indexR[BLOCKSIZE];

		t pivot = std::move(*pivot_pos);
		*pivot_pos = std::move(*last);
		iter hole = last;
		t temp;
		last--;

		int num_left = 0;
		int num_right = 0;
		int start_left = 0;
		int start_right = 0;

		int j;
		int num;
		//main loop
		while (last - begin + 1 > 2 * BLOCKSIZE)
		{
			//Compare and store in buffers
			if (num_left == 0) {
				start_left = 0;
				for (j = 0; j < BLOCKSIZE; ) {
					indexL[num_left] = j;
					num_left += (!(less(begin[j], pivot)));
					j++;
					indexL[num_left] = j;
					num_left += (!(less(begin[j], pivot)));
					j++;
					indexL[num_left] = j;
					num_left += (!(less(begin[j], pivot)));
					j++;
					indexL[num_left] = j;
					num_left += (!(less(begin[j], pivot)));
					j++;
				}
			}
			if (num_right == 0) {
				start_right = 0;
				for (j = 0; j < BLOCKSIZE; ) {
					indexR[num_right] = j;
					num_right += !(less(pivot, *(last - j)));
					j++;
					indexR[num_right] = j;
					num_right += !(less(pivot, *(last - j)));
					j++;
					indexR[num_right] = j;
					num_right += !(less(pivot, *(last - j)));
					j++;
					indexR[num_right] = j;
					num_right += !(less(pivot, *(last - j)));
					j++;
				}
			}
			//rearrange elements
			num = std::min(num_left, num_right);
			if (num != 0)
			{
				*hole = std::move(*(begin + indexL[start_left]));
				*(begin + indexL[start_left]) = std::move(*(last - indexR[start_right]));
				for (j = 1; j < num; j++)
				{
					*(last - indexR[start_right + j - 1]) = std::move(*(begin + indexL[start_left + j]));
					*(begin + indexL[start_left + j]) = std::move(*(last - indexR[start_right + j]));
				}
				hole = (last - indexR[start_right + num - 1]);
			}
			num_left -= num;
			num_right -= num;
			start_left += num;
			start_right += num;
			begin += (num_left == 0) ? BLOCKSIZE : 0;
			last -= (num_right == 0) ? BLOCKSIZE : 0;
		}//end main loop

		 //Compare and store in buffers final iteration
		int shiftR = 0, shiftL = 0;
		if (num_right == 0 && num_left == 0) {	//for small arrays or in the unlikely case that both buffers are empty
			shiftL = (int)((last - begin) + 1) / 2;
			shiftR = (int)(last - begin) + 1 - shiftL;
			assert(shiftL >= 0); assert(shiftL <= BLOCKSIZE);
			assert(shiftR >= 0); assert(shiftR <= BLOCKSIZE);
			start_left = 0; start_right = 0;
			for (j = 0; j < shiftL; j++) {
				indexL[num_left] = j;
				num_left += (!less(begin[j], pivot));
				indexR[num_right] = j;
				num_right += !less(pivot, *(last - j));
			}
			if (shiftL < shiftR)
			{
				assert(shiftL + 1 == shiftR);
				indexR[num_right] = j;
				num_right += !less(pivot, *(last - j));
			}
		}
		else if ((last - begin) + 1 - BLOCKSIZE <= 2 * BLOCKSIZE - (start_right + num_right + start_left + num_left) && (num_right + num_left) < BLOCKSIZE / 3) {
			int upper_right = start_right + num_right;
			int upper_left = start_left + num_left;
			assert((last - begin) - BLOCKSIZE + 1 > 0);
			shiftL = (int)(((last - begin) + 1 - BLOCKSIZE) / 2); // +2*(num_right + num_left)  //- num_left
			shiftR = (int)(last - begin) - BLOCKSIZE + 1 - shiftL;
			if (shiftL > BLOCKSIZE - upper_left)
			{
				shiftR += shiftL - (BLOCKSIZE - upper_left);
				shiftL = BLOCKSIZE - upper_left;
			}
			else if (shiftL < 0)
			{
				shiftR -= shiftL;
				shiftL = 0;
			}
			if (shiftR > BLOCKSIZE - upper_right)
			{
				shiftL += shiftR - (BLOCKSIZE - upper_right);
				shiftR = BLOCKSIZE - upper_right;
			}
			else if (shiftR < 0)
			{
				shiftL -= shiftR;
				shiftR = 0;
			}

			assert(shiftL + shiftR + BLOCKSIZE == (last - begin) + 1);
			assert(shiftL >= 0); assert(shiftL <= BLOCKSIZE - upper_left);
			assert(shiftR >= 0); assert(shiftR <= BLOCKSIZE - upper_right);

			int j_L = 0;
			int j_R = 0;
			if (num_left != 0) {
				shiftL += BLOCKSIZE;
				j_L = BLOCKSIZE;
			}
			if (num_right != 0) {
				shiftR += BLOCKSIZE;
				j_R = BLOCKSIZE;
			}

			for (; j_L < shiftL; j_L++) {
				indexL[upper_left] = j_L;
				upper_left += (!less(begin[j_L], pivot));
			}
			num_left = upper_left - start_left;

			for (; j_R < shiftR; j_R++) {
				indexR[upper_right] = j_R;
				upper_right += !(less(pivot, *(last - j_R)));
			}
			num_right = upper_right - start_right;
		}
		else if (num_right != 0) {
			shiftL = (int)(last - begin) - BLOCKSIZE + 1;
			shiftR = BLOCKSIZE;
			assert(shiftL >= 0); assert(shiftL <= BLOCKSIZE); assert(num_left == 0);
			start_left = 0;
			for (j = 0; j < shiftL; j++) {
				indexL[num_left] = j;
				num_left += (!less(begin[j], pivot));
			}
		}
		else {
			shiftL = BLOCKSIZE;
			shiftR = (int)(last - begin) - BLOCKSIZE + 1;
			assert(shiftR >= 0); assert(shiftR <= BLOCKSIZE); assert(num_right == 0);
			start_right = 0;
			for (j = 0; j < shiftR; j++) {
				indexR[num_right] = j;
				num_right += !(less(pivot, *(last - j)));
			}
		}

		//rearrange final iteration
		num = std::min(num_left, num_right);
		if (num != 0)
		{
			*hole = std::move(*(begin + indexL[start_left]));
			*(begin + indexL[start_left]) = std::move(*(last - indexR[start_right]));
			for (j = 1; j < num; j++)
			{
				*(last - indexR[start_right + j - 1]) = std::move(*(begin + indexL[start_left + j]));
				*(begin + indexL[start_left + j]) = std::move(*(last - indexR[start_right + j]));
			}
			hole = (last - indexR[start_right + num - 1]);
		}
		num_left -= num;
		num_right -= num;
		start_left += num;
		start_right += num;

		if (num_left == 0)
			begin += shiftL;
		if (num_right == 0)
			last -= shiftR;

		/*	std::cout << "Partition check" << std::endl;
		for (iter it = bbegin; it != begin;  it++)
		if(*it > pivot)
		std::cout << "vorne" << begin - it << ", " << it->value() << std::endl;
		for (iter it = last + 1; it != eend ;  it++)
		if (*it < pivot)
		std::cout <<"hinten" << it-last << ", " << it->value() << std::endl;
		;*/

		//rearrange remaining elements
		if (num_left != 0)
		{
			assert(num_right == 0);
			int lowerI = start_left + num_left - 1;
			int upper = (int)(last - begin);
			while (lowerI >= start_left && indexL[lowerI] == upper)
			{
				upper--; lowerI--;
			}
			temp = std::move(*(begin + upper));
			while (lowerI >= start_left)
			{
				*(begin + upper) = std::move(*(begin + indexL[lowerI]));
				*(begin + indexL[lowerI]) = std::move(*(begin + (--upper)));
				lowerI--;
			}
			*(begin + upper) = std::move(temp);
			*hole = std::move(*(begin + upper + 1));

			*(begin + upper + 1) = std::move(pivot); // fetch the pivot 
			return begin + upper + 1;
			
		}
		else if (num_right != 0) {
			assert(num_left == 0);
			int lowerI = start_right + num_right - 1;
			int upper = (int)(last - begin);
			while (lowerI >= start_right && indexR[lowerI] == upper)
			{
				upper--; lowerI--;
			}
			*hole = std::move(*(last - upper));
			while (lowerI >= start_right)
			{
				*(last - upper) = std::move(*(last - indexR[lowerI]));
				*(last - indexR[lowerI--]) = std::move(*(last - (--upper)));
			}
			
			*(last - upper) = std::move(pivot); // fetch the pivot 
			return last - upper;
		
		}
		else { //no remaining elements
			assert(last + 1 == begin);
			*hole = std::move(*begin);
			*begin = std::move(pivot); // fetch the pivot 
			return begin;
		}
	}

	// with check for elements equal to pivot -- requires that *(begin - 1) <= *pivot_pos <= *end   (in particular these positions must exist)
	template< typename iter, typename Compare>
	inline iter hoare_block_partition_unroll_loop(iter begin, iter end, iter pivot_pos, Compare less, int & pivot_length) {
		using t = typename std::iterator_traits<iter>::value_type;
		using index = typename std::iterator_traits<iter>::difference_type;
		iter last = end - 1;
		iter temp_begin = begin;
		int indexL[BLOCKSIZE], indexR[BLOCKSIZE];

		bool double_pivot_check = ((!less(*pivot_pos, *end)) || (!(less(*(begin - 1), *pivot_pos))));
		pivot_length = 1;

		t pivot = std::move(*pivot_pos);
		*pivot_pos = std::move(*last);
		iter hole = last;
		t temp;
		last--;

		int num_left = 0;
		int num_right = 0;
		int start_left = 0;
		int start_right = 0;
		int j;
		int num;

		bool small_array = (last - begin + 1 <= 2 * BLOCKSIZE) && ((last - begin) > 48);
		//main loop
		while (last - begin + 1 > 2 * BLOCKSIZE)
		{
			//Compare and store in buffers
			if (num_left == 0) {
				start_left = 0;
				for (j = 0; j < BLOCKSIZE; ) {
					indexL[num_left] = j;
					num_left += (!(less(begin[j], pivot)));
					j++;
					indexL[num_left] = j;
					num_left += (!(less(begin[j], pivot)));
					j++;
					indexL[num_left] = j;
					num_left += (!(less(begin[j], pivot)));
					j++;
					indexL[num_left] = j;
					num_left += (!(less(begin[j], pivot)));
					j++;
				}
			}
			if (num_right == 0) {
				start_right = 0;
				for (j = 0; j < BLOCKSIZE; ) {
					indexR[num_right] = j;
					num_right += !(less(pivot, *(last - j)));
					j++;
					indexR[num_right] = j;
					num_right += !(less(pivot, *(last - j)));
					j++;
					indexR[num_right] = j;
					num_right += !(less(pivot, *(last - j)));
					j++;
					indexR[num_right] = j;
					num_right += !(less(pivot, *(last - j)));
					j++;
				}
			}
			//rearrange elements
			num = std::min(num_left, num_right);
			if (num != 0)
			{
				*hole = std::move(*(begin + indexL[start_left]));
				*(begin + indexL[start_left]) = std::move(*(last - indexR[start_right]));
				for (j = 1; j < num; j++)
				{
					*(last - indexR[start_right + j - 1]) = std::move(*(begin + indexL[start_left + j]));
					*(begin + indexL[start_left + j]) = std::move(*(last - indexR[start_right + j]));
				}
				hole = (last - indexR[start_right + num - 1]);
			}
			num_left -= num;
			num_right -= num;
			start_left += num;
			start_right += num;
			begin += (num_left == 0) ? BLOCKSIZE : 0;
			last -= (num_right == 0) ? BLOCKSIZE : 0;
		}//end main loop

		if (num_left == 0) start_left = 0;
		if (num_right == 0) start_right = 0;

		 //Compare and store in buffers final iteration
		int shiftR = 0, shiftL = 0;
		if (num_right == 0 && num_left == 0) {	//for small arrays or in the unlikely case that both buffers are empty
			shiftL = (int)((last - begin) + 1) / 2;
			shiftR = (int)(last - begin) + 1 - shiftL;
			assert(shiftL >= 0); assert(shiftL <= BLOCKSIZE);
			assert(shiftR >= 0); assert(shiftR <= BLOCKSIZE);
			start_left = 0; start_right = 0;
			for (j = 0; j < shiftL; j++) {
				indexL[num_left] = j;
				num_left += (!less(begin[j], pivot));
				indexR[num_right] = j;
				num_right += !less(pivot, *(last - j));
			}
			if (shiftL < shiftR)
			{
				assert(shiftL + 1 == shiftR);
				indexR[num_right] = j;
				num_right += !less(pivot, *(last - j));
			}
		}
		else if (num_right != 0) {
			shiftL = (int)(last - begin) - BLOCKSIZE + 1;
			shiftR = BLOCKSIZE;
			assert(shiftL >= 0); assert(shiftL <= BLOCKSIZE); assert(num_left == 0);
			start_left = 0;
			for (j = 0; j < shiftL; j++) {
				indexL[num_left] = j;
				num_left += (!less(begin[j], pivot));
			}
		}
		else {
			shiftL = BLOCKSIZE;
			shiftR = (int)(last - begin) - BLOCKSIZE + 1;
			assert(shiftR >= 0); assert(shiftR <= BLOCKSIZE); assert(num_right == 0);
			start_right = 0;
			for (j = 0; j < shiftR; j++) {
				indexR[num_right] = j;
				num_right += !(less(pivot, *(last - j)));
			}
		}

		//rearrange final iteration
		num = std::min(num_left, num_right);
		if (num != 0)
		{
			*hole = std::move(*(begin + indexL[start_left]));
			*(begin + indexL[start_left]) = std::move(*(last - indexR[start_right]));
			for (j = 1; j < num; j++)
			{
				*(last - indexR[start_right + j - 1]) = std::move(*(begin + indexL[start_left + j]));
				*(begin + indexL[start_left + j]) = std::move(*(last - indexR[start_right + j]));
			}
			hole = (last - indexR[start_right + num - 1]);
		}
		num_left -= num;
		num_right -= num;
		start_left += num;
		start_right += num;

		if (num_left == 0)
			begin += shiftL;
		if (num_right == 0)
			last -= shiftR;

		/*	std::cout << "Partition check" << std::endl;
		for (iter it = bbegin; it != begin;  it++)
		if(*it > pivot)
		std::cout << "vorne" << begin - it << ", " << it->value() << std::endl;
		for (iter it = last + 1; it != eend ;  it++)
		if (*it < pivot)
		std::cout <<"hinten" << it-last << ", " << it->value() << std::endl;
		;*/

		//rearrange remaining elements
		if (num_left != 0)
		{
			assert(num_right == 0);
			int lowerI = start_left + num_left - 1;
			int upper = (int)(last - begin);
			while (lowerI >= start_left && indexL[lowerI] == upper)
			{
				upper--; lowerI--;
			}
			temp = std::move(*(begin + upper));
			while (lowerI >= start_left)
			{
				*(begin + upper) = std::move(*(begin + indexL[lowerI]));
				*(begin + indexL[lowerI]) = std::move(*(begin + (--upper)));
				lowerI--;
			}
			*(begin + upper) = std::move(temp);
			*hole = std::move(*(begin + upper + 1));

			//check for double elements if the pivot sample has repetitions or a small array is partitioned very unequal
			if (double_pivot_check || (small_array && num_left >= (15 * shiftL) / 16)) {
				iter begin_lomuto = begin + upper + 1;
				iter q = begin_lomuto + 1;

				//check at least 4 elements whether they are equal to the pivot using Elmasry, Katajainen and Stenmark's Lomuto partitioner
				unsigned int count_swaps = 1;
				unsigned int count_steps = 0;
				while (q < end && (count_swaps << 2) > count_steps) { //continue as long as there are many elements equal to pivot
					typename std::iterator_traits<iter>::value_type x = std::move(*q);
					bool smaller = !less(pivot, x);
					begin_lomuto += smaller; // smaller = 1 ? begin++ : begin
					count_swaps += smaller;
					index delta = smaller * (q - begin_lomuto);
					iter s = begin_lomuto + delta; // smaller = 1 => s = q : s = begin
					iter y = q - delta; // smaller = 1 => y = begin : y = q
					*s = std::move(*begin_lomuto);
					*y = std::move(x);
					++q;
					count_steps++;
				}

				pivot_length = begin_lomuto + 1 - (begin + upper + 1);

			//	std::cout << "check for double elements left" << pivot_length << " of " << num_left << " array size " << end - temp_begin << std::endl;
			}
			*(begin + upper + 1) = std::move(pivot); // fetch the pivot 
			return begin + upper + 1;

		}
		else if (num_right != 0) {
			assert(num_left == 0);
			int lowerI = start_right + num_right - 1;
			int upper = (int)(last - begin);
			while (lowerI >= start_right && indexR[lowerI] == upper)
			{
				upper--; lowerI--;
			}
			*hole = std::move(*(last - upper));
			while (lowerI >= start_right)
			{
				*(last - upper) = std::move(*(last - indexR[lowerI]));
				*(last - indexR[lowerI--]) = std::move(*(last - (--upper)));
			}

			//check for double elements if the pivot sample has repetitions or a small array is partitioned very unequal
			if (double_pivot_check || (small_array && num_right >= (15 * shiftR) / 16)) {
				iter begin_lomuto = last - upper;
				iter q = begin_lomuto - 1;

				//check at least 4 elements whether they are equal to the pivot using Elmasry, Katajainen and Stenmark's Lomuto partitioner
				unsigned int count_swaps = 1;
				unsigned int count_steps = 0;
				while (q > temp_begin && (count_swaps << 2) > count_steps) { //continue as long as there are many elements equal to pivot
					typename std::iterator_traits<iter>::value_type x = std::move(*q);
					bool smaller = !less(x, pivot);
					begin_lomuto -= smaller; // smaller = 1 ? begin++ : begin
					count_swaps += smaller;
					index delta = smaller * (q - begin_lomuto);
					iter s = begin_lomuto + delta; // smaller = 1 => s = q : s = begin
					iter y = q - delta; // smaller = 1 => y = begin : y = q
					*s = std::move(*begin_lomuto);
					*y = std::move(x);
					--q;
					count_steps++;
				}

				pivot_length = (last - upper) + 1 - begin_lomuto;
				*(last - upper) = std::move(pivot); // fetch the pivot 
				return begin_lomuto;
			}
			else
			{
				*(last - upper) = std::move(pivot); // fetch the pivot 
				return last - upper;
			}


		}
		else { //no remaining elements
			assert(last + 1 == begin);
			*hole = std::move(*begin);
			*begin = std::move(pivot); // fetch the pivot 
			return begin;
		}
	}


	template<typename iter, typename Compare>
	struct Hoare_block_partition_hoare_finish {
		static inline iter partition(iter begin, iter end, Compare less) {
			typedef typename std::iterator_traits<iter>::value_type t;
			iter last = end - 1;
			iter mid = begin + ((end - begin) / 2);
			unsigned char indexL[BLOCKSIZE], indexR[BLOCKSIZE];
			if (less(*mid, *begin)) {
				if (less(*last, *begin)) {
					if (less(*mid, *last)) {
						std::swap(*begin, *last);
					}
					else {
						t temp = std::move(*mid);
						*mid = std::move(*last);
						*last = std::move(*begin);
						*begin = std::move(temp);
					}
				}
			}
			else { // mid > begin 
				if (less(*last, *begin)) { // mid > begin > last 
					std::swap(*mid, *last);
				}
				else {
					if (less(*mid, *last)) { // begin < mid < last 
						std::swap(*begin, *mid);
					}
					else { // begin < mid, mid > last
						t temp = std::move(*mid);
						*mid = std::move(*begin);
						*begin = std::move(*last);
						*last = std::move(temp);
					}
				}
			}

			t q = std::move(*begin);
			mid = begin++;
			t temp;
			last--;
			int iL = 0;
			int iR = 0;
			int sL = 0;
			int sR = 0;
			int j;
			int num;
			while (last - begin + 1 > 2 * BLOCKSIZE) {
				if (iL == 0) {
					sL = 0;
					for (j = 0; j < BLOCKSIZE; j++) {
						indexL[iL] = j;
						iL += ! less(begin[j], q);
					}
				}
				if (iR == 0) {
					sR = 0;
					for (j = 0; j < BLOCKSIZE; j++) {
						indexR[iR] = j;
						iR += ! less(q, (*(last - j)));
					}
				}
				num = std::min(iL, iR);
				if (num != 0) {
					temp = std::move(*(begin + indexL[sL]));
					*(begin + indexL[sL]) = std::move(*(last - indexR[sR]));
					for (j = 1; j < num; j++) {
						*(last - indexR[sR + j - 1]) = std::move(*(begin + indexL[sL + j]));
						*(begin + indexL[sL + j]) = std::move(*(last - indexR[sR + j]));
					}
					*(last - indexR[sR + num - 1]) = std::move(temp);
				}
				iL -= num;
				iR -= num;
				sL += num;
				sR += num;
				if (iL == 0)
					begin += BLOCKSIZE;
				if (iR == 0)
					last -= BLOCKSIZE;
			}
			begin--;
			last++;
		loop:
			do; while (less(*(++begin), q));
			do; while (less(q, *(--last)));
			if (begin <= last) {
				std::swap(*begin, *last);
				goto loop;
			}
			std::swap(*mid, *last);
			return last;
		}
	};


	template<typename iter, typename Compare>
	struct Hoare_block_partition {
		static inline iter partition(iter begin, iter end, Compare less) {
			//choose pivot
			iter mid = median::median_of_3(begin, end, less);
			//partition
			return hoare_block_partition_unroll_loop(begin + 1, end - 1, mid, less);
		}

		//with duplicate check
		static inline iter partition(iter begin, iter end, Compare less, int & pivot_length){
			//choose pivot
			iter mid = median::median_of_3(begin, end, less);
			//partition
			return hoare_block_partition_unroll_loop(begin + 1, end - 1, mid, less, pivot_length);
		}
		static inline iter partition(iter begin, iter end, iter pivot, Compare less) {
			//partition
			return hoare_block_partition_unroll_loop(begin + 1, end - 1, pivot, less);
		}
	};

	template<typename iter, typename Compare>
	struct Hoare_block_partition_simple {
		static inline iter partition(iter begin, iter end, Compare less) {
			//choose pivot
			iter mid = median::median_of_3(begin, end, less);
			//partition
			return hoare_block_partition_simple(begin + 1, end - 1, mid, less);
		}
		static inline iter partition(iter begin, iter end, iter pivot, Compare less) {
			//partition
			return hoare_block_partition_simple(begin + 1, end - 1, pivot, less);
		}
	};


	template<typename iter, typename Compare>
	struct Hoare_block_partition_Mo5 {
		static inline iter partition(iter begin, iter end, Compare less) {
			//choose pivot
			iter mid;
			if (end - begin > 30)
				mid = median::median_of_5(begin, end, less);
			else
				mid = median::median_of_3(begin, end, less);
			//partition
			return hoare_block_partition_unroll_loop(begin + 1, end - 1, mid, less);
		}
	};

	template<typename iter, typename Compare>
	struct Hoare_block_partition_median_of_3_medians_of_3 {
		static inline iter partition(iter begin, iter end, Compare less) {
			//choose pivot
			iter mid;
			if (end - begin > 70)
				mid = median::median_of_3_medians_of_3(begin, end, less);
			else
				mid = median::median_of_3(begin, end, less);
			//partition
			return hoare_block_partition_unroll_loop(begin + 1, end - 1, mid, less);
		}
	};

	template<typename iter, typename Compare>
	struct Hoare_block_partition_macro_pivot {
		static inline iter partition(iter begin, iter end, Compare less) {
			//choose pivot
			iter mid;
			if (end - begin > MO3_THRESH)
			{
				mid = median::median_of_k<PIVOTSAMPLESIZE>(begin, end, less);
				return hoare_block_partition_unroll_loop(begin + PIVOTSAMPLESIZE / 2, end - PIVOTSAMPLESIZE / 2, mid, less);
			}
			else {
				mid = median::median_of_3(begin, end, less);
				return hoare_block_partition_unroll_loop(begin + 1, end - 1, mid, less);
			}		
		}
	};

	
	template<typename iter, typename Compare>
	struct Hoare_block_partition_median_of_3_medians_of_5 {
		static inline iter partition(iter begin, iter end, Compare less) {
			//choose pivot
			iter mid;
			if (end - begin > 200)
				mid = median::median_of_3_medians_of_5(begin, end, less);
			else
				mid = median::median_of_3(begin, end, less);
			//partition
			return hoare_block_partition_unroll_loop(begin + 1, end - 1, mid, less);
		}
	};

	template<typename iter, typename Compare>
	struct Hoare_block_partition_median_of_5_medians_of_5 {
		static inline iter partition(iter begin, iter end, Compare less) {
			//choose pivot
			iter mid;
			if (end - begin > 1000)
				mid = median::median_of_5_medians_of_5(begin, end, less);
			else if (end - begin > 100)
				mid = median::median_of_3_medians_of_3(begin, end, less);
			else
				mid = median::median_of_3(begin, end, less);
			//partition
			return hoare_block_partition_unroll_loop(begin + 1, end - 1, mid, less);
			
			
		}
	};

	template<typename iter, typename Compare>
	struct Hoare_block_partition_mosqrt {
		static inline iter partition(iter begin, iter end, Compare less) {
			iter mid;
			if (end - begin > 20000)
			{
				unsigned int pivot_sample_size = sqrt(end - begin);
				pivot_sample_size += (1 - (pivot_sample_size % 2));//make it an odd number
				mid = median::median_of_k(begin, end, less, pivot_sample_size); //choose pivot as median of sqrt(n)
				//partition
				return hoare_block_partition_unroll_loop(begin + pivot_sample_size / 2, end - pivot_sample_size / 2, mid, less);
			}
			else
			{
				if (end - begin > 800)
					mid = median::median_of_5_medians_of_5(begin, end, less);
				else if (end - begin > 100)
					mid = median::median_of_3_medians_of_3(begin, end, less);
				else
					mid = median::median_of_3(begin, end, less);
				//partition
				return hoare_block_partition_unroll_loop(begin + 1, end - 1, mid, less);
			}
			

		}

		//with duplicate check
		static inline iter partition(iter begin, iter end, Compare less, int & pivot_length) {
			iter mid;
			if (end - begin > 20000)
			{
				unsigned int pivot_sample_size = sqrt(end - begin);
				pivot_sample_size += (1 - (pivot_sample_size % 2));//make it an odd number
				mid = median::median_of_k(begin, end, less, pivot_sample_size);//choose pivot as median of sqrt(n)
				//partition
				return hoare_block_partition_unroll_loop(begin + pivot_sample_size / 2, end - pivot_sample_size / 2, mid, less, pivot_length);
			}
			else
			{
				if (end - begin > 800)
					mid = median::median_of_5_medians_of_5(begin, end, less);
				else if (end - begin > 100)
					mid = median::median_of_3_medians_of_3(begin, end, less);
				else
					mid = median::median_of_3(begin, end, less);
				//mid = median::median_of_3(begin , end , less);
				return hoare_block_partition_unroll_loop(begin + 1, end - 1, mid, less, pivot_length);
			}
		}
	};


	template<typename iter, typename Compare>
	struct Hoare_partition {
		static inline iter partition(iter begin, iter end, Compare less) {
			using t = typename std::iterator_traits<iter>::value_type;
			iter last = end - 1;
			iter mid = begin + ((end - begin) / 2);

			leanswap(begin, last, less);
			leanswap(mid, last, less);
			leanswap(mid, begin, less);

			t q = std::move(*begin);
			mid = begin;
		loop:
			do; while (less(*(++begin), q));
			do; while (less(q , *(--last)));
			if (begin <= last) {
				std::swap(*begin, *last);
				goto loop;
			}
			std::swap(*mid, *last);
			return last;
		}
	};


};