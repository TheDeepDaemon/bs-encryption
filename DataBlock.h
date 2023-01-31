#ifndef DATA_BLOCK_H
#define DATA_BLOCK_H
#include"Definitions.h"
#include"KeySet.h"


union DataBlock {

	uint8 data[BLOCK_SIZE];

	uint32 data32[BLOCK_SIZE / (sizeof(uint32) / sizeof(uint8))];

	uint8& get(const unsigned row, const unsigned col) {
		return data[(row * BLOCK_LEN) + col];
	}

	uint8 get(const unsigned row, const unsigned col) const {
		return data[(row * BLOCK_LEN) + col];
	}

	uint32& getRow(const unsigned row) {
		return data32[row];
	}

	uint32 getRow(const unsigned row) const {
		return data32[row];
	}

	DataBlock(const uint8* ptr) {
		memcpy(data, ptr, BLOCK_SIZE * sizeof(uint8));
	}

	DataBlock(const uint8* ptr, const uint size) {
		memset(data, (uint8)0, BLOCK_SIZE * sizeof(uint8));
		memcpy(data, ptr, std::min(size, BLOCK_SIZE) * sizeof(uint8));
	}

	DataBlock(const vector<uint8>& initVec) {
		memset(data, (uint8)0, BLOCK_SIZE * sizeof(uint8));
		memcpy(data, initVec.data(), std::min(initVec.size(), BLOCK_SIZE) * sizeof(uint8));
	}

	DataBlock(const std::initializer_list<uint8>& initList) {
		unsigned i = 0;
		for (const uint8& num : initList) {
			data[i++] = num;
		}
	}

	void print() const {
		for (unsigned i = 0; i < BLOCK_LEN; i++) {
			for (unsigned j = 0; j < BLOCK_LEN; j++) {
				cout << (int)get(i, j) << "  ";
			}
			cout << "\n";
		}
		cout << "\n";
	}

};


inline vector<DataBlock> getDataBlocks(const vector<uint8>& data) {
	const uint numBlocks = data.size() / BLOCK_SIZE;
	const uint remainder = data.size() % BLOCK_SIZE;

	uint vecSize = numBlocks;
	vecSize += (uint)(remainder > 0);

	vector<DataBlock> blocks;
	blocks.reserve(vecSize);

	for (uint i = 0; i < numBlocks; i++) {
		uint index = i * BLOCK_SIZE;
		blocks.push_back(DataBlock(data.data() + index));
	}

	if (remainder > 0) {
		blocks.push_back(DataBlock(data.data() + (numBlocks * BLOCK_SIZE), remainder));
	}

	return blocks;
}






inline vector<void(*)(DataBlock&)> combineVecs(const vector<void(*)(DataBlock&)>& v1, const vector<void(*)(DataBlock&)>& v2) {
	_ASSERT(v1.size() == v2.size());
	vector<void(*)(DataBlock&)> vec;
	vec.reserve(v1.size() * 2);
	for (uint i = 0; i < v1.size(); i++) {
		vec.push_back(v1[i]);
		vec.push_back(v2[i]);
	}
	return vec;
}

inline void apply(DataBlock& data, const vector<void(*)(DataBlock&)>& functions) {
	for (uint i = 0; i < functions.size(); i++) {
		functions[i](data);
	}
}


inline void applyXOR(DataBlock& block, const DataBlock& key) {
	for (unsigned i = 0; i < BLOCK_SIZE; i++) {
		block.data[i] ^= key.data[i];
	}
}


inline void rotateBits(DataBlock& block) {

	for (unsigned i = 0; i < BLOCK_LEN; i++) {
		uint32 temp = rotateBitsLeft(block.getRow(i), 3);
		block.getRow(i) = temp;
	}

}


inline void rotateBitsInv(DataBlock& block) {

	for (unsigned i = 0; i < BLOCK_LEN; i++) {
		uint32 temp = rotateBitsRight(block.getRow(i), 3);
		block.getRow(i) = temp;
	}

}

void shiftRows(DataBlock& block) {
	for (unsigned i = 0; i < BLOCK_LEN; i++) {
		uint8 row[BLOCK_LEN];
		for (unsigned j = 0; j < BLOCK_LEN; j++) {
			row[j] = block.get(i, (j + i) % BLOCK_LEN);
		}

		for (unsigned j = 0; j < BLOCK_LEN; j++) {
			block.get(i, j) = row[j];
		}

	}
}


void shiftCols(DataBlock& block) {
	for (unsigned i = 0; i < BLOCK_LEN; i++) {
		uint8 row[BLOCK_LEN];
		for (unsigned j = 0; j < BLOCK_LEN; j++) {
			row[j] = block.get((i + 1) % BLOCK_LEN, j);
		}

		for (unsigned j = 0; j < BLOCK_LEN; j++) {
			block.get(i, j) = row[j];
		}

	}
}


inline void permuteDataBlock(DataBlock& block) {
	shiftRows(block);
	shiftCols(block);
}


#endif // !DATA_BLOCK_H