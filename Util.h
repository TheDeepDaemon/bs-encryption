#ifndef UTIL_H
#define UTIL_H
#include"Definitions.h"
#include"random_util.h"

using std::string;


string toString(const vector<uint8>& bytes) {
	string str;
	str.reserve(bytes.size());
	for (uint i = 0; i < bytes.size(); i++) {
		str.push_back(bytes[i]);
	}
	return str;
}


vector<uint8> toBytes(const string& str) {
	vector<uint8> vec;
	vec.reserve(str.size());
	for (uint i = 0; i < str.size(); i++) {
		vec.push_back(str[i]);
	}
	return vec;
}


std::pair<vector<uint8>, string> genDataAndKey() {

	uint keyLength = 10;

	string key(keyLength, ' ');

	for (int i = 0; i < keyLength; i++) {
		key[i] = (char)Random::randInt(UCHAR_MAX);
	}

	vector<uint8> data(30);

	for (uint8& b : data) {
		b = (char)Random::randInt(UCHAR_MAX);
	}

	return std::pair<vector<uint8>, string>(data, key);
}


std::mt19937_64 getShuffleBytesGenerator(const uint64 seed) {
	std::mt19937_64 gen(seed ^ 15610115710395554844U);
	for (int i = 0; i < 12; i++) {
		uint _n_ = gen();
	}
	return gen;
}


void shuffleBytes(DataBlock& block, const uint32 seed) {
	std::mt19937_64 gen = getShuffleBytesGenerator(seed);
	std::shuffle(block.data, block.data + BLOCK_SIZE, gen);
}


template<typename T>
void shuffleVector(std::vector<T>& vec, const uint32 seed) {
	std::mt19937_64 gen = getShuffleBytesGenerator(seed);
	std::shuffle(vec.begin(), vec.end(), gen);
}


template<typename T>
void printBits(const T& x) {
	for (size_t i = 0; i < sizeof(T); ++i) {
		std::cout << std::bitset<8>(reinterpret_cast<const uint8*>(&x)[sizeof(T) - 1 - i]);
	}
}


void printBlockBits(const DataBlock& block) {
	for (unsigned i = 0; i < BLOCK_LEN; i++) {
		for (unsigned j = 0; j < BLOCK_LEN; j++) {
			const uint8 x = block.get(i, j);
			printBits(x);
			cout << " ";
		}
		cout << "\n";
	}
}

void printBlockBits32(const DataBlock& block) {
	for (unsigned i = 0; i < BLOCK_LEN; i++) {
		const uint32 x = block.getRow(i);
		printBits(x);
		cout << "\n";
	}
}


template<typename I_to, typename I_from>
void convertInt(I_to* numTo, const I_from& numFrom) {
	const unsigned arrSize = sizeof(I_from) / sizeof(I_to);
	union {
		I_from nFrom;
		I_to nTo[arrSize];
	} ints;
	ints.nFrom = numFrom;
	memcpy(numTo, ints.nTo, arrSize);
}


void applyXOR(vector<uint8>& data, const vector<uint8>& key) {
	for (size_t i = 0; i < data.size(); i++) {
		size_t k = i % key.size();
		data[i] = data[i] ^ key[k];
	}
}



template<typename T>
void swap(std::vector<T>& vec, const size_t i1, const size_t i2) {
	T temp = vec[i1];
	vec[i1] = vec[i2];
	vec[i2] = temp;
}


template<typename T>
void nonStdShuffle(std::vector<T>& vec) {
	for (size_t i = vec.size() - 1; i > 0; i--) {
		size_t r = Random::randInt(i + 1);
		swap(vec, i, r);
	}
}




#endif // !UTIL_H