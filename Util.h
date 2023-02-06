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


std::pair<vector<uint8>, string> genRandomDataAndKey(const uint dataLength, const uint keyLength) {

	string key(keyLength, ' ');

	for (int i = 0; i < keyLength; i++) {
		key[i] = (char)Random::randInt(UCHAR_MAX);
	}

	vector<uint8> data(dataLength);

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


// the Fisher�Yates shuffle, included so std is not necessarily needed
template<typename T>
void shuffleVector(std::vector<T>& vec, std::mt19937_64& gen) {
	for (size_t i = vec.size() - 1; i > 0; i--) {
		size_t r = gen() % (i + 1);
		swap(vec, i, r);
	}
}


template<typename T>
bool vectorsEqual(const vector<T>& vec1, const vector<T>& vec2) {
	if (vec1.size() != vec2.size()) {
		return false;
	}
	for (size_t i = 0; i < vec1.size(); i++) {
		if (vec1[i] != vec2[i]) {
			return false;
		}
	}
	return true;
}


string bytesToHexString(const vector<unsigned char>& str) {
	vector<char> encoded(str.size() * 2);

	string temp;

	for (size_t i = 0; i < str.size(); i++) {
		std::stringstream ss;
		ss << std::hex << (int)str[i];
		ss >> temp;
		size_t i_ = i * 2;
		if (temp.size() > 1) {
			encoded[i_] = temp[0];
			encoded[i_ + 1] = temp[1];
		}
		else {
			encoded[i_] = '0';
			encoded[i_ + 1] = temp[0];
		}
	}

	return string(encoded.begin(), encoded.end());
}


int byteFromHex(const string& str) {
	std::stringstream ss;
	ss << std::hex << str;
	int n;
	ss >> n;
	return n;
}


vector<unsigned char> hexStringToBytes(const string& str) {
	size_t n = str.size() / 2;
	vector<unsigned char> bytes(n);
	for (size_t i = 0; i < n; i++) {
		bytes[i] = byteFromHex(str.substr(i * 2, 2));
	}
	return bytes;
}


#endif // !UTIL_H