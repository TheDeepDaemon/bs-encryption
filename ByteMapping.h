#ifndef BYTE_MAPPING_H
#define BYTE_MAPPING_H
#include"Definitions.h"
#include"file_util.h"
#include"DataBlock.h"
#include"functional_util.h"
#include"random_util.h"
#include<random>



struct ByteMapping {
	static const int SIZE = 1 << 8;

	uint8 mapping[SIZE];
	uint8 inverse[SIZE];

	template<typename T>
	void load(const vector<T>& data) {
		vector<bool> tracker(SIZE, false);

		for (size_t i = 0; i < SIZE; i++) {
			uint8 c = (uint8)data[i];

			if (!tracker[c]) {
				mapping[i] = c;

				inverse[c] = (uint8)i;

				tracker[c] = true;
			}
			else {
				cout << "Not all chars loaded are unique.\n";
				_ASSERT(false);
			}
		}
	}


	ByteMapping(const std::string& dataFileName) {
		vector<char> data = slurp(dataFileName);

		memset(mapping, NULL, SIZE);
		memset(inverse, NULL, SIZE);

		if (data.size() == SIZE) {
			load(data);
		}
		else {
			cout << "The file loaded was the wrong size.\n";
			_ASSERT(false);
		}
	}

	ByteMapping(const vector<uint8>& data) {
		load(data);
	}

	ByteMapping() {
		load(getPermutationVector<uint8>(SIZE));
	}


	void save(const std::string& fname) {
		dump<uint8>(fname, mapping, SIZE);
	}

	uint8 mapByte(const uint8& c) const {
		return mapping[c];
	}

	uint8 invMapByte(const uint8& c) const {
		return inverse[c];
	}

	vector<uint8> mapBytes(const vector<uint8>& bytes) {
		vector<uint8> mapped;
		mapped.reserve(bytes.size());
		for (size_t i = 0; i < bytes.size(); i++) {
			mapped.emplace_back((uint8)mapping[bytes[i]]);
		}
		return mapped;
	}

	vector<char> mapBytes(const vector<char>& bytes) {
		vector<char> mapped;
		mapped.reserve(bytes.size());
		for (size_t i = 0; i < bytes.size(); i++) {
			mapped.emplace_back((uint8)mapping[(uint8)bytes[i]]);
		}
		return mapped;
	}

	void mapBytes(DataBlock& dataBlock) {
		for (uint i = 0; i < BLOCK_SIZE; i++) {
			dataBlock.data[i] = mapByte(dataBlock.data[i]);
		}
	}

	vector<uint8> invMapBytes(const vector<uint8>& bytes) {
		vector<uint8> mapped;
		mapped.reserve(bytes.size());
		for (size_t i = 0; i < bytes.size(); i++) {
			mapped.emplace_back((uint8)inverse[bytes[i]]);
		}
		return mapped;
	}

	vector<char> invMapBytes(const vector<char>& bytes) {
		vector<char> mapped;
		mapped.reserve(bytes.size());
		for (size_t i = 0; i < bytes.size(); i++) {
			mapped.emplace_back((uint8)inverse[(uint8)bytes[i]]);
		}
		return mapped;
	}

	void invMapBytes(DataBlock& dataBlock) {
		for (uint i = 0; i < BLOCK_SIZE; i++) {
			dataBlock.data[i] = invMapByte(dataBlock.data[i]);
		}
	}

	static vector<uint8> genRandomBytePerm() {
		vector<uint8> mapping(ByteMapping::SIZE, (uint8)0);

		for (size_t i = 0; i < mapping.size(); i++) {
			mapping[i] = (uint8)i;
		}

		Random::shuffle(mapping);

		for (size_t i = 0; i < mapping.size(); i++) {
			if (mapping[i] == (uint8)i) {
				return vector<uint8>();
			}
		}

		return mapping;
	}

	static vector<uint8> genRandomByteMapping() {

		vector<uint8> bytes;

		while (bytes.size() == 0) {
			bytes = genRandomBytePerm();
		}

		return bytes;
	}

};



struct ByteMappingGrid {
	static const int rows = BLOCK_LEN;
	static const int cols = BLOCK_LEN;
	static const int GRID_SIZE = rows * cols;
	static const int NUM_BYTES = GRID_SIZE * ByteMapping::SIZE;

	ByteMapping data[GRID_SIZE];

	ByteMapping& get(const unsigned row, const unsigned col) {
		return data[(row * cols) + col];
	}

	const ByteMapping& get(const unsigned row, const unsigned col) const {
		return data[(row * cols) + col];
	}

	ByteMappingGrid() {
		for (int i = 0; i < GRID_SIZE; i++) {
			data[i] = ByteMapping();
		}
	}

	ByteMappingGrid(const vector<uint8>& bytes) {

		const uint8* ptr = bytes.data();

		for (int i = 0; i < GRID_SIZE; i++) {

			memset(data[i].mapping, NULL, ByteMapping::SIZE);
			memset(data[i].inverse, NULL, ByteMapping::SIZE);

			data[i].load(vector<uint8>(ptr + (i * ByteMapping::SIZE), ptr + ((i + 1) * ByteMapping::SIZE)));

		}
	}

	uint8 mapByte(const unsigned row, const unsigned col, const uint8 byte) const {
		return get(row, col).mapByte(byte);
	}

	uint8 invMapByte(const unsigned row, const unsigned col, const uint8 byte) const {
		return get(row, col).invMapByte(byte);
	}

	void mapBytes(DataBlock& block) const {
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				block.get(i, j) = mapByte(i, j, block.get(i, j));
			}
		}
	}

	void invMapBytes(DataBlock& block) const {
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				block.get(i, j) = invMapByte(i, j, block.get(i, j));
			}
		}
	}

	vector<uint8> getSaveData() {
		vector<uint8> bytes(GRID_SIZE * ByteMapping::SIZE);

		for (size_t i = 0; i < GRID_SIZE; i++) {
			for (size_t j = 0; j < ByteMapping::SIZE; j++) {
				bytes[(i * ByteMapping::SIZE * sizeof(uint8)) + j] = data[i].mapping[j];
			}
		}
		return bytes;
	}

	void save(const string& fname) {
		dump(fname, getSaveData());
	}

	static ByteMappingGrid genByteMappingGrid() {
		ByteMappingGrid grid;
		for (uint i = 0; i < grid.GRID_SIZE; i++) {
			grid.data[i] = ByteMapping::genRandomByteMapping();
		}
		return grid;
	}

};


#endif // !BYTE_MAPPING_H