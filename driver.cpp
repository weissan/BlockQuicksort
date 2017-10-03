/******************************************************************************
* driver.cpp
*
* Driver to benchmark sorting algorithms. Comparison and move measurements do NOT work for all algorithms.
* Modified version of sort-driver.c++ from http://www.diku.dk/~jyrki/Myris/Kat2014S.html
******************************************************************************
* Modified (more test cases etc.) by
* Copyright (C) 2016 Stefan Edelkamp <edelkamp@tzi.de>
* Copyright (C) 2016 Armin Weiß <armin.weiss@fmi.uni-stuttgart.de>
******************************************************************************
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
*******************************************************************************
*
* Copyright (C) 2014 Amr Elmasry
* Copyright (C) 2014 Jyrki Katajainen
* Copyright (C) 2014 Max Stenmark
*
*****************************************************************************/

#if ! defined(MAXSIZE)
#define MAXSIZE (1024 * 1024 * 1024)
#endif


//Maximal number of bytes to sort
#ifndef BYTESTOSORT
#define BYTESTOSORT (128 * 1024 * 1024)
#endif

#include <algorithm>  	
#include <functional> 	
#include <iostream>   	
#include <iterator>   	
#include <string> 
#include <cmath> 
#include <random>
#include <string>
#include <chrono>
#include <typeinfo>



//to print types
#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define ALG_NAME STR(NAME)


#ifndef TYPE
#define TYPE int32_t
#endif


#if not __APPLE__
	extern int ilogb(double) throw();
#endif

//for counting numbers of calls to std::partial_sort
#define PARTIAL_SORT_COUNT
long long volatile partial_sort_count = 0;

#ifndef IS_THRESH
#define IS_THRESH (1<<4)
#endif // !1IS_THRESH

#include "algorithm.h++"



//measure comparisons does NOT work for all algorithms
#ifdef MEASURE_COMPARISONS

long long volatile comparisons = 0;

template<typename T>
class counting_comparator {
public:

  using first_argument_type = T;
  using second_argument_type = T;
  using result_type = bool;

  bool operator()(T const& a, T const& b) const {
    ++comparisons;
    return a < b;
  }
};

template<typename T>
class vector_comparator {
public:
  using first_argument_type = T;
  using second_argument_type = T;
  using result_type = bool;
  bool operator()(std::vector<T> const& a, std::vector<T> const& b) const {
	  if(a.size() < b.size())
		  return true;
	  if(b.size() < a.size())
		  return false;
	  if(a.size() == 0) // also b.size() == 0
		  return false;
    return a[0] < b[0];
  }
};
#endif


class Rational {
private:
	int numerator;
	unsigned int denominator;
public:
	Rational(int v) : numerator(v), denominator(1) {};
	Rational() : numerator(0), denominator(1) {};
	double value(void) { return (double) numerator / (double) denominator; }
	bool operator<(const Rational &b) const { return numerator*(long long int) b.denominator < b.numerator*(long long int)denominator; }
	bool operator>(const Rational &b) const { return numerator*(long long int) b.denominator > b.numerator*(long long int)denominator; }
	bool operator<=(const Rational &b) const { return numerator*(long long int) b.denominator <= b.numerator*(long long int)denominator; }
	bool operator!=(const Rational &b) const { return numerator*(long long int) b.denominator != b.numerator*(long long int)denominator;  }
	bool operator==(const Rational &b) const { return numerator*(long long int) b.denominator == b.numerator*(long long int)denominator;  }
};


class Record {
private:
	int key;
	int blub[20];
public:
	Record(int v) : key(v) { blub[v % 10] = v;};
	Record() : key(0) {};
	int value(void) { return key; }
	bool operator<(const Record &b) const { return key < b.key; }
	bool operator>(const Record &b) const { return key > b.key; }
	bool operator<=(const Record &b) const { return key <= b.key; }
	bool operator!=(const Record &b) const { return key != b.key;  }
	bool operator==(const Record &b) const { return key == b.key;  }
};




