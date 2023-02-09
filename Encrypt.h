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


vector<uint8> encrypt(const vector<uint8>& rawData, const StoredData& storedKeys, const std::string& key) {
	_ASSERT(storedKeys.mappings.size() == NUM_ROUNDS);
	ShaKeySet keys(key);

	vector<DataBlock> data = getDataBlocks(encodeLength(rawData, keys));

	for (unsigned i = 0; i < NUM_ROUNDS; i++) {

		// bytemapping
		for (unsigned j = 0; j < data.size(); j++) {
			storedKeys.mappings[i].mapBytes(data[j]);
		}

		// seed random number generator
		std::mt19937_64 gen = getShuffleBitsGenerator(keys.data[i]);

		for (unsigned j = 0; j < data.size(); j++) {
			shuffleBitsAsBitArray(data[j].data, sizeof(DataBlock), gen);
		}

	}

	// return as bytes
	uint8* ptr = (uint8*)(void*)data.data();
	uint totalSize = (data.size() * sizeof(DataBlock));
	return vector<uint8>(ptr, ptr + totalSize);
}


string encrypt(const string& rawData, const StoredData& storedKeys, const std::string& key) {
	return bytesToHexString(encrypt(vector<uint8>(rawData.begin(), rawData.end()), storedKeys, key));
}


vector<uint8> encryptAsHex(const vector<uint8>& rawData, const StoredData& storedKeys, const std::string& key) {
	return bytesToHex(encrypt(rawData, storedKeys, key));
}


vector<uint8> decrypt(const vector<uint8>& rawData, const StoredData& storedKeys, const std::string& key) {
	_ASSERT(storedKeys.mappings.size() == NUM_ROUNDS);
	ShaKeySet keys(key);

	vector<DataBlock> data = getDataBlocks(rawData);

	for (unsigned i_ = 0; i_ < NUM_ROUNDS; i_++) {
		unsigned i = NUM_ROUNDS - i_ - 1;

		
		// seed random number generator
		std::mt19937_64 gen = getShuffleBitsGenerator(keys.data[i]);

		for (unsigned j = 0; j < data.size(); j++) {
			invShuffleBits(data[j].data, sizeof(DataBlock), gen);
		}

		// reverse bytemapping
		for (unsigned j = 0; j < data.size(); j++) {
			storedKeys.mappings[i].invMapBytes(data[j]);
		}
	}

	// return as bytes
	uint8* ptr = (uint8*)(void*)data.data();
	uint totalSize = data.size() * sizeof(DataBlock);
	return decodeLength(ptr, totalSize);
}


string decrypt(const string& rawData, const StoredData& storedKeys, const std::string& key) {
	vector<uint8> decrypted = decrypt(hexStringToBytes(rawData), storedKeys, key);
	return string(decrypted.begin(), decrypted.end());
}


vector<uint8> decryptFromHex(const vector<uint8>& rawData, const StoredData& storedKeys, const std::string& key) {
	return decrypt(hexToBytes(rawData), storedKeys, key);
}


inline void processFile(
	const string& inputFileName,
	const string& outputFileName,
	const string& storedKeysFile,
	const string& key,
	vector<uint8> processFunction(const vector<uint8>&, const StoredData&, const std::string&)) {
	const vector<uint8> bytes = slurp<uint8>(inputFileName);
	StoredData storedKeys(storedKeysFile);
	dump<uint8>(outputFileName, processFunction(bytes, storedKeys, key));
}


void encryptFile(const string& inputFileName, const string& outputFileName, const string& storedKeysFile, const string& key) {
	processFile(inputFileName, outputFileName, storedKeysFile, key, encrypt);
}


void decryptFile(const string& inputFileName, const string& outputFileName, const string& storedKeysFile, const string& key) {
	processFile(inputFileName, outputFileName, storedKeysFile, key, decrypt);
}


inline void processFileAsHex(
	const string& inputFileName,
	const string& outputFileName,
	const string& storedKeysFile,
	const string& key,
	string processFunction(const string&, const StoredData&, const std::string&)) {
	const string data = slurps(inputFileName);
	StoredData storedKeys(storedKeysFile);
	dump(outputFileName, processFunction(data, storedKeys, key));
}


void encryptFileAsHex(const string& inputFileName, const string& outputFileName, const string& storedKeysFile, const string& key) {
	processFileAsHex(inputFileName, outputFileName, storedKeysFile, key, encrypt);
}


void decryptFileAsHex(const string& inputFileName, const string& outputFileName, const string& storedKeysFile, const string& key) {
	processFileAsHex(inputFileName, outputFileName, storedKeysFile, key, decrypt);
}


#endif // !ENCRYPT_H