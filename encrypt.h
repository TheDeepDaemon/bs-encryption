#ifndef ENCRYPT_H
#define ENCRYPT_H
#include"DataBlock.h"
#include"ByteMapping.h"
#include"random_util.h"
#include"BitShuffle.h"
#include"KeySet.h"
#include"FileBuffer.h"
#include"Util.h"
#include<bitset>

vector<uint8> encrypt(const vector<uint8>& rawData, const std::string& key) {

	ShaKeySet keys(key);

	// break the bytes into chunks
	vector<DataBlock> data = getDataBlocks(encodeLength(rawData));

	// load byte mapping, rounds data
	ByteMappingGrid grid(slurp<uint8>(BYTE_MAP_FNAME));

	// apply byte grid mapping
	for (size_t i = 0; i < data.size(); i++) {
		grid.mapBytes(data[i]);
	}

	// shuffle bits
	union {
		uint32 ints[2];
		uint64 longInt;
	} uint_bytes;
	uint_bytes.ints[0] = keys.get(NUM_KEY_ROWS - 1, 2);
	uint_bytes.ints[1] = keys.get(NUM_KEY_ROWS - 1, 3);
	const uint64 shKey = uint_bytes.longInt;
	shuffleBits((uint8*)data.data(), data.size() * sizeof(DataBlock), shKey);


	// shuffle rounds

	// apply series of rounds


	// return as bytes
	uint8* ptr = (uint8*)(void*)data.data();
	return vector<uint8>(ptr, ptr + (data.size() * sizeof(DataBlock)));
}


vector<uint8> decrypt(const vector<uint8>& rawData, const std::string& key) {

	ShaKeySet keys(key);

	// break the bytes into chunks
	vector<DataBlock> data = getDataBlocks(rawData);


	// shuffle rounds

	// apply series of rounds


	// inverse shuffle bits
	union {
		uint32 ints[2];
		uint64 longInt;
	} uint_bytes;
	uint_bytes.ints[0] = keys.get(NUM_KEY_ROWS - 1, 2);
	uint_bytes.ints[1] = keys.get(NUM_KEY_ROWS - 1, 3);
	const uint64 shKey = uint_bytes.longInt;
	invShuffleBits((uint8*)data.data(), data.size() * sizeof(DataBlock), shKey);

	// load byte mapping
	ByteMappingGrid grid(slurp<uint8>(BYTE_MAP_FNAME));

	// apply byte grid mapping
	for (size_t i = 0; i < data.size(); i++) {
		grid.invMapBytes(data[i]);
	}

	// return as bytes
	uint8* ptr = (uint8*)(void*)data.data();
	return decodeLength(ptr, data.size() * sizeof(DataBlock));
}



#endif // !ENCRYPT_H