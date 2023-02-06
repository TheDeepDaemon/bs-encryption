#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H
#include"Definitions.h"
#include"Util.h"


class BitArray {
private:

	vector<uint8> bytes;

	uint8 shiftBit1(const uint bitIndex) {
		return ((uint8)1 << (8 - bitIndex - 1));
	}

	void setBitTo1(const uint index) {
		uint byteIndex = index / 8;
		uint bitIndex = index % 8;

		uint8 shifted = shiftBit1(bitIndex);

		bytes[byteIndex] = bytes[byteIndex] | shifted;
	}

	void setBitTo0(const uint index) {
		uint byteIndex = index / 8;
		uint bitIndex = index % 8;

		uint8 shifted = shiftBit1(bitIndex);
		shifted = shifted ^ 0xFF;

		bytes[byteIndex] = bytes[byteIndex] & shifted;
	}

public:

	BitArray(const uint8* ptr, const uint size) : bytes(size) {
		for (uint i = 0; i < size; i++) {
			bytes[i] = ptr[i];
		}
	}

	BitArray(const vector<uint8>& vec) : bytes(vec) {
	}

	BitArray(const vector<bool>& vec) {
		if (vec.size() > 0) {
			uint size = ((vec.size() - 1) / 8) + 1;
			bytes = vector<uint8>(size, 0);
			for (uint i = 0; i < vec.size(); i++) {
				if (vec[i]) {
					setBitTo1(i);
				}
			}
		}
	}

	void setBit(const uint index, const bool bitValue) {
		if (bitValue) {
			setBitTo1(index);
		}
		else {
			setBitTo0(index);
		}
	}

	bool getBit(const uint index) const {
		uint byteIndex = index / 8;
		uint bitIndex = index % 8;

		uint8 bit = (bytes[byteIndex] >> (8 - bitIndex - 1)) & (uint8)1;

		return (bool)bit;
	}

	uint size() const {
		return bytes.size() * 8;
	}

	void swap(const uint i1, const uint i2) {
		bool temp = getBit(i1);
		setBit(i1, getBit(i2));
		setBit(i2, temp);
	}

	void shuffle(std::mt19937_64& gen) {
		uint start = size() - 1;
		for (size_t i = start; i > 0; i--) {
			size_t r = gen() % (i + 1);
			swap(i, r);
		}
	}

	void print() {
		for (uint i = 0; i < bytes.size(); i++) {
			printBits(bytes[i]);
			if (i < bytes.size() - 1) {
				cout << ' ';
			}
			else {
				cout << '\n';
			}
		}
	}

	vector<uint8> getByteArray() {
		return bytes;
	}

	void copyBytesTo(uint8* ptr) {
		for (uint i = 0; i < bytes.size(); i++) {
			ptr[i] = bytes[i];
		}
	}

};


void shuffleBitsAsBitArray(uint8* ptr, const uint size, std::mt19937_64& gen) {
	BitArray bitArray(ptr, size);
	bitArray.shuffle(gen);
	bitArray.copyBytesTo(ptr);
}


#endif // !BIT_ARRAY_H