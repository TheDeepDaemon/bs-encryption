#ifndef STORED_DATA_H
#define STORED_DATA_H
#include"Definitions.h"
#include"ByteMapping.h"
#include"FileBuffer.h"
#include"random_util.h"




struct StoredData {

	std::vector<ByteMappingGrid> mappings;

	void loadData(const string& fname) {

		FileBuffer buffer(fname);

		unsigned numMappings = (unsigned)buffer.getNextByte();

		mappings.reserve(numMappings);

		for (unsigned i = 0; i < numMappings; i++) {
			vector<uint8> bmg = buffer.getNextBytes(ByteMappingGrid::NUM_BYTES);
			mappings.push_back(ByteMappingGrid(bmg));
		}
	}

	void save(const string& fname) {

		vector<uint8> buffer;
		uint buffSize = (mappings.size() * ByteMappingGrid::NUM_BYTES) + 1;
		buffer.reserve(buffSize);

		_ASSERT(mappings.size() <= UINT8_MAX);
		uint8 numMappings = (uint8)mappings.size();
		buffer.push_back(numMappings);

		for (size_t i = 0; i < mappings.size(); i++) {
			concat(buffer, mappings[i].getSaveData());
		}

		dump(fname, buffer);
	}

	void genRandomData() {
		mappings.clear();
		mappings.reserve(NUM_ROUNDS);
		for (uint i = 0; i < NUM_ROUNDS; i++) {
			mappings.push_back(ByteMappingGrid::genByteMappingGrid());
		}
	}

	StoredData(const string& fname) {
		loadData(fname);
	}

	StoredData() : mappings() {
	}

};


#endif // !STORED_DATA_H