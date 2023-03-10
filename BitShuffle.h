#ifndef BITSHUFFLE_H
#define BITSHUFFLE_H
#include"Definitions.h"
#include"functional_util.h"
#include"Util.h"
#include"BitArray.h"


bool getBit(const uint8 n, const int offset) {
	return (n & ((uint8)1 << offset)) >> offset;
}


std::vector<bool> getBits(const uint8* bytes, const uint size) {
	std::vector<bool> bits;
	bits.reserve(size * 8);

	for (size_t i = 0; i < size; i++) {
		const uint8 n = bytes[i];
		for (int j = 7; j >= 0; j--) {
			bits.push_back(getBit(n, j));
		}
	}

	return bits;
}


void setBoolArrFromBits(bool* copyTo, const uint8* bytes, const uint size) {
	uint boolArr = size * 8;

	for (size_t i = 0; i < size; i++) {
		const uint8 n = bytes[i];
		for (int j = 7, k = 0; j >= 0; j--, k++) {
			copyTo[(i * 8) + k] = getBit(n, j);
		}
	}
}


void setBytesFromBits(uint8* bytes, const uint numBytes, const bool* bits) {
	for (size_t i = 0; i < numBytes; i++) {
		const size_t index = i * 8;

		uint8 n = 0;

		for (int j = 0; j < 8; j++) {
			bool b = bits[index + j];
			n = n | (b << (7 - j));
		}

		bytes[i] = n;
	}
}


template<typename T>
std::vector<T> applyPermutation(const std::vector<T>& vec, const std::vector<uint>& permutation) {
	_ASSERT(vec.size() == permutation.size());
	std::vector<T> permuted;
	permuted.reserve(vec.size());

	for (uint i = 0; i < vec.size(); i++) {
		permuted.push_back(vec[permutation[i]]);
	}

	return permuted;
}


template<typename T>
void applyPermutation(T* arr, const std::vector<uint>& permutation) {
	uint n = permutation.size();
	T* tempArr = new T[n];
	for (uint i = 0; i < n; i++) {
		tempArr[i] = arr[i];
	}

	for (uint i = 0; i < n; i++) {
		arr[i] = tempArr[permutation[i]];
	}

	delete[] tempArr;
}


std::vector<uint> getInvPermutation(const std::vector<uint>& permutationVector) {
	std::vector<uint> invPerm(permutationVector.size(), 0);

	for (uint i = 0; i < permutationVector.size(); i++) {
		invPerm[permutationVector[i]] = i;
	}

	return invPerm;
}


std::mt19937_64 getShuffleBitsGenerator(const uint64 seed) {
	std::mt19937_64 gen(seed ^ 7843300753556624517U);
	for (int i = 0; i < 16; i++) {
		uint _n_ = gen();
	}
	return gen;
}


void shuffleBits(uint8* bytes, const uint numBytes, const uint32 seed, bool* tempArr) {
	// seed random number generator
	std::mt19937_64 gen = getShuffleBitsGenerator(seed);

	// get array of bits
	setBoolArrFromBits(tempArr, bytes, numBytes);

	// shuffle it
	shuffleArray(tempArr, numBytes * 8, gen);

	// convert to bytes
	setBytesFromBits(bytes, numBytes, tempArr);
}


void shuffleBitsSaveMemory(uint8* bytes, const uint numBytes, const uint32 seed) {
	// seed random number generator
	std::mt19937_64 gen = getShuffleBitsGenerator(seed);

	// shuffle it
	shuffleBitsAsBitArray(bytes, numBytes, gen);
}


void invShuffleBits(uint8* bytes, const uint numBytes, std::mt19937_64& gen) {
	// get the array of bits
	bool* bits = new bool[numBytes * 8];
	setBoolArrFromBits(bits, bytes, numBytes);

	// get the permutation vector:
	// vector[0] = 0, vector[1] = 1, vector[2] = 2, ...
	std::vector<uint> perm = getPermutationVector(numBytes * 8);

	// shuffle the permutation vector
	shuffleArray(perm.data(), perm.size(), gen);

	// get the inverse of the permutation
	std::vector<uint> invPerm = getInvPermutation(perm);

	// apply it to the bits
	applyPermutation(bits, invPerm);

	// convert to bytes
	setBytesFromBits(bytes, numBytes, bits);

	delete[] bits;
}


void invShuffleBits(uint8* bytes, const uint numBytes, const uint32 seed) {
	// seed random number generator
	std::mt19937_64 gen = getShuffleBitsGenerator(seed);
	invShuffleBits(bytes, numBytes, gen);
}


#endif // !BITSHUFFLE_H