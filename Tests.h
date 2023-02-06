#ifndef TEST_H
#define TEST_H
#include"Definitions.h"
#include"random_util.h"
#include"functional_util.h"
#include"Util.h"
#include"KeySet.h"
#include"Encrypt.h"


// verify that length encoding is working as intended
bool testLengthEncoding(const uint dataLength, const uint keyLength) {

	std::pair<vector<uint8>, string> dataKeyPair = genRandomDataAndKey(dataLength, keyLength);

	vector<uint8> data = dataKeyPair.first;
	string key = dataKeyPair.second;

	ShaKeySet keySet(key);

	vector<uint8> encoded = encodeLength(data, keySet);

	if (encoded == data) {
		return false;
	}

	vector<uint8> decoded = decodeLength(encoded.data(), encoded.size());

	return (decoded == data);
}


// verify that bit shuffling works correctly
bool testShuffleBits(const uint size) {
	std::pair<vector<uint8>, string> dataKeyPair = genRandomDataAndKey(size, 0);

	const vector<uint8> originalBytes = dataKeyPair.first;
	vector<uint8> bytes = dataKeyPair.first;
	uint8* ptr = bytes.data();

	uint32 seed = (uint32)Random::randInt(UINT32_MAX);

	shuffleBits(ptr, size, seed);

	if (bytes == originalBytes) {
		return false;
	}

	invShuffleBits(ptr, size, seed);

	return bytes == originalBytes;
}


// verify that the hidden keys encrypt correctly
bool testByteMapping() {

	ByteMappingGrid byteMapping = ByteMappingGrid::genByteMappingGrid();

	std::pair<vector<uint8>, string> dataKeyPair = genRandomDataAndKey(BLOCK_SIZE, 0);

	vector<uint8> data = dataKeyPair.first;

	DataBlock block(data);

	byteMapping.mapBytes(block);

	vector<uint8> encrypted(block.data, block.data + BLOCK_SIZE);

	if (encrypted == data) {
		return false;
	}

	byteMapping.invMapBytes(block);

	vector<uint8> decrypted(block.data, block.data + BLOCK_SIZE);

	return (decrypted == data);
}


// test encryption for random data of a specific size and with a specific key length
bool testBytesEncryptionAtSize(const uint dataLength, const uint keyLength) {
	StoredData storedKeys;
	storedKeys.genRandomData(NUM_ROUNDS);

	std::pair<vector<uint8>, string> dataKeyPair = genRandomDataAndKey(dataLength, keyLength);

	vector<uint8> data = dataKeyPair.first;
	string key = dataKeyPair.second;

	vector<uint8> encrypted = encrypt(data, key, storedKeys);

	vector<uint8> decrypted = decrypt(encrypted, key, storedKeys);

	return (data == decrypted);
}


// verify that vectors of bytes are encrypted and decrypted correctly
bool testBytesEncryption() {
	StoredData storedKeys;
	storedKeys.genRandomData(NUM_ROUNDS);

	const string originalStr =
		"Lorem Ipsum, this is a message, this is more text, the red fox jumped over the lazy dog";
	vector<uint8> bytes(originalStr.begin(), originalStr.end());

	vector<uint8> encrypted = encrypt(bytes, "encryption key", storedKeys);

	if (encrypted == bytes) {
		return false;
	}

	vector<uint8> decrypted = decrypt(encrypted, "encryption key", storedKeys);

	return (bytes == decrypted);
}


// verify that strings are encrypted and decrypted correctly
bool testStringEncryption() {
	StoredData storedKeys;
	storedKeys.genRandomData(NUM_ROUNDS);

	const string originalStr =
		"Lorem Ipsum, this is a message, this is more text, the red fox jumped over the lazy dog";

	string encrypted = encrypt(originalStr, "encryption key", storedKeys);

	if (encrypted == originalStr) {
		return false;
	}

	string decrypted = decrypt(encrypted, "encryption key", storedKeys);

	return (originalStr == decrypted);
}


// verify that if encrypted data is corrupted, the decrypted result is corrupted
bool testCorruptedData() {
	StoredData storedKeys;
	storedKeys.genRandomData(NUM_ROUNDS);

	const string originalStr =
		"Lorem Ipsum, this is a message, this is more text, the red fox jumped over the lazy dog";

	string encrypted = encrypt(originalStr, "encryption key", storedKeys);

	encrypted[4] = 'a';

	string decrypted = decrypt(encrypted, "encryption key", storedKeys);

	return !(originalStr == decrypted);
}


