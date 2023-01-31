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

		uint8 numMappings = mappings.size();
		buffer.push_back(numMappings);

		for (size_t i = 0; i < mappings.size(); i++) {
			concat(buffer, mappings[i].getSaveData());
		}

		dump(fname, buffer);
	}

	void genRandomData(const uint n) {
		mappings.clear();
		mappings.reserve(n);
		for (uint i = 0; i < n; i++) {
			mappings.push_back(ByteMappingGrid::genByteMappingGrid());
		}
	}

};


#endif // !STORED_DATA_H