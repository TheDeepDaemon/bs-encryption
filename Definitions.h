#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#include<iostream>
#include<vector>
#include<cmath>
#include<bitset>
#include<sstream>


typedef uint8_t uint8;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef uint64 uint;


using std::cout;
using std::vector;
using std::string;


const unsigned BLOCK_LEN = 4;
const uint BLOCK_SIZE = BLOCK_LEN * BLOCK_LEN;

const unsigned NUM_ROUNDS = 10;
const unsigned NUM_KEY_ROWS = NUM_ROUNDS;
const unsigned NUM_KEY_COLS = 4;

const int PEPPER_SPACING = 5;

const char* BYTE_MAP_FNAME = "byte_mapping_data.bmk";



inline std::ostream& operator<<(std::ostream& os, const uint8& num) {
	os << (int)num;
	return os;
}


template<typename T>
void printVec(const vector<T>& vec) {
	for (size_t i = 0; i < vec.size(); i++) {
		cout << vec[i] << "  ";
	}
	cout << "\n";
}


#endif // !DEFINITIONS_H