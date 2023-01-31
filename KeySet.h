#ifndef KEYSET_H
#define KEYSET_H
#include<string>
#include"Definitions.h"


template<typename T>
inline T rotateBitsRight(const T& n, const int rot) {
	return (n >> rot) | (n << ((sizeof(T) * 8) - rot));
}


template<typename T>
inline T rotateBitsLeft(const T& n, const int rot) {
	return (n << rot) | (n >> ((sizeof(T) * 8) - rot));
}


union ShaKeySet {

	uint8 data[NUM_KEY_ROWS * NUM_KEY_COLS];
	uint32 data32[NUM_KEY_ROWS];


	uint8& get(unsigned row, unsigned col) {
		return data[(row * NUM_KEY_COLS) + col];
	}


	std::vector<uint8> computeRcon(const uint size) {
		uint8 x = 1;

		vector<uint8> rcon;
		rcon.reserve(size);

		for (uint i = 0; i < size; i++) {
			rcon.push_back(x);
			x = (x << 1) ^ (0x11b & -(x >> 7));
		}

		return rcon;
	}

	ShaKeySet(const std::string& strKey) {
		memset(data, 0, NUM_KEY_ROWS * NUM_KEY_COLS * sizeof(uint8));

		union {
			uint8 bytes[sizeof(uint64)];
			uint64 longInt;
		} firstRow{};

		firstRow.longInt = std::hash<std::string>{}(strKey);

		for (unsigned i = 0; i < NUM_KEY_COLS; i++) {
			get(0, i) = firstRow.bytes[i] ^ firstRow.bytes[4 + i];
		}

		std::vector<uint8> rcon = computeRcon(NUM_KEY_ROWS);

		for (unsigned i = 1; i < NUM_KEY_ROWS; i++) {
			uint8 rotated = rotateBitsRight(get(i - 1, 3), 3);
			get(i, 0) = get(i - 1, 0) ^ rotated ^ rcon[i];
			for (unsigned j = 1; j < NUM_KEY_COLS; j++) {
				get(i, j) = get(i - 1, j) ^ get(i, j - 1);
			}
		}

	}

	void print() {
		for (unsigned i = 0; i < NUM_KEY_ROWS; i++) {
			for (unsigned j = 0; j < NUM_KEY_COLS; j++) {
				cout << get(i, j) << "  ";
			}
			cout << '\n';
		}
		cout << "\n";
	}

};


#endif // !KEYSET_H