#ifndef VECTORSIZE
#define VECTORSIZE 10
#endif
class Vector {
private:
	double components[VECTORSIZE];
public:
	Vector(int v) { for (int j = 0 ; j < VECTORSIZE ; j++) components[j] = v;};
	Vector() {};
	double value(void) { 		
		double abs_this = 0;
		for (int j = 0 ; j < VECTORSIZE ; j++){
				 abs_this += components[j]*components[j];
		}
		return abs_this;
	}
	//compare Euclidean norm
	bool operator<(const Vector &b) const {
		double abs_this = 0, abs_b = 0;
		 for (int j = 0 ; j < VECTORSIZE ; j++){
			 abs_this += components[j]*components[j];
			 abs_b += b.components[j]*b.components[j];
		 }
		return abs_this < abs_b;
		}
	bool operator>(const Vector &b) const {
		double abs_this = 0, abs_b = 0;
		 for (int j = 0 ; j < VECTORSIZE ; j++){
			 abs_this += components[j]*components[j];
			 abs_b += b.components[j]*b.components[j];
		 }
		return abs_this > abs_b;
		}
	bool operator<=(const Vector &b) const {
		double abs_this = 0, abs_b = 0;
		 for (int j = 0 ; j < VECTORSIZE ; j++){
			 abs_this += components[j]*components[j];
			 abs_b += b.components[j]*b.components[j];
		 }
		return abs_this <= abs_b;
		}
	bool operator!=(const Vector &b) const {
		double abs_this = 0, abs_b = 0;
		 for (int j = 0 ; j < VECTORSIZE ; j++){
			 abs_this += components[j]*components[j];
			 abs_b += b.components[j]*b.components[j];
		 }
		return abs_this != abs_b;
		}
	bool operator==(const Vector &b) const {
		double abs_this = 0, abs_b = 0;
		 for (int j = 0 ; j < VECTORSIZE ; j++){
			 abs_this += components[j]*components[j];
			 abs_b += b.components[j]*b.components[j];
		 }
		return abs_this == abs_b;
		}
};



//measure moves does NOT work for all algorithms
#ifdef MEASURE_MOVES
long long volatile moves = 0;

template<typename T>
class move_counter {
private:
  
  T datum;

  move_counter(move_counter const&) = delete;
  move_counter& operator=(move_counter const&) = delete;
		
public:

  explicit move_counter() 
    : datum(0) {
    moves += 1;
  }

  template<typename number>
  explicit move_counter(number x = 0) 
    : datum(x) {
    moves += 1;
  }

  move_counter(move_counter&& other) {
    datum = std::move(other.datum);
    moves += 1;
  }

	move_counter& operator=(move_counter&& other) {
    datum = std::move(other.datum);
    moves += 1;
    return *this;
  }

  operator T() const {
    return datum;
  }

  template<typename U>
  friend bool operator<(move_counter<U> const&, move_counter<U> const&);

  template<typename U>
  friend bool operator==(move_counter<U> const&, move_counter<U> const&);

};

template<typename T>
bool operator<(move_counter<T> const& x, move_counter<T> const& y) {
  return x.datum < y.datum;
}

template<typename T>
bool operator==(move_counter<T> const& x, move_counter<T> const& y) {
  return x.datum == y.datum;
}

#endif


void usage(int argc, char **argv) {
  std::cerr << "Usage: " << argv[0] 
	    << " <N> <b c d i m n o p q r s t u v w z> <seed> "
	    << std::endl;
  exit(1);
}