// verify that the zero vector does not give a zero vector when encrypted
bool testZeroVector() {
	StoredData storedKeys;
	storedKeys.genRandomData(NUM_ROUNDS);

	uint vectorSizeFloor = 10;
	uint vectorSizeCeil = 100;

	// create the null vector
	uint vectorSize = Random::randInt(vectorSizeFloor, vectorSizeCeil);
	vector<uint8> nullVector(vectorSize, 0);

	// encrypt and decrypt
	vector<uint8> encrypted = encrypt(nullVector, "zero vector test encryption key", storedKeys);
	vector<uint8> decrypted = decrypt(encrypted, "zero vector test encryption key", storedKeys);

	// verify
	bool encryptedIsNotZero = (encrypted != vector<uint8>(encrypted.size(), 0));

	bool correctlyDecrypted = (decrypted == nullVector);

	return encryptedIsNotZero && correctlyDecrypted;
}

bool testStoredData() {
	StoredData storedKeys1;
	storedKeys1.genRandomData(NUM_ROUNDS);
	storedKeys1.save("test-filename");

	const string originalStr =
		"Lorem Ipsum, this is a message, this is more text, the red fox jumped over the lazy dog";

	const string encrypted = encrypt(originalStr, "encryption key", storedKeys1);

	if (encrypted == originalStr) {
		return false;
	}

	StoredData storedKeys2;
	storedKeys2.loadData("test-filename");

	const string decrypted = decrypt(encrypted, "encryption key", storedKeys2);

	return (decrypted == originalStr);
}

bool testWrongStoredData() {
	StoredData storedKeys1;
	storedKeys1.genRandomData(NUM_ROUNDS);
	storedKeys1.save("test-filename");

	StoredData wrongStoredKeys;
	wrongStoredKeys.genRandomData(NUM_ROUNDS);
	wrongStoredKeys.save("wrong-test-filename");

	const string originalStr =
		"Lorem Ipsum, this is a message, this is more text, the red fox jumped over the lazy dog";

	const string encrypted = encrypt(originalStr, "encryption key", storedKeys1);

	if (encrypted == originalStr) {
		return false;
	}

	StoredData storedKeys2;
	storedKeys2.loadData("wrong-test-filename");

	const string decrypted = decrypt(encrypted, "encryption key", storedKeys2);

	return (decrypted != originalStr);
}


bool testFileEncryption() {
	string fname = "test-file-encryption.txt";
	string encryptedFname = "test-file-encryption-encrypted.txt";
	string decryptedFname = "test-file-encryption-decrypted.txt";

	StoredData storedKeys;
	storedKeys.genRandomData(NUM_ROUNDS);
	storedKeys.save("test-filename");

	encryptFile(fname, encryptedFname, "encryption-key1", "test-filename");
	decryptFile(encryptedFname, decryptedFname, "encryption-key1", "test-filename");

	string originalFile = slurps(fname);
	string decryptedFile = slurps(decryptedFname);

	return originalFile == decryptedFile;
}


bool runTests() {

	const uint lengthEncodingStartSize = 10;
	const uint lengthEncodingEndSize = 100;
	const uint lengthEncodingKeySize = 8;
	for (int i = lengthEncodingStartSize; i < lengthEncodingEndSize; i++) {
		if (!testLengthEncoding(i, lengthEncodingKeySize)) {
			cout << "Length encoding failed.\n";
			return false;
		}
	}

	const uint shuffleBitsStartSize = 8;
	const uint shuffleBitsEndSize = 20;
	for (uint i = shuffleBitsStartSize; i < shuffleBitsEndSize; i++) {
		if (!testShuffleBits(i)) {
			cout << "Bit shuffling test failed.\n";
			return false;
		}
	}

	if (!testBytesEncryption()) {
		cout << "Bytes encryption failed.\n";
		return false;
	}
	if (!testStringEncryption()) {
		cout << "String encryption failed.\n";
		return false;
	}

	const uint numDataSizes = 10;
	const uint numKeySizes = 10;
	const uint minDataMultiplier = 8;
	const uint dataMultiplierCeil = 100;
	const uint minKeySize = 6;
	for (int i = 0; i < numDataSizes; i++) {
		uint dataSize = i * Random::randInt(minDataMultiplier, dataMultiplierCeil);
		for (int j = 0; j < numKeySizes; j++) {
			int keySize = minKeySize + j;
			if (!testBytesEncryptionAtSize(dataSize, keySize)) {
				cout << "Encryption at data size=" << dataSize << ", and key size=" << keySize << "\n";
				return false;
			}
		}
	}

	if (!testCorruptedData()) {
		cout << "Data corruption test failed.\n";
		return false;
	}

	if (!testZeroVector()) {
		cout << "Zero vector test failed.\n";
		return false;
	}

	if (!testStoredData()) {
		cout << "Stored data test failed.\n";
		return false;
	}

	if (!testWrongStoredData()) {
		cout << "Wrong stored data test failed.\n";
		return false;
	}

	if (!testFileEncryption()) {
		cout << "File encryption test failed.\n";
		return false;
	}

	cout << "Finished tests, passed.\n";
	return true;
}


#endif // !TEST_H