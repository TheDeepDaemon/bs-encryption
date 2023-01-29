#ifndef STORED_DATA_H
#define STORED_DATA_H
#include"Definitions.h"
#include"ByteMapping.h"
#include"FileBuffer.h"
#include"random_util.h"


union ShaKeySet;


void applyKey(DataBlock& dataBlock, const uint32 key) {
	union {
		uint8 b[4];
		uint32 k;
	} keyBytes;
	keyBytes.k = key;
	for (unsigned i = 0; i < BLOCK_LEN; i++) {
		for (unsigned j = 0; j < BLOCK_LEN; j++) {
			dataBlock.get(i, j) ^= keyBytes.b[j];
		}
	}
}


struct StoredData {


	unsigned numSteps = 0;
	ByteMappingGrid entryGrid;
	std::vector<ByteMapping> mappings;
	std::vector<DataBlock> hiddenKeys;


	void loadData(const string& fname) {

		FileBuffer buffer(fname);

		unsigned numMappings = (unsigned)buffer.getNextByte();
		unsigned numHiddenKeys = (unsigned)buffer.getNextByte();

		vector<uint8> bmg = buffer.getNextBytes(BLOCK_SIZE * ByteMapping::SIZE);
		entryGrid = ByteMappingGrid(bmg);

		mappings.reserve(numMappings);
		for (uint i = 0; i < numMappings; i++) {
			vector<uint8> byteMapping = buffer.getNextBytes(ByteMapping::SIZE);
			mappings.push_back(byteMapping);
		}

		hiddenKeys.reserve(numHiddenKeys);
		for (uint i = 0; i < numHiddenKeys; i++) {
			vector<uint8> hiddenKey = buffer.getNextBytes(BLOCK_SIZE);
			hiddenKeys.push_back(hiddenKey);
		}

	}


	void save(const string& fname) {

		vector<uint8> buffer;
		uint buffSize = 2 +
			(BLOCK_SIZE * ByteMapping::SIZE) +
			(ByteMapping::SIZE * mappings.size()) +
			(BLOCK_SIZE * hiddenKeys.size());
		buffer.reserve(buffSize);

		buffer.push_back((uint8)mappings.size());
		buffer.push_back((uint8)hiddenKeys.size());

		concat(buffer, entryGrid.getSaveData());

		for (uint i = 0; i < mappings.size(); i++) {
			concat(buffer, vector<uint8>(mappings[i].mapping, mappings[i].mapping + ByteMapping::SIZE));
		}

		for (uint i = 0; i < hiddenKeys.size(); i++) {
			concat(buffer, vector<uint8>(hiddenKeys[i].data, hiddenKeys[i].data + BLOCK_SIZE));
		}

	}

	void genRandomData(const uint n) {

		entryGrid = ByteMappingGrid::genByteMappingGrid();

		for (uint i = 0; i < n; i++) {
			mappings.push_back(ByteMapping::genRandomByteMapping());
		}

		for (uint i = 0; i < n; i++) {
			hiddenKeys.push_back(Random::genRandBytes(BLOCK_SIZE));
		}

	}


	enum class FunctionType {
		MAP_BYTES = 1,
		XOR_BYTES,
		PERMUTE,
		ROTATE,
		SHUFFLE_BYTES,
		SHUFFLE_BITS,
	};


	struct FunctionData {
		FunctionType type;
		uint32 extraData;
		FunctionData(const FunctionType& functionType, const uint64 extraArg) :
			type(functionType), extraData(extraArg) {}
	};


	void applyFunction(DataBlock& block, const FunctionData& fdat) {
		switch (fdat.type)
		{
		case FunctionType::MAP_BYTES:
			mappings[fdat.extraData].mapBytes(block);
			break;
		case FunctionType::XOR_BYTES:
			applyXOR(block, hiddenKeys[fdat.extraData]);
			break;
		case FunctionType::PERMUTE:
			permuteDataBlock(block);
			applyKey(block, fdat.extraData);
			break;
		case FunctionType::ROTATE:
			rotateBits(block);
			applyKey(block, fdat.extraData);
			break;
		case FunctionType::SHUFFLE_BYTES:
			//shuffleBytes(block, fdat.extraData);
			break;
		case FunctionType::SHUFFLE_BITS:
			//shuffleBits(block.data, BLOCK_SIZE, fdat.extraData);
			break;
		default:
			break;
		}
	}


	vector<FunctionData> genFunctions(const ShaKeySet& keys, const uint32 seed, const unsigned iterations) {

		vector<FunctionData> transformFuncs;
		transformFuncs.reserve(iterations * 4);

		vector<FunctionData> mixFuncs;
		mixFuncs.reserve(iterations * 4);

		for (unsigned i = 0; i < iterations * 2; i++) {
			transformFuncs.push_back(FunctionData(FunctionType::MAP_BYTES, i));
		}

		for (unsigned i = 0; i < iterations * 2; i++) {
			transformFuncs.push_back(FunctionData(FunctionType::XOR_BYTES, i));
		}


		for (unsigned i = 0; i < iterations; i++) {
			uint32 key = keys.data32[i];
			mixFuncs.push_back(FunctionData(FunctionType::PERMUTE, key));
		}

		for (unsigned i = 0; i < iterations; i++) {
			uint32 key = keys.data32[i];
			mixFuncs.push_back(FunctionData(FunctionType::ROTATE, key));
		}

		for (unsigned i = 0; i < iterations; i++) {
			uint32 key = keys.data32[i];
			mixFuncs.push_back(FunctionData(FunctionType::SHUFFLE_BYTES, key));
		}

		for (unsigned i = 0; i < iterations; i++) {
			uint32 key = keys.data32[i];
			mixFuncs.push_back(FunctionData(FunctionType::SHUFFLE_BITS, key));
		}


		// next: shuffle the vectors and zip() them



	}


};


#endif // !STORED_DATA_H