#ifndef ENCRYPT_H
#define ENCRYPT_H
#include"DataBlock.h"
#include"ByteMapping.h"
#include"random_util.h"
#include"BitShuffle.h"
#include"KeySet.h"
#include"FileBuffer.h"
#include"Util.h"
#include"StoredData.h"
#include<bitset>



uint8 encodeLastByte(const uint8 valueBits, const uint8 paddingBits) {
	return (valueBits & 0xF) | (paddingBits & 0xF0);
}

uint8 decodeLastByte(const uint8 byte) {
	return byte & 0xF;
}


// data -> (data + length)
inline vector<uint8> encodeLength(const vector<uint8>& bytes, const ShaKeySet& keys) {

	// total number of blocks
	const uint numBlocks = bytes.size() / BLOCK_SIZE;

	// number of bytes in last block before padding
	const uint8 paddingN = bytes.size() % BLOCK_SIZE;

	vector<uint8> encoded((numBlocks + 1) * BLOCK_SIZE, 0);

	// where to start copying from the keys
	const uint keysBegin = ((uint)NUM_KEY_ROWS * NUM_KEY_COLS) - BLOCK_SIZE;

	// where in data to start using the keys for padding
	const uint dataBlockBegin = encoded.size() - BLOCK_SIZE;

	// use the key values to fill in the last block
	for (uint i = 0; i < BLOCK_SIZE; i++) {
		encoded[dataBlockBegin + i] = keys.data[keysBegin + i];
	}

	// copy over all data
	for (uint i = 0; i < bytes.size(); i++) {
		encoded[i] = bytes[i];
	}

	// encode the number of padding bytes
	encoded.back() = encodeLastByte(paddingN, encoded.back());

	return encoded;
}


// (data + length) -> data
inline vector<uint8> decodeLength(const uint8* const bytes, const uint size_) {
	_ASSERT(size_ % BLOCK_SIZE == 0);
	uint8 paddingN = decodeLastByte(bytes[size_ - 1]);
	return vector<uint8>(bytes, bytes + size_ - BLOCK_SIZE + paddingN);
}


vector<uint8> encrypt(const vector<uint8>& rawData, const std::string& key, const StoredData storedKeys) {
	_ASSERT(storedKeys.mappings.size() == NUM_ROUNDS);
	ShaKeySet keys(key);

	vector<DataBlock> data = getDataBlocks(encodeLength(rawData, keys));
	uint8* ptr = (uint8*)(void*)data.data();
	uint totalSize = (data.size() * sizeof(DataBlock));

	for (unsigned i = 0; i < NUM_ROUNDS; i++) {

		// bytemapping
		for (unsigned j = 0; j < data.size(); j++) {
			storedKeys.mappings[i].mapBytes(data[j]);
		}

		// shuffle bits
		shuffleBits(ptr, totalSize, keys.data32[i]);
	}

	// return as bytes
	return vector<uint8>(ptr, ptr + totalSize);
	
}


string encrypt(const string& rawData, const std::string& key, const StoredData storedKeys) {
	return bytesToHexString(encrypt(vector<uint8>(rawData.begin(), rawData.end()), key, storedKeys));
}


vector<uint8> decrypt(const vector<uint8>& rawData, const std::string& key, const StoredData storedKeys) {
	_ASSERT(storedKeys.mappings.size() == NUM_ROUNDS);
	ShaKeySet keys(key);

	vector<DataBlock> data = getDataBlocks(rawData);
	uint8* ptr = (uint8*)(void*)data.data();
	uint totalSize = data.size() * sizeof(DataBlock);

	for (unsigned i_ = 0; i_ < NUM_ROUNDS; i_++) {
		unsigned i = NUM_ROUNDS - i_ - 1;

		// unshuffle bits
		invShuffleBits(ptr, totalSize, keys.data32[i]);

		// reverse bytemapping
		for (unsigned j = 0; j < data.size(); j++) {
			storedKeys.mappings[i].invMapBytes(data[j]);
		}
	}

	// return as bytes
	return decodeLength(ptr, totalSize);
}


string decrypt(const string& rawData, const std::string& key, const StoredData storedKeys) {
	vector<uint8> decrypted = decrypt(hexStringToBytes(rawData), key, storedKeys);
	return string(decrypted.begin(), decrypted.end());
}


#endif // !ENCRYPT_H