int main(int argc, char** argv) {

#ifdef MEASURE_MOVES

  using element = move_counter<int>;
  using C = std::less<element>;



#else
	#ifdef TYPE
		using element = TYPE;
	#else
		using element = int;
	#endif

	#ifdef MEASURE_COMPARISONS
		using C = counting_comparator<element>;  
	#else
		using C = std::less<element>;
	#endif
#endif

  unsigned int N = 15;
  unsigned int inversions = 0;
  unsigned long long int modulo = 2;
  unsigned long long int offset = 0;
	char method = 'i';
	std::string method_string = std::string("random");
	std::string param_string = std::string("0");
  if (argc == 2) {
    N = atoi(argv[1]);
  } else if (argc != 3 && argc != 4) {
    usage(argc, argv);
  } else {
    N = atoi(argv[1]);
    method = *argv[2];
  }

if(argc == 5)
{  
	inversions = atoi(argv[3]);
	modulo = inversions;
	offset = inversions;
//	param_string = std::to_string(atoi(argv[3]));
}
else	
{   
	inversions = N;
	modulo = (unsigned int)sqrt(N);
	offset = N / 2;
	if (method=='s' || method =='m'|| method =='o'|| method =='v') {
		param_string = "sqrt(N)";
		modulo = (unsigned int)sqrt(N);
		inversions = (unsigned int)sqrt(N);
	}
	if (method=='p'){
		param_string = "N";
		inversions = N;
	} 
	if (method=='q'){
		param_string = "NlogN";
		inversions = (int)((double)N*log(N));
	} 
	if (method=='n'){
		param_string = "N/2";
		modulo = N/2;
	} 
}

std::random_device rd;
unsigned long long int seed = 0;
if(argc == 5)
{  
	seed = atoi(argv[4]);
}
else if(argc == 4)
{  
	seed = atoi(argv[3]);
}
else	
{   
	seed = rd();
}
  
	  
  if (N < 1 || N > MAXSIZE) {
    std::cerr << "N out of bounds [1.."
	      << MAXSIZE
	      << "]"
	      << std::endl;
    usage(argc, argv);
  };
  switch (method) { 
   case 'b':
   method_string = "boolean                   ";
    break;
     case 'c':
	 method_string = "constant                ";
	  break;
	  case 'd': 
   method_string = "decreasing                ";
    break;
    case 'm':
	method_string = "modulo                   ";
	 break;
	 case 'n':
	method_string = "modulo                   ";
	 break;
	  case 'i': 
    method_string = "increasing               ";
	 break;
	case 'o':
	case 'p':
	case 'q':
	method_string = "inversions               ";
	 break;
	 case 'r':
    method_string = "random                   ";
	 break;
	case 's':
	method_string = "sawtooth                 ";
	 break;
	 case 't':
	method_string = "transposition of identity";
    break;
	case 'u':
	method_string = "step                     ";
    break;
	case 'v':
	method_string = "stairs                   ";
    break;
	case 'w':
	method_string = "eigth_power_mod_power_of_2";
    break;
	case 'z':
	method_string = "squares_mod_power_of_2   ";
    break;
  default:
    std::cerr << "Method not in ['d','i','r','b','m', 's', 'q', 'z', 'c', 't', 'w']" << std::endl;
    usage(argc, argv);
  }

  if(N*sizeof(element) > ((unsigned long long int)1)<<31)
	  return 0;
  
#if defined(MEASURE_MOVES)

  moves = 0;

#elif defined(MEASURE_COMPARISONS)

  comparisons = 0;

#endif

#if defined(REPETITIONS)

  int const repetitions = REPETITIONS;

#else

//for every measurement sort at least almost BYTESTOSORT bytes -- to ensure that the elapsed time is someting around 1 second (this is not always the case, but often)
  unsigned int const repetitions = std::max((unsigned int)(BYTESTOSORT / ((unsigned long long int)N*sizeof(element))), (unsigned int)1);

#endif

	unsigned long long int temp;
	int elements = N;
    unsigned long long int modulo_power_2 =  1 << ((unsigned int)ilogb(elements)); 
	unsigned long long int offset_zw = modulo_power_2 / 2;

	typedef std::vector<element> tv;
	std::vector<tv> test_data;
	std::vector<tv> test_data_sorted;
	std::mt19937 random_generator(seed);

	//generate test data
	//  std::cout << "Generating " << repeats << " times " << elements << " elements of test data..." << std::endl;
	test_data.clear();
	test_data.reserve(repetitions);
	for (unsigned int j = 0; j < repetitions; j++) {
		test_data.push_back(tv());
		test_data_sorted.push_back(tv());
		test_data[j].reserve(elements);
		test_data_sorted[j].reserve(elements);
		switch (method) {
		case 'd':
			for (int i = 0; i < elements; i++) {
			  test_data[j].push_back(element(elements-1-i));
			  test_data_sorted[j].push_back(element(elements-1-i));
			}
			break;
		case 'c':
			for (int i = 0; i < elements; i++) {
			  test_data[j].push_back(element(1));
			  test_data_sorted[j].push_back(element(1));
			}
			break;
		case 'i':
			for (int i = 0; i < elements; i++) {
			  test_data[j].push_back(element(i));
			  test_data_sorted[j].push_back(element(i));
			}
			break;
		case 'b':
			for (int i = 0; i < elements; i++) {
				int el = random_generator() % 2;
			  test_data[j].push_back(element(el));
			  test_data_sorted[j].push_back(element(el));
			}
			break;
		case 'r':
			for (int i = 0; i < elements; i++) {
			  test_data[j].push_back(element(i));
			  test_data_sorted[j].push_back(element(i));
			}
			std::shuffle(test_data[j].begin(), test_data[j].end(),random_generator);
			break;
		case 'm':
		case 'n':
			for (int i = 0; i < elements; i++) {
			  test_data[j].push_back(element(i % modulo));
			  test_data_sorted[j].push_back(element(i % modulo));
			}
			std::shuffle(test_data[j].begin(), test_data[j].end(),random_generator);
			break;
		case 's':
			for (int i = 0; i < elements; i++) {
			  test_data[j].push_back(element(i % modulo));
			  test_data_sorted[j].push_back(element(i % modulo));
			}
			break;		
		case 'o':
		case 'p':
		case 'q':
			for (int i = 0; i < elements; i++) {
			  test_data[j].push_back(element(i));
			  test_data_sorted[j].push_back(element(i));
			}
			for (unsigned int i = 0; i < inversions; i++) {
				int pos = random_generator() % (elements - 1);
			  std::swap(test_data[j][pos],test_data[j][pos + 1]);
			  std::swap(test_data_sorted[j][pos],test_data_sorted[j][pos + 1]);
			}
			break;
		case 't':
			for (int i = 0; i < elements; i++) {
			  test_data[j].push_back(element((i + offset) % elements));
			  test_data_sorted[j].push_back(element((i + offset) % elements));
			}
			break;
		case 'u':
			for (int i = 0; i < elements; i++) {
			  test_data[j].push_back(element(i / (elements/2)));
			  test_data_sorted[j].push_back(element(i / (elements/2)));
			}
			break;
		case 'v':
			for (int i = 0; i < elements; i++) {
				test_data[j].push_back(element(i / modulo));
				test_data_sorted[j].push_back(element(i / modulo));
			}
			break;
		case 'w':
			for (int i = 0; i < elements; i++) {
				temp = ((long long int)i*(long long int)i) % modulo_power_2;
				temp = ((long long int)temp*(long long int)temp) % modulo_power_2;
			  test_data[j].push_back(element((offset_zw + (long long int)temp*(long long int)temp) % modulo_power_2));
			  test_data_sorted[j].push_back(element((offset_zw + (long long int)temp*(long long int)temp) % modulo_power_2));
			}
			break;	
		case 'z':
			unsigned long long int offset_zw = modulo_power_2 / 2;
			for (int i = 0; i < elements; i++) {
			  test_data[j].push_back(element((offset_zw + (long long int)i*(long long int)i) % modulo_power_2));
			  test_data_sorted[j].push_back(element((offset_zw + (long long int)i*(long long int)i) % modulo_power_2));
			}
			break;		
		}
	}


#if ! defined(MEASURE_COMPARISONS) && ! defined(MEASURE_MOVES)

  auto begin = std::chrono::high_resolution_clock::now();

#endif
	
  for (unsigned int j = 0; j < repetitions; ++j)  {
    NAME::sort(test_data[j].begin(), test_data[j].end(), C());
  }


#if ! defined(MEASURE_COMPARISONS) && ! defined(MEASURE_MOVES)

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
#endif

#if ! defined(NDEBUG)

  std::cout << "Sorting test data..." << std::endl;
  for (unsigned int j = 0; j < repetitions; j++) {
    std::sort(test_data_sorted[j].begin(), test_data_sorted[j].end(), C());
  }

  for (unsigned int j = 0; j < repetitions; ++j)  {
    for (int i = 0; i < elements; i++) {
      if (test_data[j][i] != test_data_sorted[j][i]) 
	  {
		  std::cout << "Not sorted correctly:" << i << std::endl;
		  break;
	  }
    }
  }

#endif
  
  double T = double(repetitions) * double(N);  
  	std::string type_string;
	if (typeid(element) == typeid(uint32_t) || typeid(element) == typeid(int))
		type_string = std::string("int");
	else if (typeid(element) == typeid(Record))
		type_string = std::string("Record");
  	else if (typeid(element) == typeid(double))
		type_string = std::string("double");
	else if (typeid(element) == typeid(Vector))
		type_string = std::string("Vector");
	else if (typeid(element) == typeid(Rational))
		type_string = std::string("Rational");
	else if (typeid(element) == typeid(std::string))
		type_string = std::string("string");

#if defined(MEASURE_COMPARISONS)
//prints N, #comparisons/N ,  #comparisons/(NlogN ) ,  (#comparisons-NlogN)/N
  double avg_comp = double(comparisons) / double(repetitions);
  double c = ((double) avg_comp/(N * log2((double)N)));
  double d = ((double) (avg_comp - (N * log2((double)N))) / (double)N);
  std::cout.precision(5);  
  std::cout << N << '\t' << double(comparisons) / T;
  std::cout.precision(3); 
  std::cout << '\t' << c;
  std::cout << '\t' << d << std::endl;

#elif defined(MEASURE_MOVES)
//prints N, #moves/N ,  #moves/(NlogN )
  std::cout.precision(4);  
  double avg_move = double(moves) / double(repetitions);
  double c = ((double) avg_move/(N * log2((double)N)));
  std::cout << N << '\t' << double(moves) / T;
  std::cout << '\t' << c<< std::endl;

#else
	
  double ns = double(duration) ; 
  std::cout.precision(4);
  std::cout.width(10);
#ifdef BLOCKSIZETEST
 std::cout << type_string << '\t' << BLOCKSIZE <<'\t'  <<  N << '\t' << ns / T <<'\t' << partial_sort_count <<'\t' << repetitions << std::endl;
#else	
  std::cout<<ALG_NAME<<'\t'<< method_string << '\t'<< param_string << '\t'  << type_string << '\t'<< N << '\t' << ns / T <<'\t' << partial_sort_count<<'\t' << repetitions << std::endl;
#endif
#endif

  return 0;
}
